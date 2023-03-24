//******************************************************************************************************
//  ReverseSubscribe.cpp - Gbtc
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

SubscriberHandler* Subscriber;

int main(int argc, char* argv[])
{
    uint16_t port;

    // Ensure that the necessary
    // command line arguments are given.
    if (argc < 2)
    {
        cout << "Usage:" << endl;
        cout << "    ReverseSubscribe PORT" << endl;
        return 0;
    }

    // Get port.
    stringstream(argv[1]) >> port;

    // Initialize the subscriber.
    // Maintain the life-time of SubscriberHandler instances within main
    Subscriber = new SubscriberHandler();

    Subscriber->SetFilterExpression(SubscriberInstance::SubscribeAllNoStatsExpression);
                
    // In this example we also specify a meta-data filtering expression:
    //Subscriber->SetMetadataFilters(SubscriberInstance::FilterMetadataStatsExpression);

    // The following line of code is the only thing special about a data subscriber
    // in reverse connection mode, i.e., execute a listen instead of a connect:
    if (Subscriber->Listen(port))
        Subscriber->StatusMessage("Listening on port " + ToString(port));
    else
        Subscriber->ErrorMessage("Failed to listen on port " + ToString(port));

    // Wait until the user presses enter before quitting.
    string line;
    getline(cin, line);

    // Shutdown subscriber instance
    Subscriber->Disconnect();

    // Disconnect the subscriber to stop background threads.
    cout << "Disconnected." << endl;

    // Delete subscriber instance
    delete Subscriber;

    return 0;
}