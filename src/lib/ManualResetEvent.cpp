//******************************************************************************************************
//  ManualResetEvent.cpp - Gbtc
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
//  12/02/2022 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include "ManualResetEvent.h"

using namespace std;
using namespace sttp;
using namespace boost::chrono;

ManualResetEvent::ManualResetEvent(const bool initialState)
{
    m_signaled = initialState;
}

bool ManualResetEvent::IsSet() const
{
    return m_signaled;
}

void ManualResetEvent::Set()
{
    ScopeLock lock(m_mutex);

    if (!m_signaled)
    {
        m_signaled = true;
        m_handle.notify_all();
    }
}

void ManualResetEvent::Reset()
{
    ScopeLock lock(m_mutex);
    m_signaled = false;
}

bool ManualResetEvent::Wait(const int32_t timeout)
{
    UniqueLock lock(m_mutex);

    auto isSignaled = [this] { return m_signaled.load(); };

    if (timeout > -1)
        m_handle.wait_until(lock, steady_clock::now() + milliseconds(timeout), isSignaled);
    else
        m_handle.wait(lock, isSignaled);

    return m_signaled;
}
