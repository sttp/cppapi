//******************************************************************************************************
//  CompactMeasurement.h - Gbtc
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
//  03/09/2012 - Stephen C. Wills
//       Generated original version of source code.
//  02/06/2019 - J. Ritchie Carroll
//       Added format serialization method.
//
//******************************************************************************************************

#pragma once

#include "TransportTypes.h"
#include "SignalIndexCache.h"

namespace sttp::transport
{
    // Encoder / decoder for the compact measurement format of the Streaming Telemetry Transport Protocol.
    class CompactMeasurement
    {
    private:
        SignalIndexCachePtr m_signalIndexCache;
        int64_t* m_baseTimeOffsets;
        bool m_includeTime;
        bool m_useMillisecondResolution;
        int32_t m_timeIndex;

        // Gets the calculated byte length of compact measurement
        uint32_t GetBinaryLength(bool usingBaseTimeOffset) const;

    public:
        // Creates a new instance of the compact measurement parser.
        explicit CompactMeasurement(SignalIndexCachePtr signalIndexCache, int64_t* baseTimeOffsets = nullptr, bool includeTime = true, bool useMillisecondResolution = false, int32_t timeIndex = 0);

        // Attempts to parse a measurement from the buffer. Return value of false indicates
        // that there is not enough data to parse the measurement. Offset and length will be
        // updated by this method to indicate how many bytes were used when parsing.
        bool TryParseMeasurement(const uint8_t* data, uint32_t& offset, uint32_t length, MeasurementPtr& measurement) const;

        // Serializes a measurement into a buffer
        uint32_t SerializeMeasurement(const Measurement& measurement, std::vector<uint8_t>& buffer, int32_t runtimeID) const;
    };
}
