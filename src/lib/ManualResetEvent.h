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
        explicit ManualResetEvent(bool initialState = false);

        /**
         * Gets whether the event is signaled.
         */
        bool IsSet() const;

        /**
         * Sets the state of the event to signaled, which allows one or more threads waiting on the event to proceed.
         */
        void Set();

        /**
         * Sets the state of the event to non-signaled, which causes threads to block.
         */
        void Reset();

        /**
         * Blocks the current thread until the ManualResetEvent is signaled.
         * @param timeout the number of milliseconds to wait, or -1 to wait indefinitely.
         * @return `true` if the ManualResetEvent was signaled; otherwise, `false`.
         */
        bool Wait(int32_t timeout = -1);
    };
}
