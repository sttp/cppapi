//******************************************************************************************************
//  PublisherHandler.cpp - Gbtc
//
//  Copyright � 2019, Grid Protection Alliance.  All Rights Reserved.
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
//  03/27/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

// ReSharper disable CppAssignedValueIsNeverUsed
// ReSharper disable CppClangTidyConcurrencyMtUnsafe
#include "PublisherHandler.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;

Mutex PublisherHandler::s_coutLock {};

PublisherHandler::PublisherHandler() : 
    m_lastMessage(DateTime::MinValue),
    m_publishTimer(nullptr),
    m_metadataVersion(0)
{
}

void PublisherHandler::StatusMessage(const string& message)
{
    // TODO: Make sure these messages get logged to an appropriate location

    // Calls can come from multiple threads, so we impose a simple lock before write to console
    ScopeLock lock(s_coutLock);

    // For now, we just the base class to display to console:
    PublisherInstance::StatusMessage(message);
}

void PublisherHandler::ErrorMessage(const string& message)
{
    // TODO: Make sure these messages get logged to an appropriate location

    // Calls can come from multiple threads, so we impose a simple lock before write to console
    ScopeLock lock(s_coutLock);

    // For now, we just the base class to display to console:
    PublisherInstance::ErrorMessage(message);
}

void PublisherHandler::ClientConnected(const SubscriberConnectionPtr& connection)
{
    if (IsReverseConnection())
        StatusMessage("Reverse connection to client \"" + connection->GetConnectionID() + "\" with subscriber ID " + ToString(connection->GetSubscriberID()) + " connected...\n\n");
    else
        StatusMessage("Client \"" + connection->GetConnectionID() + "\" with subscriber ID " + ToString(connection->GetSubscriberID()) + " connected...\n\n");
}

void PublisherHandler::ClientDisconnected(const SubscriberConnectionPtr& connection)
{
    if (IsReverseConnection())
        StatusMessage("Reverse connection to client \"" + connection->GetConnectionID() + "\" with subscriber ID " + ToString(connection->GetSubscriberID()) + " disconnected...\n\n");
    else
        StatusMessage("Client \"" + connection->GetConnectionID() + "\" with subscriber ID " + ToString(connection->GetSubscriberID()) + " disconnected...\n\n");
}

void PublisherHandler::DefineMetadata()
{
    // This sample just generates random Guid measurement and device identifiers - for a production system,
    // these Guid values would need to persist between runs defining a permanent association between the
    // defined metadata and the identifier...

    DeviceMetadataPtr device1Metadata = NewSharedPtr<DeviceMetadata>();
    const datetime_t timestamp = UtcNow();

    // Add a device
    device1Metadata->Name = "Test PMU";
    device1Metadata->Acronym = ToUpper(Replace(device1Metadata->Name, " ", "", false));
    device1Metadata->UniqueID = NewGuid();
    device1Metadata->Longitude = 85.3094;
    device1Metadata->Latitude = 35.0458;
    device1Metadata->FramesPerSecond = 30;
    device1Metadata->ProtocolName = "STTP";
    device1Metadata->UpdatedOn = timestamp;

    m_deviceMetadata.emplace_back(device1Metadata);

    const string& pointTagPrefix = device1Metadata->Acronym + ".";
    const string& measurementSource = "PPA:";
    int runtimeIndex = 1;

    // Add a frequency measurement
    MeasurementMetadataPtr measurement1Metadata = NewSharedPtr<MeasurementMetadata>();
    measurement1Metadata->ID = measurementSource + ToString(runtimeIndex++);
    measurement1Metadata->PointTag = pointTagPrefix + "FREQ";
    measurement1Metadata->SignalID = NewGuid();
    measurement1Metadata->DeviceAcronym = device1Metadata->Acronym;
    measurement1Metadata->Reference.Acronym = device1Metadata->Acronym;
    measurement1Metadata->Reference.Kind = SignalKind::Frequency;
    measurement1Metadata->Reference.Index = 0;
    measurement1Metadata->PhasorSourceIndex = 0;
    measurement1Metadata->UpdatedOn = timestamp;

    // Add a dF/dt measurement
    MeasurementMetadataPtr measurement2Metadata = NewSharedPtr<MeasurementMetadata>();
    measurement2Metadata->ID = measurementSource + ToString(runtimeIndex++);
    measurement2Metadata->PointTag = pointTagPrefix + "DFDT";
    measurement2Metadata->SignalID = NewGuid();
    measurement2Metadata->DeviceAcronym = device1Metadata->Acronym;
    measurement2Metadata->Reference.Acronym = device1Metadata->Acronym;
    measurement2Metadata->Reference.Kind = SignalKind::DfDt;
    measurement2Metadata->Reference.Index = 0;
    measurement2Metadata->PhasorSourceIndex = 0;
    measurement2Metadata->UpdatedOn = timestamp;

    // Add a phase angle measurement
    MeasurementMetadataPtr measurement3Metadata = NewSharedPtr<MeasurementMetadata>();
    measurement3Metadata->ID = measurementSource + ToString(runtimeIndex++);
    measurement3Metadata->PointTag = pointTagPrefix + "VPHA";
    measurement3Metadata->SignalID = NewGuid();
    measurement3Metadata->DeviceAcronym = device1Metadata->Acronym;
    measurement3Metadata->Reference.Acronym = device1Metadata->Acronym;
    measurement3Metadata->Reference.Kind = SignalKind::Angle;
    measurement3Metadata->Reference.Index = 1;   // First phase angle
    measurement3Metadata->PhasorSourceIndex = 1; // Match to Phasor.SourceIndex = 1
    measurement3Metadata->UpdatedOn = timestamp;

    // Add a phase magnitude measurement
    MeasurementMetadataPtr measurement4Metadata = NewSharedPtr<MeasurementMetadata>();
    measurement4Metadata->ID = measurementSource + ToString(runtimeIndex++);
    measurement4Metadata->PointTag = pointTagPrefix + "VPHM";
    measurement4Metadata->SignalID = NewGuid();
    measurement4Metadata->DeviceAcronym = device1Metadata->Acronym;
    measurement4Metadata->Reference.Acronym = device1Metadata->Acronym;
    measurement4Metadata->Reference.Kind = SignalKind::Magnitude;
    measurement4Metadata->Reference.Index = 1;   // First phase magnitude
    measurement4Metadata->PhasorSourceIndex = 1; // Match to Phasor.SourceIndex = 1
    measurement4Metadata->UpdatedOn = timestamp;

    m_measurementMetadata.emplace_back(measurement1Metadata);
    m_measurementMetadata.emplace_back(measurement2Metadata);
    m_measurementMetadata.emplace_back(measurement3Metadata);
    m_measurementMetadata.emplace_back(measurement4Metadata);

    // Add a phasor
    PhasorMetadataPtr phasor1Metadata = NewSharedPtr<PhasorMetadata>();
    phasor1Metadata->DeviceAcronym = device1Metadata->Acronym;
    phasor1Metadata->Label = device1Metadata->Name + " Voltage Phasor";
    phasor1Metadata->Type = "V";      // Voltage phasor
    phasor1Metadata->Phase = "+";     // Positive sequence
    phasor1Metadata->SourceIndex = 1; // Phasor number 1
    phasor1Metadata->UpdatedOn = timestamp;

    m_phasorMetadata.emplace_back(phasor1Metadata);

    m_metadataVersion++;

    // Pass meta-data to publisher instance for proper conditioning
    PublisherInstance::DefineMetadata(m_deviceMetadata, m_measurementMetadata, m_phasorMetadata, m_metadataVersion);
}

