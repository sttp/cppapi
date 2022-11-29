//******************************************************************************************************
//  Timer.h - Gbtc
//
//  Copyright © 2019, Grid Protection Alliance.  All Rights Reserved.
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
//  01/29/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "CommonTypes.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

namespace sttp
{
    class Timer;
    typedef std::function<void(Timer* timer, void* userData)> TimerElapsedCallback;
    typedef sttp::SharedPtr<Timer> TimerPtr;

    class Timer // NOLINT
    {
    private:
        ThreadPtr m_timerThread;
        int32_t m_interval;
        TimerElapsedCallback m_callback;
        void* m_userData;
        bool m_autoReset;
        std::atomic_bool m_running;

        void TimerThread()
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
                    m_callback(this, m_userData);
            }
            while (m_autoReset && m_running);

            m_running = false;
        }

    public:
        Timer() : Timer(1000, nullptr, false)
        {
        }

        Timer(const int32_t interval, TimerElapsedCallback callback, const bool autoReset = false) :
            m_timerThread(nullptr),
            m_interval(interval),
            m_callback(std::move(callback)),
            m_userData(nullptr),
            m_autoReset(autoReset),
            m_running(false)
        {
        }

        ~Timer() noexcept
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

        bool IsRunning() const
        {
            return m_running;
        }

        int32_t GetInterval() const
        {
            return m_interval;
        }

        void SetInterval(const int32_t value)
        {
            if (value == m_interval)
                return;
            
            const bool restart = m_running;
            Stop();

            m_interval = value;

            if (restart)
                Start();
        }

        TimerElapsedCallback GetCallback() const
        {
            return m_callback;
        }

        void SetCallback(TimerElapsedCallback value)
        {
            m_callback = std::move(value);
        }

        const void* GetUserData() const
        {
            return m_userData;
        }

        void SetUserData(void* value)
        {
            m_userData = value;
        }

        bool GetAutoReset() const
        {
            return m_autoReset;
        }

        void SetAutoReset(const bool value)
        {
            m_autoReset = value;
        }

        void Start()
        {
            if (m_callback == nullptr)
                throw std::invalid_argument("Cannot start timer, no callback function has been defined.");

            if (m_running)
                Stop();

            m_timerThread = NewSharedPtr<sttp::Thread>([this] { TimerThread(); });
        }

        void Stop()
        {
            if (!m_running)
                return;

            m_running = false;

            if (m_timerThread != nullptr)
            {
                ThreadPtr timerThread = m_timerThread;

                if (timerThread != nullptr)
                {
                    timerThread->interrupt();

                    if (boost::this_thread::get_id() != timerThread->get_id())
                        timerThread->join();
                }
            }

            m_timerThread.reset();
        }

        void Wait() const
        {
            try
            {
                if (m_running && m_timerThread != nullptr)
                {
                    ThreadPtr timerThread = m_timerThread;

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

        static void EmptyCallback(Timer*, void*)
        {            
        }

        static TimerPtr WaitTimer(const int32_t interval, const bool autoStart = true)
        {
            TimerPtr waitTimer = NewSharedPtr<Timer>(interval, EmptyCallback);

            if (autoStart)
                waitTimer->Start();

            return waitTimer;
        }
    };

    typedef sttp::SharedPtr<Timer> TimerPtr;
}

// Setup standard hash code for TimerPtr
namespace std  // NOLINT
{
    template<>
    struct hash<sttp::TimerPtr>
    {
        size_t operator () (const sttp::TimerPtr& timer) const noexcept
        {
            return boost::hash<sttp::TimerPtr>()(timer);
        }
    };
}