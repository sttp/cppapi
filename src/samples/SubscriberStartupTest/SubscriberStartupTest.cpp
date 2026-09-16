// Copyright (c) 2026, Grid Protection Alliance. Licensed under the MIT License.
#include "../../lib/transport/SubscriberInstance.h"
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
    std::atomic<int> connections{0}, metadata{0}, errors{0};
};

class TestSubscriber final : public SubscriberInstance
{
    State& m_state;
    void ConnectionEstablished() override { ++m_state.connections; }
    void ReceivedMetadata(const std::vector<uint8_t>&) override { ++m_state.metadata; }
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