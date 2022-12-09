//******************************************************************************************************
//  ReversePublish.cpp - Gbtc
//
//  Copyright © 2022, Grid Protection Alliance.  All Rights Reserved.
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
//  12/08/2022 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include "PublisherHandler.h"
#include <iostream>

using namespace std;
using namespace sttp;
using namespace sttp::transport;

PublisherHandler* Publisher;

int main(int argc, char* argv[])
{
    // Ensure that the necessary command line arguments are provided.
    if (argc < 3)
    {
        cout << "Usage:" << endl;
        cout << "    ReversePublish HOSTNAME PORT" << endl;
        return 0;
    }

    // Get host name port.
    const string hostname = argv[1];
    uint16_t port;
    stringstream(argv[2]) >> port;

    // Maintain the life-time of PublisherHandler instances within main
    Publisher = new PublisherHandler();

    // Setup metadata
    Publisher->DefineMetadata();

    // Start data publication - this is just a timer to simulate data. In production
    // measurements would simply be passed to PublishMeasurements when received.
    Publisher->StartDataPublication();

    // The following code (up to Connect) is the only thing special about
    // a data publisher in reverse connection mode:
    Publisher->Initialize(hostname, port);

    // Available reverse connection settings:
    //Publisher->SetAutoReconnect(true);
    //Publisher->SetMaxRetries(-1);
    //Publisher->SetRetryInterval(2000);
    //Publisher->SetMaxRetryInterval(120000);

    // We call Connect instead of Start to initiate reverse connection mode
    Publisher->Connect();

    // Wait until the user presses enter before quitting.
    string line;
    getline(cin, line);

    // Stop publisher - this stops publication
    Publisher->Stop();

    // Delete publisher instances - this closes sockets
    delete Publisher;

    cout << "Publisher stopped." << endl;

    return 0;
}