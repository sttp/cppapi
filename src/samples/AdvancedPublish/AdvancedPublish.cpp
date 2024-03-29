//******************************************************************************************************
//  AdvancedPublish.cpp - Gbtc
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
//  02/14/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

// ReSharper disable CppAssignedValueIsNeverUsed
// ReSharper disable CppClangTidyConcurrencyMtUnsafe
#include "../../lib/transport/DataPublisher.h"
#include "GenHistory.h"
#include "TemporalSubscriber.h"
#include <iostream>

using namespace std;
using namespace sttp;
using namespace sttp::data;
using namespace sttp::transport;
using namespace sttp::filterexpressions;

DataPublisherPtr Publisher;
GenHistoryPtr HistoryGenerator;
TimerPtr PublishTimer;
vector<DeviceMetadataPtr> DevicesToPublish;
vector<MeasurementMetadataPtr> MeasurementsToPublish;
vector<PhasorMetadataPtr> PhasorsToPublish;
unordered_map<sttp::Guid, TemporalSubscriberPtr> TemporalSubscriptions;
Mutex TemporalSubscriptionsLock;
const TemporalSubscriberPtr NullTemporalSubscription = nullptr;

bool RunPublisher(uint16_t port, bool genHistory);

void DisplayClientConnected(DataPublisher* source, const SubscriberConnectionPtr& connection);
void DisplayClientDisconnected(DataPublisher* source, const SubscriberConnectionPtr& connection);
void DisplayStatusMessage(DataPublisher* source, const string& message);
void DisplayErrorMessage(DataPublisher* source, const string& message);
void HandleProcessingIntervalChangeRequested(DataPublisher* source, const SubscriberConnectionPtr& connection);
void HandleTemporalSubscriptionRequested(DataPublisher* source, const SubscriberConnectionPtr& connection);
void HandleTemporalSubscriptionCanceled(DataPublisher* source, const SubscriberConnectionPtr& connection);

bool UpdateTemporalSubscriptionProcessingInterval(const SubscriberConnectionPtr& connection);
TemporalSubscriberPtr CreateNewTemporalSubscription(const SubscriberConnectionPtr& connection);
bool RemoveTemporalSubscription(const SubscriberConnectionPtr& connection, bool& completed);

void LoadMetadataToPublish(vector<DeviceMetadataPtr>& deviceMetadata, vector<MeasurementMetadataPtr>& measurementMetadata, vector<PhasorMetadataPtr>& phasorMetadata)
{
    DeviceMetadataPtr device1Metadata = NewSharedPtr<DeviceMetadata>();
    const datetime_t timestamp = UtcNow();

    // Add a device
    device1Metadata->Name = "Test PMU";
    device1Metadata->Acronym = ToUpper(Replace(device1Metadata->Name, " ", "", false));
    device1Metadata->UniqueID = ParseGuid("933690ab-71e1-4c56-ab54-097f5ed8db34");
    device1Metadata->Longitude = 300;
    device1Metadata->Latitude = 200;
    device1Metadata->FramesPerSecond = 30;
    device1Metadata->ProtocolName = "STTP";
    device1Metadata->UpdatedOn = timestamp;

    deviceMetadata.emplace_back(device1Metadata);

    const string& pointTagPrefix = device1Metadata->Acronym + ".";
    const string& measurementSource = "PPA:";
    int runtimeIndex = 1;

    // Add a frequency measurement
    MeasurementMetadataPtr measurement1Metadata = NewSharedPtr<MeasurementMetadata>();
    measurement1Metadata->ID = measurementSource + ToString(runtimeIndex++);
    measurement1Metadata->PointTag = pointTagPrefix + "FREQ";
    measurement1Metadata->SignalID = ParseGuid("6586f230-8e7f-4f0f-9e18-1eefee4b9edd");
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
    measurement2Metadata->SignalID = ParseGuid("60c97530-2ed2-4abb-a7a2-99e2170479a4");
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
    measurement3Metadata->SignalID = ParseGuid("aa47a61c-8596-46af-8c28-f9ee774bcf26");
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
    measurement4Metadata->SignalID = ParseGuid("4ab24720-3763-407c-afa0-15f0d69ac897");
    measurement4Metadata->DeviceAcronym = device1Metadata->Acronym;
    measurement4Metadata->Reference.Acronym = device1Metadata->Acronym;
    measurement4Metadata->Reference.Kind = SignalKind::Magnitude;
    measurement4Metadata->Reference.Index = 1;   // First phase magnitude
    measurement4Metadata->PhasorSourceIndex = 1; // Match to Phasor.SourceIndex = 1
    measurement4Metadata->UpdatedOn = timestamp;

    measurementMetadata.emplace_back(measurement1Metadata);
    measurementMetadata.emplace_back(measurement2Metadata);
    measurementMetadata.emplace_back(measurement3Metadata);
    measurementMetadata.emplace_back(measurement4Metadata);

    // Add a phasor
    PhasorMetadataPtr phasor1Metadata = NewSharedPtr<PhasorMetadata>();
    phasor1Metadata->DeviceAcronym = device1Metadata->Acronym;
    phasor1Metadata->Label = device1Metadata->Name + " Voltage Phasor";
    phasor1Metadata->Type = "V";      // Voltage phasor
    phasor1Metadata->Phase = "+";     // Positive sequence
    phasor1Metadata->SourceIndex = 1; // Phasor number 1
    phasor1Metadata->UpdatedOn = timestamp;

    phasorMetadata.emplace_back(phasor1Metadata);
}

