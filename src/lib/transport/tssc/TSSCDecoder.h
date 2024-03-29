//******************************************************************************************************
//  TSSCDecoder.h - Gbtc
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
//  12/02/2016 - Steven E. Chisholm
//       Generated original version of source code.
//  04/10/2019 - J. Ritchie Carroll
//       Migrated code to C++.
//
//******************************************************************************************************

#pragma once

#include "TSSCPointMetadata.h"

namespace sttp::transport::tssc
{
    class TSSCDecoder;
    typedef SharedPtr<TSSCDecoder> TSSCDecoderPtr;

    // Decoder for the Time-Series Special Compression algorithm of the Streaming Telemetry Transport Protocol.
    class TSSCDecoder final
    {
    private:
        uint8_t* m_data;
        uint32_t m_position;
        uint32_t m_lastPosition;

        int64_t m_prevTimestamp1;
        int64_t m_prevTimestamp2;

        int64_t m_prevTimeDelta1;
        int64_t m_prevTimeDelta2;
        int64_t m_prevTimeDelta3;
        int64_t m_prevTimeDelta4;

        TSSCPointMetadataPtr m_lastPoint;
        std::vector<TSSCPointMetadataPtr> m_points;

        // The number of bits in m_bitStreamCache that are valid. 0 Means the bitstream is empty.
        int32_t m_bitStreamCount;

        // A cache of bits that need to be flushed to m_buffer when full. Bits filled starting from the right moving left.
        int32_t m_bitStreamCache;

        uint16_t m_sequenceNumber;

        void DecodePointID(uint8_t code, const TSSCPointMetadataPtr& lastPoint);
        int64_t DecodeTimestamp(uint8_t code);
        uint32_t DecodeQuality(uint8_t code, const TSSCPointMetadataPtr& nextPoint);

        bool BitStreamIsEmpty() const;
        void ClearBitStream();

        TSSCPointMetadataPtr NewTSSCPointMetadata();

    public:
        // Creates a new instance of the TSSC decoder.
        TSSCDecoder();

        // Resets the TSSC Decoder to the initial state.
        void Reset();

        // Sets the internal buffer to read data from.
        void SetBuffer(uint8_t* data, uint32_t offset, uint32_t length);

        // Reads the next measurement from the stream. If the end of the stream has been encountered, return false.
        bool TryGetMeasurement(int32_t& id, int64_t& timestamp, uint32_t& quality, float32_t& value);

        int32_t ReadBit();
        int32_t ReadBits4();
        int32_t ReadBits5();

        uint16_t GetSequenceNumber() const;
        void ResetSequenceNumber();
        void IncrementSequenceNumber();

        static const TSSCDecoderPtr NullPtr;
    };
}
