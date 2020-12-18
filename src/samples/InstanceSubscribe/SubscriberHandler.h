//******************************************************************************************************
//  SubscriberHandler.h - Gbtc
//
//  Copyright © 2018, Grid Protection Alliance.  All Rights Reserved.
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

#pragma once

#include "../../lib/CommonTypes.h"
#include "../../lib/transport/SubscriberInstance.h"

class SubscriberHandler : public sttp::transport::SubscriberInstance
{
private:
    std::string m_name;
    uint64_t m_processCount;

    static sttp::Mutex s_coutLock;

protected:
    sttp::transport::SubscriptionInfo CreateSubscriptionInfo() override;
    void SetupSubscriberConnector(sttp::transport::SubscriberConnector& connector) override;
    void StatusMessage(const std::string& message) override;
    void ErrorMessage(const std::string& message) override;
    void DataStartTime(time_t unixSOC, uint16_t milliseconds) override;
    void DataStartTime(sttp::datetime_t startTime) override;
    void ReceivedMetadata(const std::vector<uint8_t>& payload) override;
    void ReceivedNewMeasurements(const std::vector<sttp::transport::MeasurementPtr>& measurements) override;
    void SubscriptionUpdated(const sttp::transport::SignalIndexCachePtr& signalIndexCache) override;
    void ParsedMetadata() override;
    void ConfigurationChanged() override;
    void HistoricalReadComplete() override;
    void ConnectionEstablished() override;
    void ConnectionTerminated() override;

public:
    SubscriberHandler(std::string name);
};

typedef sttp::SharedPtr<SubscriberHandler> SubscriberHandlerPtr;