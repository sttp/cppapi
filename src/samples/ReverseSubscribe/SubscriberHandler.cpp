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

SubscriberHandler::SubscriberHandler()
{
}

SubscriptionInfo SubscriberHandler::CreateSubscriptionInfo()
{
    SubscriptionInfo info = SubscriberInstance::CreateSubscriptionInfo();

    // TODO: Modify subscription info properties as desired...

    // See SubscriptionInfo class in DataSubscriber.h for all properties
    //info.Throttled = false;
    //info.IncludeTime = true;
    //info.UseMillisecondResolution = true;

    return info;
}

void SubscriberHandler::StatusMessage(const string& message)
{
    // TODO: Make sure these messages get logged to an appropriate location

    // Calls can come from multiple threads, so we impose a simple lock before write to console
    ScopeLock lock(s_coutLock);

    // For now, we just the base class to display to console:
    SubscriberInstance::StatusMessage(message);
}

void SubscriberHandler::ErrorMessage(const string& message)
{
    // TODO: Make sure these messages get logged to an appropriate location

    // Calls can come from multiple threads, so we impose a simple lock before write to console
    ScopeLock lock(s_coutLock);

    // For now, we just the base class to display to console:
    SubscriberInstance::ErrorMessage(message);
}

void SubscriberHandler::DataStartTime(time_t unixSOC, uint16_t milliseconds)
{
    // TODO: This reports timestamp, time_t format, of very first received measurement (if useful)
}

void SubscriberHandler::DataStartTime(datetime_t startTime)
{
    // TODO: This reports timestamp, ptime format, of very first received measurement (if useful)
}

void SubscriberHandler::ReceivedMetadata(const vector<uint8_t>& payload)
{
    StatusMessage("Received " + ToString(payload.size()) + " bytes of metadata, parsing...");
    SubscriberInstance::ReceivedMetadata(payload);
}

void SubscriberHandler::ParsedMetadata()
{
    StatusMessage("Metadata successfully parsed.");
}

// ReSharper disable CppDeclaratorNeverUsed
void SubscriberHandler::ReceivedNewMeasurements(const vector<MeasurementPtr>& measurements)
{   
    static constexpr float32_t interval = 2.0; // Show status every 2 seconds
    static constexpr uint64_t maxToShow = 20ULL;
    const uint64_t measurementCount = measurements.size();
    static datetime_t lastMessage = DateTime::MinValue;

    if (TimeSince(lastMessage) < interval)
        return;

    lastMessage = UtcNow();
    uint64_t shown = 0ULL;
    stringstream message;

    message << GetTotalMeasurementsReceived() << " measurements received so far..." << endl;

    if (measurementCount > 0) // Just show time of first measurement
        message << ToString(measurements[0]->GetDateTime()) << endl;

    message << "\tRuntime-ID\tMeta-data ID\tValue\t\tType\tSignalID" << endl;

    // Start processing measurements
    for (auto &measurement : measurements)
    {
        if (shown++ > maxToShow)
            break;

        // Get adjusted value
        const float64_t value = measurement->AdjustedValue();

        // Get timestamp
        datetime_t timestamp = measurement->GetDateTime();

        // Handle per measurement quality flags
        MeasurementStateFlags qualityFlags = measurement->Flags;

        ConfigurationFramePtr configurationFrame;
        MeasurementMetadataPtr measurementMetadata;

        // Find associated configuration for measurement
        if (TryFindTargetConfigurationFrame(measurement->SignalID, configurationFrame))
        {
            // Lookup measurement metadata - it's faster to find metadata from within configuration frame
            if (TryGetMeasurementMetadataFromConfigurationFrame(measurement->SignalID, configurationFrame, measurementMetadata))
            {
                const SignalReference& reference = measurementMetadata->Reference;

                // reference.Acronym  << target device acronym 
                // reference.Kind     << kind of signal (see SignalKind in "Types.h"), like Frequency, Angle, etc
                // reference.Index    << for Phasors, Analogs and Digitals - this is the ordered "index"

                message << '\t' << measurement->ID << "\t\t" << measurementMetadata->ID << "\t\t" << measurement->Value << fixed << setprecision(3) << "\t\t" << SignalKindAcronym[static_cast<int32_t>(reference.Kind)] << '\t' << ToString(measurement->SignalID) << endl;
            }
        }
    }

    StatusMessage(message.str());
}

void SubscriberHandler::SubscriptionUpdated(const SignalIndexCachePtr& signalIndexCache)
{
    StatusMessage("Publisher provided " + ToString(signalIndexCache->Count()) + " measurements in response to subscription.");
}

void SubscriberHandler::ConfigurationChanged()
{
    StatusMessage("Configuration change detected. Metadata refresh requested.");
}

void SubscriberHandler::HistoricalReadComplete()
{
    StatusMessage("Historical data read complete.");
}
