#pragma once

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <mutex>

namespace sttp { namespace diagnostics {

// Opt-in, synchronous startup diagnostics. Bypasses the callback queue so a busy
// metadata callback cannot delay the timestamps. Set STTP_STARTUP_TRACE=1 before launch.
inline bool StartupTraceEnabled()
{
    static const bool enabled = [] {
#ifdef _MSC_VER
        char* value = nullptr;
        size_t length = 0;
        const bool found = _dupenv_s(&value, &length, "STTP_STARTUP_TRACE") == 0 &&
            value != nullptr && value[0] == '1' && value[1] == '\0';
        std::free(value);
        return found;
#else
        const char* value = std::getenv("STTP_STARTUP_TRACE");
        return value != nullptr && value[0] == '1' && value[1] == '\0';
#endif
    }();
    return enabled;
}

using StartupClock = std::chrono::steady_clock;

inline void StartupEvent(const void* subscriber, const char* stage, double elapsedMs = 0.0, size_t items = 0)
{
    if (!StartupTraceEnabled())
        return;

    static const auto origin = StartupClock::now();
    static std::mutex outputMutex;
    const auto now = StartupClock::now();
    const double sinceOrigin = std::chrono::duration<double, std::milli>(now - origin).count();
    const std::lock_guard<std::mutex> lock(outputMutex);
    std::fprintf(stderr, "[STTP startup +%.3f ms subscriber=%p] %s: %.3f ms; items=%zu\n",
        sinceOrigin, const_cast<void*>(subscriber), stage, elapsedMs, items);
    std::fflush(stderr);
}

class StartupPhase
{
public:
    StartupPhase(const void* subscriber, const char* stage) :
        m_subscriber(subscriber), m_stage(stage), m_enabled(StartupTraceEnabled())
    {
        if (m_enabled)
        {
            m_start = m_previous = StartupClock::now();
            StartupEvent(m_subscriber, m_stage);
        }
    }

    // Each mark reports time since the previous mark; destruction reports total scope time.
    void Mark(const char* stage, size_t items = 0)
    {
        if (!m_enabled)
            return;
        const auto now = StartupClock::now();
        StartupEvent(m_subscriber, stage,
            std::chrono::duration<double, std::milli>(now - m_previous).count(), items);
        m_previous = now;
    }

    ~StartupPhase()
    {
        if (m_enabled)
            StartupEvent(m_subscriber, m_stage,
                std::chrono::duration<double, std::milli>(StartupClock::now() - m_start).count());
    }

private:
    const void* m_subscriber;
    const char* m_stage;
    bool m_enabled;
    StartupClock::time_point m_start{}, m_previous{};
};

} }
