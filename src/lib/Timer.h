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

namespace sttp
{
    class Timer;
    typedef SharedPtr<Timer> TimerPtr;

    // Defines call back function signature that is executed when timer delay expires.
    // The `timer` parameter will be `nullptr` if timer is not instantiated as shared_ptr.
    typedef std::function<void(const TimerPtr& timer, void* userData)> TimerElapsedCallback;

    class Timer final : public EnableSharedThisPtr<Timer> // NOLINT
    {
    private:
        TimerPtr m_this;
        ThreadPtr m_timerThread;
        int32_t m_interval;
        TimerElapsedCallback m_callback;
        void* m_userData;
        bool m_autoReset;
        std::atomic_bool m_running;

        void TimerThread();

    public:
        Timer();

        explicit Timer(int32_t interval, TimerElapsedCallback callback, bool autoReset = false);

        ~Timer() noexcept;

        bool IsRunning() const;

        int32_t GetInterval() const;

        void SetInterval(int32_t value);

        TimerElapsedCallback GetCallback() const;

        void SetCallback(TimerElapsedCallback value);

        const void* GetUserData() const;

        void SetUserData(void* value);

        bool GetAutoReset() const;

        void SetAutoReset(bool value);

        void Start();

        void Stop();

        void Wait() const;

        static void EmptyCallback(const TimerPtr&, void*);

        static TimerPtr WaitTimer(int32_t interval, bool autoStart = true);

        const static TimerPtr NullPtr;
    };
}

// Setup standard hash code for TimerPtr
template<>
struct std::hash<sttp::TimerPtr>
{
    size_t operator () (const sttp::TimerPtr& timer) const noexcept
    {
        return boost::hash<sttp::TimerPtr>()(timer);
    }
};
