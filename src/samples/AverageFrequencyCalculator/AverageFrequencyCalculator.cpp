//******************************************************************************************************
//  AverageFrequencyCalculator.cpp - Gbtc
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
//  04/25/2012 - Stephen C. Wills
//       Generated original version of source code.
//
//******************************************************************************************************

#include "../../lib/transport/DataSubscriber.h"
#include "../../lib/Convert.h"
#include <iostream>
#include <string>

using namespace std;
using namespace sttp;
using namespace sttp::transport;

DataSubscriber* Subscriber;
SubscriptionInfo Info;

// Create helper objects for subscription.
void SetupSubscriberConnector(SubscriberConnector& connector, const string& hostname, uint16_t port);
SubscriptionInfo CreateSubscriptionInfo();

// Handlers for subscriber callbacks.
void Resubscribe(DataSubscriber* source);
void ProcessMeasurements(const DataSubscriber* source, const vector<MeasurementPtr>& measurements);
void ConfigurationChanged(DataSubscriber* source);
void DisplayStatusMessage(DataSubscriber* source, const string& message);
void DisplayErrorMessage(DataSubscriber* source, const string& message);

// Runs the subscriber.
void RunSubscriber(const string& hostname, uint16_t port);

// Sample application to demonstrate average frequency calculation using the subscriber API.
int main(int argc, char* argv[])
{
    string hostname;
    uint16_t port;

    // Ensure that the necessary
    // command line arguments are given.
    if (argc < 3)
    {
        cout << "Usage:" << endl;
        cout << "    AverageFrequencyCalculator HOSTNAME PORT" << endl;
        return 0;
    }

    // Get hostname and port.
    hostname = argv[1];
    stringstream(argv[2]) >> port;

    // Maintain the life-time of DataSubscriber within main
    Subscriber = new DataSubscriber();

    // Run the subscriber.
    RunSubscriber(hostname, port);

    // Wait until the user presses enter before quitting.
    string line;
    getline(cin, line);

    // Disconnect the subscriber to stop background threads.
    Subscriber->Disconnect();
    cout << "Disconnected." << endl;

    delete Subscriber;

    return 0;
}

// The proper procedure when creating and running a subscriber is:
//   - Create subscriber
//   - Register callbacks
//   - Connect to publisher
//   - Subscribe
void RunSubscriber(const string& hostname, uint16_t port)
{
    SubscriberConnector& connector = Subscriber->GetSubscriberConnector();

    // Set up helper objects
    SetupSubscriberConnector(connector, hostname, port);
    Info = CreateSubscriptionInfo();

    // Register callbacks
    Subscriber->RegisterStatusMessageCallback(&DisplayStatusMessage);
    Subscriber->RegisterErrorMessageCallback(&DisplayErrorMessage);
    Subscriber->RegisterNewMeasurementsCallback(&ProcessMeasurements);
    Subscriber->RegisterConfigurationChangedCallback(&ConfigurationChanged);

    cout << endl << "Connecting to " << hostname << ":" << port << "..." << endl << endl;

    // Connect and subscribe to publisher
    if (connector.Connect(*Subscriber, Info))
    {
        cout << "Connected! Subscribing to data..." << endl << endl;
        Subscriber->Subscribe();
    }
    else
    {
        cout << "Connection attempts exceeded. Press enter to exit." << endl;
    }
}

SubscriptionInfo CreateSubscriptionInfo()
{
    // SubscriptionInfo is a helper object which allows the user
    // to set up their subscription and reuse subscription settings.
    SubscriptionInfo info;

    info.FilterExpression = "FILTER ActiveMeasurements WHERE SignalType = 'FREQ'";

    // Uncomment to enable optional UDP data channel
    //info.UdpDataChannel = true;
    //info.DataChannelLocalPort = 9600;

    info.IncludeTime = true;
    info.UseLocalClockAsRealTime = false;
    info.UseMillisecondResolution = true;

    // This controls the down-sampling time, in seconds
    info.Throttled = true;
    info.PublishInterval = 1.0;

    return info;
}

