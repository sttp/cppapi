//******************************************************************************************************
//  TemporalSubscriber.h - Gbtc
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
//  03/01/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "../../lib/CommonTypes.h"
#include "../../lib/transport/SubscriberConnection.h"

using namespace sttp::transport;

class TemporalSubscriber // NOLINT
{
private:
    const SubscriberConnectionPtr m_connection;
    int64_t m_startTimestamp;
    int64_t m_stopTimestamp;
    int64_t m_currentTimestamp;
    int32_t m_currentRow;
    int32_t m_lastRow;
    sttp::TimerPtr m_processTimer;
    bool m_stopped;

    void SendTemporalData();

    static sttp::data::DataSetPtr s_history;

public:
    TemporalSubscriber(SubscriberConnectionPtr connection);
    ~TemporalSubscriber();

    void SetProcessingInterval(int32_t processingInterval) const;
    void CompleteTemporalSubscription();

    bool GetIsStopped() const;

    static constexpr const int64_t HistoryInterval = sttp::Ticks::PerMillisecond * 33L;
};

typedef sttp::SharedPtr<TemporalSubscriber> TemporalSubscriberPtr;
