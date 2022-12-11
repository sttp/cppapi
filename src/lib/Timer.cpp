//******************************************************************************************************
//  Timer.cpp - Gbtc
//
//  Copyright © 2022, Grid Protection Alliance.  All Rights Reserved.
//
//  Licensed to the Grid Protection Alliance (GPA) under one or more contributor license agreements. See
//  the NOTICE file distributed with this work for additional information regarding copyright ownership.
//  The GPA licenses this file to you under the MIT License (MIT), the "License"; you may not use this
//  file except in compliance with the License. You may obtain a copy of the License at:
//
//      http://opensource.org/licenses/MIT
//
//  Unless agreed to in writing, the subject software distributed under the License is distributed on an
//  "AS-IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. Refer to the
//  License for the specific language governing permissions and limitations.
//
//  Code Modification History:
//  ----------------------------------------------------------------------------------------------------
//  12/10/2022 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include "Timer.h"

using namespace std;
using namespace sttp;

const TimerPtr Timer::NullPtr = nullptr;

void Timer::TimerThread()
{
    m_running = true;

    do
    {
        try
        {
            const int32_t interval = m_interval;

            if (interval > 0)
            {
                static constexpr int32_t MaxSleepDuration = 500;
                const int32_t waits = interval / MaxSleepDuration;
                const int32_t remainder = interval % MaxSleepDuration;

                for (int32_t i = 0; i < waits && m_running; i++)
                    ThreadSleep(MaxSleepDuration);

                if (remainder > 0 && m_running)
                    ThreadSleep(remainder);
            }
        }
        catch (boost::thread_interrupted&)
        {
            m_running = false;
            return;
        }

        if (m_running && m_callback != nullptr)
            m_callback(m_this, m_userData);
    }
    while (m_autoReset && m_running);

    m_running = false;
}

Timer::Timer(): Timer(1000, nullptr, false)
{
}

Timer::Timer(const int32_t interval, TimerElapsedCallback callback, const bool autoReset):
    m_this(NullPtr),
    m_timerThread(ThreadNullPtr),
    m_interval(interval),
    m_callback(std::move(callback)),
    m_userData(nullptr),
    m_autoReset(autoReset),
    m_running(false)
{
}

Timer::~Timer() noexcept
{
    try
    {
        Stop();
    }
    catch (...)
    {
        // ReSharper disable once CppRedundantControlFlowJump
        return;
    }
}

bool Timer::IsRunning() const
{
    return m_running;
}

int32_t Timer::GetInterval() const
{
    return m_interval;
}

void Timer::SetInterval(const int32_t value)
{
    if (value == m_interval)
        return;
            
    const bool restart = m_running;
    Stop();

    m_interval = value;

    if (restart)
        Start();
}

TimerElapsedCallback Timer::GetCallback() const
{
    return m_callback;
}

void Timer::SetCallback(TimerElapsedCallback value)
{
    m_callback = std::move(value);
}

const void* Timer::GetUserData() const
{
    return m_userData;
}

void Timer::SetUserData(void* value)
{
    m_userData = value;
}

bool Timer::GetAutoReset() const
{
    return m_autoReset;
}

void Timer::SetAutoReset(const bool value)
{
    m_autoReset = value;
}

void Timer::Start()
{
    if (m_callback == nullptr)
        throw std::invalid_argument("Cannot start timer, no callback function has been defined.");

    if (m_this == nullptr && !weak_from_this().expired())
        m_this = shared_from_this();

    if (m_running)
        Stop();

    m_timerThread = NewSharedPtr<sttp::Thread>([this] { TimerThread(); });
}

void Timer::Stop()
{
    if (!m_running)
        return;

    m_running = false;

    if (m_timerThread != nullptr)
    {
        const ThreadPtr timerThread = m_timerThread;

        if (timerThread != nullptr)
        {
            timerThread->interrupt();

            if (boost::this_thread::get_id() != timerThread->get_id())
                timerThread->join();
        }
    }

    m_timerThread.reset();
}

void Timer::Wait() const
{
    try
    {
        if (m_running && m_timerThread != nullptr)
        {
            const ThreadPtr timerThread = m_timerThread;

            if (timerThread != nullptr)
                timerThread->join();
        }                
    }
    catch (...)
    {
        // ReSharper disable once CppRedundantControlFlowJump
        return;
    }
}

void Timer::EmptyCallback(const TimerPtr&, void*)
{            
}

TimerPtr Timer::WaitTimer(const int32_t interval, const bool autoStart)
{
    TimerPtr waitTimer = NewSharedPtr<Timer>(interval, EmptyCallback);

    if (autoStart)
        waitTimer->Start();

    return waitTimer;
}
