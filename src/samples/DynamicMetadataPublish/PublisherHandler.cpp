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

#include "PublisherHandler.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;

Mutex PublisherHandler::s_coutLock{};

PublisherHandler::PublisherHandler(string name) :
    m_name(std::move(name)),
    m_processCount(0L),
    m_metadataVersion(0)
{
}

void PublisherHandler::StatusMessage(const string& message)
{
    // TODO: Make sure these messages get logged to an appropriate location

    // Calls can come from multiple threads, so we impose a simple lock before write to console
    ScopeLock lock(s_coutLock);

    // For now, we just the base class to display to console:
    PublisherInstance::StatusMessage("[" + m_name + "] " + message);
}

void PublisherHandler::ErrorMessage(const string& message)
{
    // TODO: Make sure these messages get logged to an appropriate location

    // Calls can come from multiple threads, so we impose a simple lock before write to console
    ScopeLock lock(s_coutLock);

    // For now, we just the base class to display to console:
    PublisherInstance::ErrorMessage("[" + m_name + "] " + message);
}

void PublisherHandler::ClientConnected(const SubscriberConnectionPtr& connection)
{
    StatusMessage("Client \"" + connection->GetConnectionID() + "\" with subscriber ID " + ToString(connection->GetSubscriberID()) + " connected...\n\n");
}

void PublisherHandler::ClientDisconnected(const SubscriberConnectionPtr& connection)
{
    StatusMessage("Client \"" + connection->GetConnectionID() + "\" with subscriber ID " + ToString(connection->GetSubscriberID()) + " disconnected...\n\n");
}

// IMPORTANT: Guids for defined measurements should be persisted between runs. However, for this sample
// we create new Guids for all new metadata, but they remain consistent for the life of the publisher
// which is sufficient for testing purposes in the sample applications.
void PublisherHandler::DefineMetadata(int32_t devices)
{
    static int32_t accessID = 1;
    static int32_t runtimeIndex = 1;

    WriterLock writeLock(m_measurementUpdateLock);

    StatusMessage("Updating meta-data with " + ToString(devices) + " devices of metadata...");

    // This sample just generates random Guid measurement and device identifiers - for a production system,
    // these Guid values would need to persist between runs defining a permanent association between the
    // defined metadata and the identifier...

    // Add device metadata incrementally so that Guids remain consistent for duration of publisher lifetime...
    for (int32_t i = static_cast<int>(m_deviceMetadata.size()); i < devices; i++)
    {
        DeviceMetadataPtr device1Metadata = NewSharedPtr<DeviceMetadata>();
        const datetime_t timestamp = UtcNow();

        // Add a device
        device1Metadata->Name = "Test PMU " + ToString(i);
        device1Metadata->Acronym = ToUpper(Replace(device1Metadata->Name, " ", "", false));
        device1Metadata->UniqueID = NewGuid();
        device1Metadata->Longitude = 300;
        device1Metadata->Latitude = 200;
        device1Metadata->FramesPerSecond = 30;
        device1Metadata->ProtocolName = "STTP";
        device1Metadata->UpdatedOn = timestamp;
        device1Metadata->AccessID = accessID++;

        m_deviceMetadata.emplace_back(device1Metadata);

        const string& pointTagPrefix = device1Metadata->Acronym + ".";
        const string& measurementSource = "PPA:";

        // Add a frequency measurement
        MeasurementMetadataPtr measurement1Metadata = NewSharedPtr<MeasurementMetadata>();
        measurement1Metadata->ID = measurementSource + ToString(runtimeIndex++);
        measurement1Metadata->PointTag = pointTagPrefix + "FREQ";
        measurement1Metadata->SignalID = NewGuid();
        measurement1Metadata->DeviceAcronym = device1Metadata->Acronym;
        measurement1Metadata->Reference.Acronym = device1Metadata->Acronym;
        measurement1Metadata->Reference.Kind = Frequency;
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
        measurement2Metadata->Reference.Kind = DfDt;
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
        measurement3Metadata->Reference.Kind = Angle;
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
        measurement4Metadata->Reference.Kind = Magnitude;
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
    }

    // Pass meta-data to publisher instance for proper conditioning
    PublisherInstance::DefineMetadata(m_deviceMetadata, m_measurementMetadata, m_phasorMetadata, m_metadataVersion);
}

void PublisherHandler::Start(uint16_t port, bool ipV6)
{
    PublisherInstance::Start(port, ipV6);

    static float64_t randMax = float64_t(RAND_MAX);
    static const uint64_t interval = 1000;

    const int32_t maxConnections = GetMaximumAllowedConnections();
    StatusMessage("\nListening on port: " + ToString(GetPort()) + ", max connections = " + (maxConnections == -1 ? "unlimited" : ToString(maxConnections)) + "...\n");

    // Setup meta-data
    DefineMetadata();

    // Setup data publication timer - for this publishing sample we send
    // data type reasonable random values every 33 milliseconds
    m_publishTimer = NewSharedPtr<Timer>(33, [this](Timer*, void*)
    {
        ReaderLock readLock(m_measurementUpdateLock);

        const uint32_t count = ConvertUInt32(m_measurementMetadata.size());
        const int64_t timestamp = ToTicks(UtcNow());
        vector<MeasurementPtr> measurements;

        measurements.reserve(count);

        // Create new measurement values for publication
        for (uint32_t i = 0; i < count; i++)
        {
            const MeasurementMetadataPtr metadata = m_measurementMetadata[i];
            MeasurementPtr measurement = NewSharedPtr<Measurement>();

            measurement->SignalID = metadata->SignalID;
            measurement->Timestamp = timestamp;

            const float64_t randFraction = rand() / randMax;
            const float64_t sign = randFraction > 0.5 ? 1.0 : -1.0;
            float64_t value;

            switch (metadata->Reference.Kind)
            {
            case Frequency:
                value = 60.0 + sign * randFraction * 0.1;
                break;
            case DfDt:
                value = sign * randFraction * 2;
                break;
            case Magnitude:
                value = 500 + sign * randFraction * 50;
                break;
            case Angle:
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
        const bool showMessage = m_processCount + count >= (m_processCount / interval + 1) * interval && GetTotalMeasurementsSent() > 0;
        m_processCount += count;


        if (showMessage)
            StatusMessage(ToString(GetTotalMeasurementsSent()) + " measurements published so far...\n");
    },
    true);

    // Start data publication
    m_publishTimer->Start();
}

void PublisherHandler::Stop()
{
    PublisherInstance::Stop();

    m_publishTimer->Stop();
}