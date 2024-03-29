//******************************************************************************************************
//  SimplePublish.cpp - Gbtc
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
//  01/30/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

// ReSharper disable CppClangTidyConcurrencyMtUnsafe
#include "../../lib/transport/DataPublisher.h"
#include <iostream>

using namespace std;
using namespace sttp;
using namespace sttp::data;
using namespace sttp::transport;
using namespace sttp::filterexpressions;

DataPublisherPtr Publisher;
TimerPtr PublishTimer;
vector<MeasurementMetadataPtr> MeasurementsToPublish;

bool RunPublisher(uint16_t port);
void DisplayClientConnected(DataPublisher* source, const SubscriberConnectionPtr& connection);
void DisplayClientDisconnected(DataPublisher* source, const SubscriberConnectionPtr& connection);
void DisplayStatusMessage(DataPublisher* source, const string& message);
void DisplayErrorMessage(DataPublisher* source, const string& message);

// Sample application to demonstrate the most simple use of the publisher API.
//
// This application accepts the port of the publisher via command line argument,
// starts listening for subscriber connections, the displays summary information
// about the measurements it publishes. It provides fourteen measurements, i.e.,
// PPA:1 through PPA:14
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
        cout << "    SimplePublish PORT" << endl;
        return 0;
    }

    // Get hostname and port.
    stringstream(argv[1]) >> port;

    // Run the publisher.
    if (RunPublisher(port))
    {
        // Wait until the user presses enter before quitting.
        string line;
        getline(cin, line);

        // Stop data publication
        PublishTimer->Stop();
    }

    cout << "Publisher stopped." << endl;

    return 0;
}

bool RunPublisher(const uint16_t port)
{
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

        // Define metadata
        Publisher->DefineMetadata(DataSet::FromXml("Metadata.xml"));

        // Filter metadata for measurements to publish, in this case, all non statistics
        MeasurementsToPublish = Publisher->FilterMetadata("SignalAcronym <> 'STAT'");

        cout << "Loaded " << MeasurementsToPublish.size() << " measurement metadata records for publication." << endl << endl;

        // Setup data publication timer - for this simple publishing sample we just
        // send random values every 33 milliseconds
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
                measurement->Value = static_cast<float64_t>(rand());

                measurements.push_back(measurement);
            }

            // Publish measurements
            Publisher->PublishMeasurements(measurements);
        },
        true);

        // Start data publication
        PublishTimer->Start();
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