// Sample application to demonstrate the more advanced use of the publisher API.
//
// This application accepts the port of the publisher via command line argument,
// starts listening for subscriber connections, the displays summary information
// about the measurements it publishes. It provides four manually defined
// measurements, i.e., PPA:1 through PPA:4
//
// Measurements are transmitted via the TCP command channel.
int main(int argc, char* argv[])
{
    uint16_t port;

    // Ensure that the necessary
    // command line arguments are given.
    if (argc < 2)
    {
        cout << "Usage:" << endl;
        cout << "    AdvancedPublish PORT" << endl;
        return 0;
    }

    // Get hostname and port.
    stringstream(argv[1]) >> port;
    const bool genHistory = argc > 2 && IsEqual(argv[2], "GenHistory");

    // Run the publisher.
    const bool publisherRunning = RunPublisher(port, genHistory);

    // Wait until the user presses enter before quitting.
    string line;
    getline(cin, line);

    // Stop data publication
    if (publisherRunning)
        PublishTimer->Stop();

    if (genHistory)
        HistoryGenerator->StopArchive();

    cout << "Publisher stopped." << endl;

    return 0;
}

bool RunPublisher(const uint16_t port, const bool genHistory)
{
    constexpr float64_t randMax = RAND_MAX;
    string errorMessage;
    bool running = false;

    try
    {
        Publisher = NewSharedPtr<DataPublisher>();
        Publisher->Start(port);
        running = true;
    }
    catch (PublisherException& ex)
    {
        errorMessage = ex.what();
    }
    catch (SystemError& ex)
    {
        errorMessage = ex.what();
    }
    catch (...)
    {
        errorMessage = boost::current_exception_diagnostic_information(true);
    }

    if (running)
    {
        cout << endl << "Listening on port: " << port << "..." << endl << endl;

        // Register callbacks
        Publisher->RegisterClientConnectedCallback(&DisplayClientConnected);
        Publisher->RegisterClientDisconnectedCallback(&DisplayClientDisconnected);
        Publisher->RegisterStatusMessageCallback(&DisplayStatusMessage);
        Publisher->RegisterErrorMessageCallback(&DisplayErrorMessage);
        Publisher->RegisterProcessingIntervalChangeRequestedCallback(&HandleProcessingIntervalChangeRequested);
        Publisher->RegisterTemporalSubscriptionRequestedCallback(&HandleTemporalSubscriptionRequested);
        Publisher->RegisterTemporalSubscriptionCanceledCallback(&HandleTemporalSubscriptionCanceled);

        // Enable temporal subscription support - this allows historical data requests as well as real-time
        Publisher->SetSupportsTemporalSubscriptions(true);

        // Load metadata to be used for publication
        LoadMetadataToPublish(DevicesToPublish, MeasurementsToPublish, PhasorsToPublish);
        Publisher->DefineMetadata(DevicesToPublish, MeasurementsToPublish, PhasorsToPublish);

        cout << "Loaded " << MeasurementsToPublish.size() << " measurement metadata records for publication:" << endl;

        for (size_t i = 0; i < MeasurementsToPublish.size(); i++)
            cout << "    " << MeasurementsToPublish[i]->PointTag << std::endl;

        cout << endl;

        // Setup data publication timer - for this publishing sample we send
        // data type reasonable random values every 33 milliseconds
        PublishTimer = NewSharedPtr<Timer>(33, [](const TimerPtr&, void*)
        {
            // If metadata can change, the following integer should not be static:
            static uint32_t count = ConvertUInt32(MeasurementsToPublish.size());
            const int64_t timestamp = RoundToSubsecondDistribution(ToTicks(UtcNow()), 30);
            vector<MeasurementPtr> measurements;

            measurements.reserve(count);

            // Create new measurement values for publication
            for (size_t i = 0; i < count; i++)
            {
                const MeasurementMetadataPtr metadata = MeasurementsToPublish[i];
                MeasurementPtr measurement = NewSharedPtr<Measurement>();

                measurement->SignalID = metadata->SignalID;
                measurement->Timestamp = timestamp;

                const float64_t randFraction = rand() / randMax;
                const float64_t sign = randFraction > 0.5 ? 1.0 : -1.0;
                float64_t value;

                switch (metadata->Reference.Kind)  // NOLINT
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
            Publisher->PublishMeasurements(measurements);
        },
        true);

        // Start data publication
        PublishTimer->Start();

        if (genHistory)
        {
            HistoryGenerator = NewSharedPtr<GenHistory>(port);
            HistoryGenerator->StartArchive();
        }            
    }
    else
    {
        cerr << "Failed to listen on port: " << port << ": " << errorMessage;
    }

    return running;
}

void DisplayClientConnected(DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    stringstream message;

    message << ">> New Client Connected:" << endl;
    message << "   Subscriber ID: " << ToString(connection->GetSubscriberID()) << endl;
    message << "   Connection ID: " << ToString(connection->GetConnectionID());

    DisplayStatusMessage(source, message.str());
}

void DisplayClientDisconnected(DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    stringstream message;

    message << ">> Client Disconnected:" << endl;
    message << "   Subscriber ID: " << ToString(connection->GetSubscriberID()) << endl;
    message << "   Connection ID: " << ToString(connection->GetConnectionID());

    DisplayStatusMessage(source, message.str());
}

// Callback which is called to display status messages from the subscriber.
void DisplayStatusMessage(DataPublisher* source, const string& message)
{
    cout << message << endl << endl;
}

// Callback which is called to display error messages from the connector and subscriber.
void DisplayErrorMessage(DataPublisher* source, const string& message)
{
    cerr << message << endl << endl;
}

void HandleProcessingIntervalChangeRequested(DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    if (!UpdateTemporalSubscriptionProcessingInterval(connection))
        return;

    stringstream message;

    message << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " has requested to change its temporal processing interval to " << ToString(connection->GetProcessingInterval()) << "ms";

    DisplayStatusMessage(source, message.str());
}

void HandleTemporalSubscriptionRequested(DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    stringstream message;
    bool completed;
    
    message << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " has requested a temporal subscription starting at " << ToString(connection->GetStartTimeConstraint()) << endl;

    RemoveTemporalSubscription(connection, completed);
    
    if (CreateNewTemporalSubscription(connection))
    {
        const size_t count = TemporalSubscriptions.size();
        message << "Created new temporal subscription - " <<  count << (count == 1 ? " is" : " are") << " now active...";
    }

    DisplayStatusMessage(source, message.str());
}

void HandleTemporalSubscriptionCanceled(DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    bool completed;

    if (!RemoveTemporalSubscription(connection, completed))
        return;

    stringstream message;
    const size_t count = TemporalSubscriptions.size();

    message << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << (completed ? " completed" : " canceled") << " temporal subscription starting at " << ToString(connection->GetStartTimeConstraint()) << endl;
    message << "Temporal subscription removed - " << count << (count == 1 ? " is" : " are") << " now active...";

    DisplayStatusMessage(source, message.str());
}

bool UpdateTemporalSubscriptionProcessingInterval(const SubscriberConnectionPtr& connection)
{
    const sttp::Guid& instanceID = connection->GetInstanceID();
    TemporalSubscriberPtr temporalSubscription;
    const int32_t processingInterval = connection->GetProcessingInterval();
    bool updated = false;

    TemporalSubscriptionsLock.lock();

    if (TryGetValue(TemporalSubscriptions, instanceID, temporalSubscription, NullTemporalSubscription))
    {
        temporalSubscription->SetProcessingInterval(processingInterval);
        updated = true;
    }

    TemporalSubscriptionsLock.unlock();

    return updated;
}

TemporalSubscriberPtr CreateNewTemporalSubscription(const SubscriberConnectionPtr& connection)
{
    const sttp::Guid& instanceID = connection->GetInstanceID();

    TemporalSubscriptionsLock.lock();

    TemporalSubscriberPtr temporalSubscription = NewSharedPtr<TemporalSubscriber>(connection);
    TemporalSubscriptions.insert(pair(instanceID, temporalSubscription));

    TemporalSubscriptionsLock.unlock();

    return temporalSubscription;
}

bool RemoveTemporalSubscription(const SubscriberConnectionPtr& connection, bool& completed)
{
    const sttp::Guid& instanceID = connection->GetInstanceID();
    TemporalSubscriberPtr temporalSubscription;
    bool removed = false;

    TemporalSubscriptionsLock.lock();

    if (TryGetValue(TemporalSubscriptions, instanceID, temporalSubscription, NullTemporalSubscription))
    {
        completed = temporalSubscription->GetIsStopped();
        temporalSubscription->CompleteTemporalSubscription();
        TemporalSubscriptions.erase(instanceID);
        temporalSubscription.reset();
        removed = true;
    }

    TemporalSubscriptionsLock.unlock();

    return removed;
}
