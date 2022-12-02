//******************************************************************************************************
//  ManualResetEvent.h - Gbtc
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
//  12/01/2022 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "CommonTypes.h"

namespace sttp
{
    /**
     * Defines a synchronization primitive that notifies one or more waiting threads
     * that an event has occurred where signal must be reset manually.
     */
    class ManualResetEvent final
    {
    private:
        Mutex m_mutex;
        WaitHandle m_handle;
        std::atomic_bool m_signaled;

    public:
        /**
         * Initializes a new instance of the ManualResetEvent class.
         * @param initialState `true` to set the initial state as signaled; `false` for non-signaled.
         */
        ManualResetEvent(const bool initialState = false)
        {
            m_signaled = initialState;
        }

        /**
         * Gets whether the event is signaled.
         */
        bool IsSet() const
        {
            return m_signaled;
        }

        /**
         * Sets the state of the event to signaled, which allows one or more threads waiting on the event to proceed.
         */
        void Set()
        {
            ScopeLock lock(m_mutex);

            if (!m_signaled)
            {
                m_signaled = true;
                m_handle.notify_all();
            }
        }

        /**
         * Sets the state of the event to non-signaled, which causes threads to block.
         */
        void Reset()
        {
            ScopeLock lock(m_mutex);
            m_signaled = false;
        }

        /**
         * Blocks the current thread until the ManualResetEvent is signaled.
         * @param timeout the number of milliseconds to wait, or -1 to wait indefinitely.
         * @return `true` if the ManualResetEvent was signaled; otherwise, `false`.
         */
        bool Wait(const int32_t timeout = -1)
        {
            UniqueLock lock(m_mutex);

            m_handle.wait(lock);

            if (timeout > -1)
            {
                if (!m_signaled)
                    m_handle.wait_for(lock, boost::chrono::milliseconds(timeout));
            }
            else
            {
                while (!m_signaled)
                    m_handle.wait(lock);
            }

            return m_signaled;
        }
    };
}
