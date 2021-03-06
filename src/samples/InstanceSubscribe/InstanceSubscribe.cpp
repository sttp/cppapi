//******************************************************************************************************
//  InstanceSubscribe.cpp - Gbtc
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
//  03/27/2018 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include <iostream>
#include "SubscriberHandler.h"
#include "../../lib/Convert.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;

#define TotalInstances 3
SubscriberHandler* Subscriber[TotalInstances];

int main(int argc, char* argv[])
{
    string hostname;
    uint16_t port;
    bool usePortOffset = false;

    // Ensure that the necessary
    // command line arguments are given.
    if (argc < 3)
    {
        cout << "Usage:" << endl;
        cout << "    InstanceSubscribe HOSTNAME PORT" << endl;
        return 0;
    }

    // Get hostname and port.
    hostname = argv[1];
    stringstream(argv[2]) >> port;

    if (argc > 3)
        TryParseBoolean(argv[3], usePortOffset);

    // Initialize the subscribers.
    for (uint32_t i = 0; i < TotalInstances; i++)
    {
        // Maintain the life-time of SubscriberHandler instances within main
        SubscriberHandler* subscriber = new SubscriberHandler("Subscriber " + ToString(i + 1));
        subscriber->Initialize(hostname, port + static_cast<uint16_t>(usePortOffset ? i : 0));

        switch (i)
        {
            case 0:
                subscriber->SetFilterExpression("FILTER ActiveMeasurements WHERE SignalType = 'FREQ'");
                break;
            case 1:
                subscriber->SetFilterExpression("FILTER ActiveMeasurements WHERE SignalType LIKE '%PHA'");
                
                // In this example we also specify a meta-data filtering expression:
                subscriber->SetMetadataFilters(SubscriberInstance::FilterMetadataStatsExpression);
                break;
            case 2:
                subscriber->SetFilterExpression("FILTER ActiveMeasurements WHERE SignalType LIKE '%PHM'");
                break;
            default:
                subscriber->SetFilterExpression(SubscriberInstance::SubscribeAllNoStatsExpression);
                break;
        }

        subscriber->ConnectAsync();
        Subscriber[i] = subscriber;
    }

    // Wait until the user presses enter before quitting.
    string line;
    getline(cin, line);

    // Shutdown subscriber instances
    for (uint32_t i = 0; i < TotalInstances; i++)
        Subscriber[i]->Disconnect();

    // Disconnect the subscriber to stop background threads.
    cout << "Disconnected." << endl;

    // Delete subscriber instances
    for (uint32_t i = 0; i < TotalInstances; i++)
        delete Subscriber[i];

    return 0;
}