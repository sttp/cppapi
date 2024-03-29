//******************************************************************************************************
//  TemporalSubscriber.cpp - Gbtc
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
//  03/01/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//.
//******************************************************************************************************

#include "TemporalSubscriber.h"
 
using namespace std;
using namespace sttp;
using namespace sttp::data;

sttp::data::DataSetPtr TemporalSubscriber::s_history = nullptr;

TemporalSubscriber::TemporalSubscriber(SubscriberConnectionPtr connection) :
    m_connection(std::move(connection)),
    m_startTimestamp(ToTicks(m_connection->GetStartTimeConstraint())),
    m_stopTimestamp(ToTicks(m_connection->GetStopTimeConstraint())),
    m_currentTimestamp(m_startTimestamp),
    m_currentRow(0),
    m_stopped(false)
{
    if (s_history == nullptr)
        s_history = DataSet::FromXml("History.xml");

    m_lastRow = s_history->Table("History")->RowCount() - 1;

    if (m_lastRow < 0)
        throw runtime_error("No history available - run with \"GenHistory\" argument.");

    m_processTimer = NewSharedPtr<Timer>(33, [this](const TimerPtr&, void*) { SendTemporalData(); }, true);
    SetProcessingInterval(m_connection->GetProcessingInterval());
    m_processTimer->Start();
}

TemporalSubscriber::~TemporalSubscriber()
{
    CompleteTemporalSubscription();
    m_processTimer.reset();
}

void TemporalSubscriber::SetProcessingInterval(int32_t processingInterval) const
{
    if (processingInterval == -1)
        processingInterval = 33;

    m_processTimer->SetInterval(processingInterval);
}

void TemporalSubscriber::SendTemporalData()
{
    if (m_stopped)
        return;
    
    static DataTable& history = *s_history->Table("History");
    static const int32_t signalIDColumn = history["SignalID"]->Index();
    static const int32_t timestampColumn = history["Timestamp"]->Index();
    static const int32_t valueColumn = history["Value"]->Index();

    vector<MeasurementPtr> measurements;
    DataRow& row = *history.Row(m_currentRow);
    int64_t historyTimestamp = row.ValueAsInt64(timestampColumn).GetValueOrDefault();
    const int64_t groupTimestamp = historyTimestamp;

    while (historyTimestamp == groupTimestamp)
    {
        MeasurementPtr measurement = NewSharedPtr<Measurement>();

        measurement->Timestamp = m_currentTimestamp;
        measurement->SignalID = row.ValueAsGuid(signalIDColumn).GetValueOrDefault();
        measurement->Value = row.ValueAsDouble(valueColumn).GetValueOrDefault();

        measurements.push_back(measurement);

        if (++m_currentRow > m_lastRow)
            m_currentRow = 0;

        row = *history.Row(m_currentRow);
        historyTimestamp = row.ValueAsInt64(timestampColumn).GetValueOrDefault();
    }

    m_connection->PublishMeasurements(measurements);

    // Setup next publication timestamp
    m_currentTimestamp += HistoryInterval;

    if (m_currentTimestamp > m_stopTimestamp)
        CompleteTemporalSubscription();
}

void TemporalSubscriber::CompleteTemporalSubscription()
{
    if (m_stopped)
        return;

    m_stopped = true;
    m_processTimer->Stop();
    m_connection->CancelTemporalSubscription();
}

bool TemporalSubscriber::GetIsStopped() const
{
    return m_stopped;
}
