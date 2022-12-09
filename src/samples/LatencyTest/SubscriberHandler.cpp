//******************************************************************************************************
//  SubscriberHandler.cpp - Gbtc
//
//  Copyright © 2018, Grid Protection Alliance.  All Rights Reserved.
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
//  03/27/2018 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include "SubscriberHandler.h"
#include "../../lib/Convert.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;

Mutex SubscriberHandler::s_coutLock{};

SubscriberHandler::SubscriberHandler(string name) :
    m_name(std::move(name)),
    m_lastMessage(DateTime::MinValue),
    m_total(0.0),
    m_count(0ULL),
    m_unreasonable(0ULL)
{
}

void SubscriberHandler::ReceivedNewMeasurements(const vector<MeasurementPtr>& measurements)
{
    static constexpr double_t unreasonableLatency = 60 * 5;

    // Only display messages every few seconds
    if (TimeSince(m_lastMessage) > 5.0F)
    {
        if (m_lastMessage == DateTime::MinValue)
        {
            StatusMessage("Receiving measurements...");
        }
        else
        {
            stringstream receivedUpdate;
            receivedUpdate << GetTotalMeasurementsReceived() << " measurements received so far..." << endl << endl;
            receivedUpdate << "Average latency: " << setprecision(3) << m_total / static_cast<double_t>(m_count) << " seconds" << endl;
            receivedUpdate << "   Total values: " << m_count << endl;
            receivedUpdate << "   Unreasonable: " << m_unreasonable << endl;
            StatusMessage(receivedUpdate.str());
        }

        // Reset latency stats
        m_total = 0.0;
        m_count = m_unreasonable = 0ULL;
        m_lastMessage = UtcNow();
    }

    // Process measurements
    for (auto &measurement : measurements)
    {
        // Ignore timestamps marked with bad time quality
        if (static_cast<int>(measurement->Flags & MeasurementStateFlags::BadTime) == 0)
        {
            TimeSpan duration = UtcNow() - measurement->GetDateTime();
            const double_t difference = static_cast<int32_t>(duration.total_microseconds()) * 1e-6;

            // Throw out unreasonable timestamps
            if (abs(difference) > unreasonableLatency)
            {
                m_unreasonable++;
                continue;
            }

            m_total += difference;
            m_count++;
        }
    }
}

void SubscriberHandler::StatusMessage(const string& message)
{
    // Calls can come from multiple threads, so we impose a simple lock before write to console
    s_coutLock.lock();
    SubscriberInstance::StatusMessage(message);
    s_coutLock.unlock();
}

void SubscriberHandler::ErrorMessage(const string& message)
{
    // Calls can come from multiple threads, so we impose a simple lock before write to console
    s_coutLock.lock();
    SubscriberInstance::ErrorMessage(message);
    s_coutLock.unlock();
}

void SubscriberHandler::SubscriptionUpdated(const SignalIndexCachePtr& signalIndexCache)
{
    StatusMessage("Publisher provided " + ToString(signalIndexCache->Count()) + " measurements in response to subscription.");
}

void SubscriberHandler::ConnectionEstablished()
{
    StatusMessage("Connection established.");
}

void SubscriberHandler::ConnectionTerminated()
{
    StatusMessage("Connection terminated.");
}
