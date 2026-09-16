// Copyright (c) 2026, Grid Protection Alliance. Licensed under the MIT License.
#include "../../lib/transport/SubscriberInstance.h"
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
using namespace sttp;
using namespace sttp::transport;

// Synchronization outlives the subscriber whose destruction is under test.
struct State
{
    std::mutex mutex;
    std::condition_variable changed;
    bool ready = false, releaseMetadata = false, metadataCompleted = false;
    bool holdMetadata;
    explicit State(bool hold) : holdMetadata(hold) {}
};

class TestSubscriber final : public SubscriberInstance
{
    State& m_state;
    void StatusMessage(const std::string&) override {}
    void ErrorMessage(const std::string& message) override { std::cerr << message << '\n'; }
    void ReceivedMetadata(const std::vector<uint8_t>& payload) override
    {
        State& state = m_state;
        if (state.holdMetadata)
        {
            std::unique_lock<std::mutex> lock(state.mutex);
            state.ready = true;
            state.changed.notify_all();
            state.changed.wait(lock, [&state] { return state.releaseMetadata; });
        }
        // Must finish while the base instance's metadata maps and mutex still exist.
        SubscriberInstance::ReceivedMetadata(payload);
        std::lock_guard<std::mutex> lock(state.mutex);
        state.metadataCompleted = true;
    }
    void ReceivedNewMeasurements(const std::vector<MeasurementPtr>& measurements) override
    {
        if (measurements.empty() || m_state.holdMetadata)
            return;
        std::lock_guard<std::mutex> lock(m_state.mutex);
        m_state.ready = true;
        m_state.changed.notify_all();
    }
public:
    explicit TestSubscriber(State& state) : m_state(state) {}
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: SubscriberShutdownTest HOST [PORT=7165] [ITERATIONS=10] [data|metadata]\n";
        return 2;
    }
    try
    {
        const int port = argc > 2 ? std::stoi(argv[2]) : 7165;
        const int iterations = argc > 3 ? std::stoi(argv[3]) : 10;
        const std::string mode = argc > 4 ? argv[4] : "data";
        if (port < 1 || port > 65535 || iterations < 1 || (mode != "data" && mode != "metadata"))
            throw std::invalid_argument("Invalid port, iteration count, or mode");
        for (int iteration = 1; iteration <= iterations; ++iteration)
        {
            State state(mode == "metadata");
            auto subscriber = std::make_unique<TestSubscriber>(state);
            subscriber->Initialize(argv[1], static_cast<uint16_t>(port));
            subscriber->SetAutoReconnect(false);
            subscriber->SetFilterExpression("FILTER ActiveMeasurements WHERE SignalType <> 'STAT'");
            subscriber->ConnectAsync();
            bool ready;
            {
                std::unique_lock<std::mutex> lock(state.mutex);
                ready = state.changed.wait_for(lock, std::chrono::seconds(30), [&state] { return state.ready; });
            }
            // Hold metadata across destruction, independent of parsing speed.
            std::thread release([&state]
            {
                if (state.holdMetadata)
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::lock_guard<std::mutex> lock(state.mutex);
                state.releaseMetadata = true;
                state.changed.notify_all();
            });
            subscriber->Disconnect();
            subscriber.reset();
            release.join();
            if (!ready || (state.holdMetadata && !state.metadataCompleted))
                throw std::runtime_error("Timed out or metadata callback failed to complete");
            std::cout << "PASS," << mode << ',' << iteration << std::endl;
        }
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "FAIL: " << ex.what() << std::endl;
        return 1;
    }
}
