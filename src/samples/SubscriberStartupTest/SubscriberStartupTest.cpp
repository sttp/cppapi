// Copyright (c) 2026, Grid Protection Alliance. Licensed under the MIT License.
#include "../../lib/transport/SubscriberInstance.h"
#include "../../lib/Convert.h"
#include "../../lib/EndianConverter.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
using namespace sttp;
using namespace sttp::transport;
using boost::asio::ip::tcp;
using namespace std::chrono_literals;

struct State
{
    std::atomic<int> connections{0}, metadata{0}, errors{0}, early{0}, confirmed{0}, unexpected{0};
};

// Signals defined by the test signal index cache, runtime index is array position.
const Guid Signals[] = {ParseGuid("6f2a0e6c-5a3b-4c1d-9e7f-000000000001"), ParseGuid("6f2a0e6c-5a3b-4c1d-9e7f-000000000002")};

class TestSubscriber final : public SubscriberInstance
{
    State& m_state;
    void ConnectionEstablished() override { ++m_state.connections; }
    void ReceivedMetadata(const std::vector<uint8_t>&) override { ++m_state.metadata; }
    void ReceivedNewMeasurements(const std::vector<MeasurementPtr>& measurements) override
    {
        // Publisher sends value 1 before signal index cache confirmation and value 2 after.
        for (const auto& measurement : measurements)
        {
            if (measurement->SignalID == Signals[1] && measurement->Value == 1.0) ++m_state.early;
            else if (measurement->SignalID == Signals[0] && measurement->Value == 2.0) ++m_state.confirmed;
            else ++m_state.unexpected;
        }
    }
    void StatusMessage(const std::string&) override {}
    void ErrorMessage(const std::string&) override { ++m_state.errors; }
public:
    explicit TestSubscriber(State& state) : m_state(state) {}
};

void Require(bool value, const char* message)
{
    if (!value) throw std::runtime_error(message);
}

template<typename Predicate> void Wait(Predicate predicate)
{
    const auto deadline = std::chrono::steady_clock::now() + 5s;
    while (!predicate())
    {
        Require(std::chrono::steady_clock::now() < deadline, "Timed out waiting for subscriber");
        std::this_thread::sleep_for(1ms);
    }
}

// Read complete command frames with a deadline, including fragmented TCP reads.
std::vector<uint8_t> ReadCommand(tcp::socket& socket, std::chrono::milliseconds timeout)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    std::vector<uint8_t> packet;
    size_t expected = 4;
    while (packet.size() < expected)
    {
        if (socket.available())
        {
            uint8_t data[4096];
            const size_t count = socket.read_some(boost::asio::buffer(data, std::min(sizeof(data), expected - packet.size())));
            packet.insert(packet.end(), data, data + count);
            if (packet.size() == 4)
            {
                expected = 4 + (uint32_t(packet[0]) << 24 | uint32_t(packet[1]) << 16 | uint32_t(packet[2]) << 8 | packet[3]);
                Require(expected > 4 && expected <= 65536, "Invalid command frame length");
            }
        }
        else
        {
            if (std::chrono::steady_clock::now() >= deadline)
            {
                Require(packet.empty(), "Incomplete command frame");
                return {};
            }
            std::this_thread::sleep_for(1ms);
        }
    }
    return packet;
}

void Respond(tcp::socket& socket, uint8_t command, bool accepted = true)
{
    // Four-byte frame size, response code, command code, four-byte body size.
    const uint8_t packet[] = {0, 0, 0, 6, uint8_t(accepted ? 0x80 : 0x81), command, 0, 0, 0, 0};
    boost::asio::write(socket, boost::asio::buffer(packet));
}

void Send(tcp::socket& socket, uint8_t response, const std::vector<uint8_t>& body)
{
    // Four-byte frame size, response code, subscribe command code, four-byte body size, body.
    std::vector<uint8_t> packet;
    EndianConverter::WriteBigEndianBytes(packet, uint32_t(body.size() + 6));
    packet.push_back(response);
    packet.push_back(0x02);
    EndianConverter::WriteBigEndianBytes(packet, uint32_t(body.size()));
    packet.insert(packet.end(), body.begin(), body.end());
    boost::asio::write(socket, boost::asio::buffer(packet));
}

void SendSignalIndexCache(tcp::socket& socket, uint8_t cacheIndex)
{
    std::vector<uint8_t> body{cacheIndex};
    EndianConverter::WriteBigEndianBytes(body, uint32_t(0)); // Binary length, unused by subscriber.
    WriteBytes(body, Empty::Guid);                           // Subscriber ID.
    EndianConverter::WriteBigEndianBytes(body, int32_t(2));  // Reference count.
    for (int32_t index = 0; index < 2; ++index)
    {
        EndianConverter::WriteBigEndianBytes(body, index);
        WriteBytes(body, Signals[index]);
        EndianConverter::WriteBigEndianBytes(body, uint32_t(3));
        body.insert(body.end(), {'P', 'P', 'A'});
        EndianConverter::WriteBigEndianBytes(body, uint64_t(index + 1));
    }
    EndianConverter::WriteBigEndianBytes(body, uint32_t(0)); // Unauthorized signal count.
    Send(socket, 0x83, body);
}

