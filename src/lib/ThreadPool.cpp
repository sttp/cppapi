//******************************************************************************************************
//  ThreadPool.cpp - Gbtc
//
//  Copyright © 2020, Grid Protection Alliance.  All Rights Reserved.
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
//  12/27/2020 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include "ThreadPool.h"

using namespace std;
using namespace sttp;

ThreadPool::ThreadPool() :
    m_disposing(false)
{
    m_removeCompletedTimersThread = Thread([this]()
    {
        while (!m_disposing)
        {
            m_completedTimers.WaitForData();

            if (m_disposing)
                break;

            // Dequeue next completed timer
            const TimerPtr timer = m_completedTimers.Dequeue();

            while (timer->IsRunning() && !m_disposing)
                ThreadSleep(10);

            if (m_disposing)
                break;
            
            // Remove timer from reference set
            ScopeLock lock(m_waitTimersLock);
            m_waitTimers.erase(timer);
        }
    });
}

ThreadPool::~ThreadPool() noexcept
{
    try
    {
        ShutDown();
    }
    catch (...)
    {
        // ReSharper disable once CppRedundantControlFlowJump
        return;
    }
}

void ThreadPool::ShutDown()
{
    if (m_disposing)
        return;
    
    m_disposing = true;

    m_completedTimers.Release();
    m_removeCompletedTimersThread.join();
    
    ScopeLock lock(m_waitTimersLock);
    
    for (const auto& currentTimer : m_waitTimers)
        currentTimer->Stop(); // This joins thread if running
}

void ThreadPool::Queue(const std::function<void()>& action)
{
    Queue(0, nullptr, [action](void*){ action(); });
}

void ThreadPool::Queue(void* state, const std::function<void(void*)>& action)
{
    Queue(0, state, action);
}

void ThreadPool::Queue(const uint32_t delay, const std::function<void()>& action)
{
    Queue(delay, nullptr, [action](void*){ action(); });
}

void ThreadPool::Queue(const uint32_t delay, void* state, const std::function<void(void*)>& action)
{
    if (m_disposing)
        return;
    
    TimerPtr waitTimer = NewSharedPtr<Timer>(delay, [&,this,action,state](Timer* timer, void*)
    {        
        if (m_disposing)
            return;

        // Execute action after specified delay (zero for immediate execution)
        if (action != nullptr)
            action(state);

        ScopeLock lock(m_waitTimersLock);
        
        if (m_disposing)
            return;

        TimerPtr targetTimer = nullptr;
        
        // Find this timer in reference set
        for (const auto& currentTimer : m_waitTimers)
        {
            if (currentTimer.get() == timer)
            {
                targetTimer = currentTimer;
                break;
            }
        }

        if (targetTimer == nullptr || m_disposing)
            return;

        // Queue timer for removal from reference set which will cause its immediate
        // destruction - cannot remove here before timer callback completes
        m_completedTimers.Enqueue(targetTimer);
    });

    ScopeLock lock(m_waitTimersLock);

    if (m_disposing)
        return;
    
    // Keep a reference to timer in a set so it will have scope beyond this function
    m_waitTimers.insert(waitTimer);

    // Start timer
    waitTimer->Start();
}