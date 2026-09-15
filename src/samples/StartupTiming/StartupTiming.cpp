#include "../../lib/transport/SubscriberInstance.h"
#include <atomic>
#include <chrono>
#include <csignal>
#include <conio.h>
#include <iomanip>
#include <iostream>
#include <thread>

using Clock = std::chrono::steady_clock;
using namespace sttp;
using namespace sttp::transport;

static volatile std::sig_atomic_t stopped = 0;
static long long SteadyNanoseconds() { return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now().time_since_epoch()).count(); }

class TimingSubscriber final : public SubscriberInstance
{
public:
    std::atomic<long long> connected{0}, first{0};
    std::atomic<unsigned long long> count{0};
protected:
    void StatusMessage(const std::string&) override {}
    void ErrorMessage(const std::string& message) override { std::cerr << "STTP: " << message << std::endl; }
    void ConnectionEstablished() override
    {
        // Capture the earliest callback even if the library reports establishment twice.
        long long expected = 0;
        connected.compare_exchange_strong(expected, SteadyNanoseconds());
    }
    void ConnectionTerminated() override { std::cerr << "Connection terminated." << std::endl; }
    void ReceivedNewMeasurements(const std::vector<MeasurementPtr>& measurements) override
    {
        if (measurements.empty()) return;
        const auto now = SteadyNanoseconds();
        count.fetch_add(measurements.size());
        long long expected = 0;
        first.compare_exchange_strong(expected, now);
    }
};

int main(int argc, char** argv)
{
    try
    {
        if (argc < 2 || std::string(argv[1]) == "--help")
        {
            std::cout << "Usage: StartupTiming IP [port=7165] [--no-metadata] [--seconds N]\n";
            return argc < 2 ? 1 : 0;
        }
        unsigned long port = 7165;
        bool metadata = true;
        int seconds = 0;
        int index = 2;
        if (index < argc && argv[index][0] != '-')
        {
            std::string value(argv[index++]);
            size_t consumed = 0;
            port = std::stoul(value, &consumed);
            if (consumed != value.size() || port == 0 || port > 65535) throw std::runtime_error("Invalid port");
        }
        while (index < argc)
        {
            const std::string option(argv[index++]);
            if (option == "--no-metadata") metadata = false;
            else if (option == "--seconds" && index < argc)
            {
                const std::string value(argv[index++]);
                size_t consumed = 0;
                seconds = std::stoi(value, &consumed);
                if (consumed != value.size() || seconds <= 0) throw std::runtime_error("Invalid duration");
            }
            else throw std::runtime_error("Unknown/incomplete option: " + option);
        }
        std::signal(SIGINT, [](int) { stopped = 1; });
        TimingSubscriber subscriber;
        subscriber.Initialize(argv[1], static_cast<uint16_t>(port));
        subscriber.SetFilterExpression("FILTER ActiveMeasurements WHERE SignalType <> 'STAT'");
        subscriber.SetAutoParseMetadata(metadata);
        subscriber.SetAutoReconnect(false);
        std::cout << "Endpoint: " << argv[1] << ':' << port << "; metadata: " << (metadata ? "enabled" : "disabled") << '\n'
                  << "Press any key to exit.\n";
        const auto start = SteadyNanoseconds();
        subscriber.ConnectAsync();
        bool reported = false;
        unsigned long long previousCount = 0;
        long long previousTime = start;
        unsigned spinner = 0;
        while (!stopped)
        {
            if (_kbhit()) { _getch(); break; }
            const auto now = SteadyNanoseconds();
            if (seconds > 0 && (now - start) / 1e9 >= seconds) break;
            const auto first = subscriber.first.load();
            if (!reported && first != 0)
            {
                std::cout << std::fixed << std::setprecision(3)
                          << "Connection to first measurement: " << (first - subscriber.connected.load()) / 1e6 << " ms\n"
                          << "Total from connection attempt: " << (first - start) / 1e6 << " ms\n\n";
                previousTime = first;
                reported = true;
            }
            const double elapsed = (now - previousTime) / 1e9;
            if (reported && elapsed >= 1.0)
            {
                const auto count = subscriber.count.load();
                if (count != previousCount) spinner = (spinner + 1) % 4;
                std::cout << '\r' << "\\|/-"[spinner] << ' ' << std::setw(12) << std::setprecision(0)
                          << (count - previousCount) / elapsed << " measurements/sec" << std::flush;
                previousCount = count;
                previousTime = now;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        subscriber.Disconnect();
        std::cout << "\nTotal measurements: " << subscriber.count.load() << std::endl;
        return subscriber.count.load() == 0 ? 2 : 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
}


