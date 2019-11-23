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
    m_processCount(0ULL),
    m_export(nullptr),
    m_ready(false)
{
}

void SubscriberHandler::ReceivedNewMeasurements(const vector<MeasurementPtr>& measurements)
{   
    static const uint64_t interval = 5000;
    static const int32_t precision = 10;
    static const uint64_t exportCount = 10000;
    const uint64_t measurementCount = measurements.size();
    const bool showMessage = (m_processCount + measurementCount >= (m_processCount / interval + 1) * interval);

    if (m_processCount >= exportCount)
        return;

    // Only display messages every few seconds
    if (showMessage)
    {
        stringstream receivedUpdate;
        receivedUpdate << GetTotalMeasurementsReceived() << " measurements received so far..." << endl << endl;
    }

    // Process measurements
    for (auto &measurement : measurements)
    {
        datetime_t timestamp = measurement->GetDateTime();

        if (!m_ready)
        {
            // Start export at top of second
            if (DatePart(timestamp, TimeInterval::Second) == 0)
                m_ready = true;
            else
                continue;

            fprintf(m_export, ("Export for measurement " + ToString(measurement->SignalID) + "\r\n\r\n").c_str());
            fprintf(m_export, "Timestamp,Value,Flags\r\n");
        }

        fprintf(m_export, "%s,%d.*%f,%i\r\n", ToString(measurement->Timestamp).c_str(), precision, measurement->Value, measurement->Flags);

        m_processCount++;

        if (m_processCount >= exportCount)
        {
            Disconnect();
            break;
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

    stringstream fileNameBuf;
    fileNameBuf << m_name << ".csv";

    const char* fileName = fileNameBuf.str().c_str();
        
    if (fopen_s(&m_export, fileName, "w") != 0)
    {
        ErrorMessage("InteropTest canceled: failed to open export file \"" + ToString(fileName) + "\"");
        Disconnect();
    }
}

void SubscriberHandler::ConnectionTerminated()
{
    StatusMessage("Connection terminated.");
    fclose(m_export);
}