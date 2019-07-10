//******************************************************************************************************
//  InstancePublish.cpp - Gbtc
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
//  03/27/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include <iostream>
#include "PublisherHandler.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;

const int32_t UpdateInterval = 10000;
const int32_t MaxDeviceCount = 20;
int32_t deviceCount = 1;

int main(int argc, char* argv[])
{
    uint16_t port;
    bool autoUpdateMetadata;

    // Ensure that the necessary
    // command line arguments are given.
    if (argc < 2)
    {
        cout << "Usage:" << endl;
        cout << "    InstancePublish PORT [AutoUpdateMetadata?]" << endl;
        return 0;
    }

    // Get port.
    stringstream(argv[1]) >> port;

    if (argc > 2)
        autoUpdateMetadata = ParseBoolean(argv[2]);
    else
        autoUpdateMetadata = false;

    PublisherHandler* publisher = new PublisherHandler("Publisher");
    string line;
    
    if (publisher->Start(port, false))
    {
        if (autoUpdateMetadata)
        {
            // Setup thread to continually update meta-data
            Timer metadataUpdate(UpdateInterval, [publisher](Timer* timer, void*)
            {
                publisher->DefineMetadata(++deviceCount);

                if (deviceCount >= MaxDeviceCount)
                {
                    timer->Stop();
                    publisher->StatusMessage("Metadata update timer stopped: reached configured device limit of " + ToString(MaxDeviceCount) + ".");
                }
            },
            true);

            metadataUpdate.Start();

            getline(cin, line);

            metadataUpdate.Stop();
        }
        else
        {
            cout << "At any time, enter the number of devices to define in meta-data and press enter to update meta-data. Type 'exit' and press enter to quit.\r\n\r\n";

            getline(cin, line);

            while (line != "exit")
            {
                int32_t devices;

                if (TryParseInt32(line, devices))
                    publisher->DefineMetadata(devices);

                getline(cin, line);                
            }
        }
    }

    publisher->Stop();
    delete publisher;

    cout << "Publishers stopped." << endl;

    return 0;
}