void SetupSubscriberConnector(SubscriberConnector& connector, const string& hostname, uint16_t port)
{
    // SubscriberConnector is another helper object which allows the
    // user to modify settings for auto-reconnects and retry cycles.
    connector.RegisterErrorMessageCallback(&DisplayErrorMessage);
    connector.RegisterReconnectCallback(&Resubscribe);

    connector.SetHostname(hostname);
    connector.SetPort(port);
    connector.SetMaxRetries(-1);
    connector.SetRetryInterval(2000);
    connector.SetAutoReconnect(true);
}

// Callback which is called when the subscriber has
// received a new packet of measurements from the publisher.
void ProcessMeasurements(const DataSubscriber* source, const vector<MeasurementPtr>& measurements)
{
    const float64_t LoFrequency = 57.0F;
    const float64_t HiFrequency = 62.0F;
    const float64_t HzResolution = 1000.0; // three decimal places

    static unordered_map<Guid, int> m_lastValues;
    MeasurementPtr currentMeasurement;
    Guid signalID;

    float64_t frequency;
    float64_t frequencyTotal;
    float64_t maximumFrequency = LoFrequency;
    float64_t minimumFrequency = HiFrequency;
    int32_t adjustedFrequency;
    int32_t lastValue;
    int32_t total;

    frequencyTotal = 0.0;
    total = 0;

    cout << source->GetTotalMeasurementsReceived() << " measurements received so far..." << endl;

    if (!measurements.empty())
    {
        cout << "Timestamp: " << ToString(measurements[0]->GetDateTime()) << endl;
        cout << "\tPoint\tValue" << endl;

        for (uint32_t i = 0; i < measurements.size(); ++i)
            cout  << '\t' << measurements[i]->ID << '\t' << measurements[i]->Value << endl;

        cout << endl;

        for (uint32_t i = 0; i < measurements.size(); ++i)
        {
            currentMeasurement = measurements[i];
            frequency = currentMeasurement->Value;
            signalID = currentMeasurement->SignalID;
            adjustedFrequency = static_cast<int32_t>(frequency * HzResolution);

            // Do some simple flat line avoidance...
            if (TryGetValue(m_lastValues, signalID, lastValue, 0))
            {
                if (lastValue == adjustedFrequency)
                    frequency = 0.0; // Don't contribute duplicates to the average
                else
                    m_lastValues[signalID] = adjustedFrequency;
            }
            else
            {
                m_lastValues[signalID] = adjustedFrequency;
            }

            // Validate frequency
            if (frequency > LoFrequency && frequency < HiFrequency)
            {
                frequencyTotal += frequency;

                if (frequency > maximumFrequency)
                    maximumFrequency = frequency;

                if (frequency < minimumFrequency)
                    minimumFrequency = frequency;

                total++;
            }
        }

        if (total > 0)
        {
            cout << "Avg frequency: " << (frequencyTotal / total) << endl;
            cout << "Max frequency: " << maximumFrequency << endl;
            cout << "Min frequency: " << minimumFrequency << endl << endl;
        }
    }
}

// Callback that is called when the subscriber auto-reconnects.
void Resubscribe(DataSubscriber* source)
{
    if (source->IsConnected())
    {
        cout << "Reconnected! Subscribing to data..." << endl << endl;
        source->Subscribe(Info);
    }
    else
    {
        source->Disconnect();
        cout << "Connection retry attempts exceeded. Press enter to exit." << endl;
    }
}

void ConfigurationChanged(DataSubscriber* source)
{
    // Resubscribe since new metadata is available - filter expression will be reapplied...
    Subscriber->Subscribe();
}

// Callback which is called to display status messages from the subscriber.
void DisplayStatusMessage(DataSubscriber* source, const string& message)
{
    cout << message << endl << endl;
}

// Callback which is called to display error messages from the connector and subscriber.
void DisplayErrorMessage(DataSubscriber* source, const string& message)
{
    cerr << message << endl << endl;
}