void SendMeasurement(tcp::socket& socket, int cacheIndex, int32_t runtimeIndex, float value)
{
    // Compact format data packet, flagged with cache index, holding one measurement with full timestamp.
    std::vector<uint8_t> body{uint8_t(0x02 | (cacheIndex ? 0x10 : 0x00))};
    EndianConverter::WriteBigEndianBytes(body, int32_t(1));
    body.push_back(0);
    EndianConverter::WriteBigEndianBytes(body, runtimeIndex);
    EndianConverter::WriteBigEndianBytes(body, value);
    EndianConverter::WriteBigEndianBytes(body, int64_t(638900000000000000LL));
    Send(socket, 0x82, body);
}

void Run(bool reverse, bool parse, int version, const std::string& handshake, bool reconnect)
{
    boost::asio::io_context service;
    tcp::acceptor acceptor(service, tcp::endpoint(boost::asio::ip::address_v4::loopback(), 0));
    const uint16_t port = acceptor.local_endpoint().port();
    State state; // Outlives callbacks and subscriber destruction.
    auto subscriber = std::make_unique<TestSubscriber>(state);
    subscriber->Initialize("127.0.0.1", port);
    subscriber->SetAutoReconnect(reconnect);
    subscriber->SetRetryInterval(50);
    subscriber->SetAutoParseMetadata(parse);
    subscriber->SetVersion(static_cast<uint8_t>(version));
    subscriber->SetOperationalModesResponseTimeout(500);
    subscriber->SetSignalIndexCacheCompressed(false);
    if (reverse)
    {
        acceptor.close();
        Require(subscriber->Listen("127.0.0.1", port), "Reverse listener failed");
    }
    else
    {
        acceptor.non_blocking(true);
        subscriber->ConnectAsync();
    }

    const int attempts = reconnect ? 2 : 1;
    for (int attempt = 1; attempt <= attempts; ++attempt)
    {
        tcp::socket socket(service);
        if (reverse)
            socket.connect(tcp::endpoint(boost::asio::ip::address_v4::loopback(), port));
        else
            Wait([&]
            {
                boost::system::error_code error;
                acceptor.accept(socket, error);
                if (error == boost::asio::error::would_block || error == boost::asio::error::try_again) return false;
                if (error) throw boost::system::system_error(error);
                return true;
            });
        auto packet = ReadCommand(socket, 5s);
        Require(!packet.empty() && packet[4] == 0x06, "Expected operational modes first");
        if (version > 2)
        {
            Require(ReadCommand(socket, 100ms).empty(), "Startup command precedes handshake acceptance");
            Require(state.connections == attempt - 1, "Connection event precedes handshake acceptance");
            if (handshake != "timeout") Respond(socket, 0x06, handshake != "reject");
            if (handshake != "accept")
            {
                Wait([&] { return state.errors > 0; });
                Require(ReadCommand(socket, 200ms).empty(), "Startup command follows failed handshake");
                Require(state.connections == 0 && state.metadata == 0, "Failed handshake starts subscriber");
                subscriber.reset();
                break;
            }
        }
        packet = ReadCommand(socket, 5s);
        Require(!packet.empty() && packet[4] == (parse ? 0x01 : 0x02), "Incorrect initial startup command");
        if (parse)
        {
            Respond(socket, 0x01);
            packet = ReadCommand(socket, 5s);
            Require(!packet.empty() && packet[4] == 0x02, "Metadata does not trigger subscription");
        }
        Require(ReadCommand(socket, 200ms).empty(), "Duplicate metadata request or subscription");
        Require(state.connections == attempt, "Duplicate or missing connection event");
        Require(state.metadata == (parse ? attempt : 0), "Duplicate or missing metadata callback");
        // A publisher starts publishing with its current cache index as soon as the subscription starts, i.e.,
        // before confirmation of the new cache is processed. This early data uses the same signal mappings.
        SendSignalIndexCache(socket, 0);
        SendMeasurement(socket, 1, 1, 1.0F);
        packet = ReadCommand(socket, 5s);
        Require(!packet.empty() && packet[4] == 0x0A, "Signal index cache was not confirmed");
        SendMeasurement(socket, 0, 0, 2.0F);
        Wait([&] { return state.confirmed == attempt; });
        Require(state.early == attempt, "Data published before signal index cache confirmation was dropped");
        Require(state.unexpected == 0, "Measurement received with incorrect signal mapping");
        if (attempt == attempts) subscriber.reset();
        // Closing the publisher-side socket triggers real automatic reconnect on the next iteration.
    }
    std::cout << "PASS " << (reverse ? "reverse" : "outbound") << " parse=" << parse
              << " version=" << version << " handshake=" << handshake << " reconnect=" << reconnect << std::endl;
}

int main()
{
    try
    {
        for (bool reverse : {false, true})
        {
            for (bool parse : {false, true})
                for (int version : {2, 3})
                    Run(reverse, parse, version, "accept", !reverse);
            Run(reverse, true, 3, "reject", false);
            Run(reverse, true, 3, "timeout", false);
        }
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "FAIL: " << ex.what() << std::endl;
        return 1;
    }
}