void PublisherHandler::StartDataPublication()
{
    static constexpr float64_t randMax = RAND_MAX;

    // Setup data publication timer - for this publishing sample we send
    // data type reasonable random values every 33 milliseconds
    m_publishTimer = NewSharedPtr<Timer>(33, [this](const TimerPtr&, void*)
    {
        // If metadata can change, the following integer should not be static:
        static uint32_t count = ConvertUInt32(m_measurementMetadata.size());
        const int64_t timestamp = RoundToSubsecondDistribution(ToTicks(UtcNow()), 30);
        vector<MeasurementPtr> measurements;

        measurements.reserve(count);

        // Create new measurement values for publication
        for (size_t i = 0; i < count; i++)
        {
            const MeasurementMetadataPtr metadata = m_measurementMetadata[i];
            MeasurementPtr measurement = NewSharedPtr<Measurement>();

            measurement->SignalID = metadata->SignalID;
            measurement->Timestamp = timestamp;

            const float64_t randFraction = rand() / randMax;
            const float64_t sign = randFraction > 0.5 ? 1.0 : -1.0;
            float64_t value;

            switch (metadata->Reference.Kind) // NOLINT
            {
                case SignalKind::Frequency:
                    value = 60.0 + sign * randFraction * 0.1;
                    break;
                case SignalKind::DfDt:
                    value = sign * randFraction * 2;
                    break;
                case SignalKind::Magnitude:
                    value = 500 + sign * randFraction * 50;
                    break;
                case SignalKind::Angle:
                    value = sign * randFraction * 180;
                    break;
                default:
                    value = sign * randFraction * UInt32::MaxValue;
                    break;
            }

            measurement->Value = value;

            measurements.push_back(measurement);
        }

        // Publish measurements
        PublishMeasurements(measurements);

        // Display a processing message every few seconds
        if (TimeSince(m_lastMessage) <  10.0F)
            return;

        // Total published measurements will be zero when no clients are connected
        if (m_lastMessage == DateTime::MinValue)
            StatusMessage("Publishing data...\n");
        else
            StatusMessage(ToString(GetTotalMeasurementsSent()) + " measurements published so far...\n");

        m_lastMessage = UtcNow();
    },
    true);

    // Start data publication
    m_publishTimer->Start();
}

void PublisherHandler::Stop()
{
    PublisherInstance::Stop();

    if (m_publishTimer != nullptr)
        m_publishTimer->Stop();
}
