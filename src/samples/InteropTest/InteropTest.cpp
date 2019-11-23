//******************************************************************************************************
//  InteropTest.cpp - Gbtc
//
//  Copyright © 2019, Grid Protection Alliance.  All Rights Reserved.
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
//  11/22/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include <iostream>
#include "SubscriberHandler.h"
#include "../../lib/Convert.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;

int main(int argc, char* argv[])
{
    string hostname;
    uint16_t port;
    
    // Ensure that the necessary
    // command line arguments are given.
    if (argc < 3)
    {
        cout << "Usage:" << endl;
        cout << "    InteropTest HOSTNAME PORT" << endl;
        return 0;
    }

    // Get hostname and port.
    hostname = argv[1];
    stringstream(argv[2]) >> port;


    // Initialize the subscribers, maintaining the life-time of SubscriberHandler instances within main
    SubscriberHandler* subscriber = new SubscriberHandler("InteropSubscriber");
    subscriber->Initialize(hostname, port);
    subscriber->SetAutoParseMetadata(false);
    subscriber->SetFilterExpression("FILTER TOP 1 ActiveMeasurements WHERE SignalType='VPHM' ORDER BY ID");
    subscriber->ConnectAsync();

    // Wait until the user presses enter before quitting.
    string line;
    getline(cin, line);

    // Shutdown subscriber instance
    subscriber->Disconnect();

    // Delete subscriber instance
    delete subscriber;

    return 0;
}