//******************************************************************************************************
//  SubscriptionInfo.h - Gbtc
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
//  12/02/2022 - rcarroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "../CommonTypes.h"

namespace sttp::transport
{
    // Info structure used to configure subscriptions.
    struct SubscriptionInfo
    {
        std::string FilterExpression;

        // Down-sampling properties
        bool Throttled;
        float64_t PublishInterval;

        // UDP channel properties
        bool UdpDataChannel;
        uint16_t DataChannelLocalPort;

        // Compact measurement properties
        bool IncludeTime;
        float64_t LagTime;
        float64_t LeadTime;
        bool UseLocalClockAsRealTime;
        bool UseMillisecondResolution;
        bool RequestNaNValueFilter;

        // Temporal playback properties
        std::string StartTime;
        std::string StopTime;
        std::string ConstraintParameters;
        int32_t ProcessingInterval;

        std::string ExtraConnectionStringParameters;

        SubscriptionInfo();
    };
}