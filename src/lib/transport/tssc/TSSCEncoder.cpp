//******************************************************************************************************
//  TSSCEncoder.cpp - Gbtc
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
//  12/02/2016 - Steven E. Chisholm
//       Generated original version of source code.
//  04/10/2019 - J. Ritchie Carroll
//       Migrated code to C++.
//
//******************************************************************************************************

// ReSharper disable CppClangTidyClangDiagnosticUndefinedReinterpretCast
#include "TSSCEncoder.h"
#include "../Constants.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;
using namespace sttp::transport::tssc;

void Encode7BitUInt32(uint8_t* stream, uint32_t& position, uint32_t value);
void Encode7BitUInt64(uint8_t* stream, uint32_t& position, uint64_t value);

TSSCEncoder::TSSCEncoder() :
    m_data(nullptr),
    m_position(0),
    m_lastPosition(0),
    m_prevTimestamp1(0LL),
    m_prevTimestamp2(0LL),
    m_prevTimeDelta1(Int64::MaxValue),
    m_prevTimeDelta2(Int64::MaxValue),
    m_prevTimeDelta3(Int64::MaxValue),
    m_prevTimeDelta4(Int64::MaxValue),    
    m_bitStreamBufferIndex(-1),
    m_bitStreamCacheBitCount(0),
    m_bitStreamCache(0)
{
    m_lastPoint = NewTSSCPointMetadata();
}

void TSSCEncoder::Reset()
{
    m_data = nullptr;
    m_points.clear();
    m_lastPoint = NewTSSCPointMetadata();
    m_position = 0;
    m_lastPosition = 0;
    ClearBitStream();
    m_prevTimeDelta1 = Int64::MaxValue;
    m_prevTimeDelta2 = Int64::MaxValue;
    m_prevTimeDelta3 = Int64::MaxValue;
    m_prevTimeDelta4 = Int64::MaxValue;
    m_prevTimestamp1 = 0LL;
    m_prevTimestamp2 = 0LL;
}

void TSSCEncoder::SetBuffer(uint8_t* data, const uint32_t offset, const uint32_t length)
{
    ClearBitStream();
    m_data = data;
    m_position = offset;
    m_lastPosition = offset + length;
}

uint32_t TSSCEncoder::FinishBlock()
{
    BitStreamFlush();
    return m_position;
}

bool TSSCEncoder::TryAddMeasurement(const int32_t id, const int64_t timestamp, const uint32_t quality, float32_t value)
{
    // If there are fewer than 100 bytes available on the buffer assume that we cannot add any more.
    if (m_lastPosition - m_position < 100)
        return false;

    // Setup tracking for metadata associated with measurement ID and next point to encode
    const int32_t pointCount = ConvertInt32(m_points.size());
    TSSCPointMetadataPtr point = id >= pointCount ? nullptr : m_points[id];
    
    if (point == nullptr)
    {
        point = NewTSSCPointMetadata();

        if (id >= pointCount)
            m_points.resize(static_cast<size_t>(id) + 1, nullptr);

        point->PrevNextPointID1 = id + 1;
        
        m_points[id] = point;
    }

    // Given that the incoming pointID is not known in advance, the current
    // measurement will contain the encoding details for the next.

    // General compression strategy is to use delta-encoding for each
    // measurement component value that is received with the same identity.
    // See https://en.wikipedia.org/wiki/Delta_encoding

    // Delta-encoding sizes are embedded in the stream as type-specific
    // codes using as few bits as possible

    // Encode measurement ID and code for value delta
    if (m_lastPoint->PrevNextPointID1 != id)
        WritePointIDChange(id);

    // Encode measurement timestamp and code for value delta
    if (m_prevTimestamp1 != timestamp)
        WriteTimestampChange(timestamp);

    // Encode measurement quality and code for no-change or 7-bit encoding
    if (point->PrevQuality1 != quality)
        WriteQualityChange(quality, point);

    // Reinterpret bits of floating-point value as unsigned integer for bit manipulation
    const uint32_t valueRaw = *reinterpret_cast<uint32_t*>(&value);

    // Encode measurement value and code for value delta
    if (point->PrevValue1 == valueRaw)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::Value1);
    }
    else if (point->PrevValue2 == valueRaw)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::Value2);
        point->PrevValue2 = point->PrevValue1;
        point->PrevValue1 = valueRaw;
    }
    else if (point->PrevValue3 == valueRaw)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::Value3);
        point->PrevValue3 = point->PrevValue2;
        point->PrevValue2 = point->PrevValue1;
        point->PrevValue1 = valueRaw;
    }
    else if (valueRaw == 0)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::ValueZero);
        point->PrevValue3 = point->PrevValue2;
        point->PrevValue2 = point->PrevValue1;
        point->PrevValue1 = 0;
    }
    else
    {
        const uint32_t bitsChanged = valueRaw ^ point->PrevValue1;

        if (bitsChanged <= Bits4)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::ValueXOR4);
            WriteBits(static_cast<uint8_t>(bitsChanged) & 15, 4);
        }
        else if (bitsChanged <= Bits8)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::ValueXOR8);

            m_data[m_position] = static_cast<uint8_t>(bitsChanged);
            m_position++;
        }
        else if (bitsChanged <= Bits12)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::ValueXOR12);
            WriteBits(static_cast<uint8_t>(bitsChanged) & 15, 4);

            m_data[m_position] = static_cast<uint8_t>(bitsChanged >> 4);
            m_position++;
        }
        else if (bitsChanged <= Bits16)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::ValueXOR16);
            m_data[m_position] = static_cast<uint8_t>(bitsChanged);
            m_data[m_position + 1] = static_cast<uint8_t>(bitsChanged >> 8);
            m_position = m_position + 2;
        }
        else if (bitsChanged <= Bits20)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::ValueXOR20);
            WriteBits(static_cast<uint8_t>(bitsChanged) & 15, 4);

            m_data[m_position] = static_cast<uint8_t>(bitsChanged >> 4);
            m_data[m_position + 1] = static_cast<uint8_t>(bitsChanged >> 12);
            m_position = m_position + 2;
        }
        else if (bitsChanged <= Bits24)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::ValueXOR24);

            m_data[m_position] = static_cast<uint8_t>(bitsChanged);
            m_data[m_position + 1] = static_cast<uint8_t>(bitsChanged >> 8);
            m_data[m_position + 2] = static_cast<uint8_t>(bitsChanged >> 16);
            m_position = m_position + 3;
        }
        else if (bitsChanged <= Bits28)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::ValueXOR28);
            WriteBits(static_cast<uint8_t>(bitsChanged) & 15, 4);

            m_data[m_position] = static_cast<uint8_t>(bitsChanged >> 4);
            m_data[m_position + 1] = static_cast<uint8_t>(bitsChanged >> 12);
            m_data[m_position + 2] = static_cast<uint8_t>(bitsChanged >> 20);
            m_position = m_position + 3;
        }
        else
        {
            m_lastPoint->WriteCode(TSSCCodeWords::ValueXOR32);

            m_data[m_position] = static_cast<uint8_t>(bitsChanged);
            m_data[m_position + 1] = static_cast<uint8_t>(bitsChanged >> 8);
            m_data[m_position + 2] = static_cast<uint8_t>(bitsChanged >> 16);
            m_data[m_position + 3] = static_cast<uint8_t>(bitsChanged >> 24);
            m_position = m_position + 4;
        }

        point->PrevValue3 = point->PrevValue2;
        point->PrevValue2 = point->PrevValue1;
        point->PrevValue1 = valueRaw;
    }

    m_lastPoint = point;

    return true;
}

TSSCPointMetadataPtr TSSCEncoder::NewTSSCPointMetadata()
{
    return NewSharedPtr<TSSCPointMetadata>([this](const int32_t code, const int32_t length) { return WriteBits(code, length); });
}

void TSSCEncoder::WritePointIDChange(const int32_t id)
{
    const uint32_t bitsChanged = static_cast<uint32_t>(id ^ m_lastPoint->PrevNextPointID1);

    if (bitsChanged <= Bits4)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::PointIDXOR4);
        WriteBits(static_cast<uint8_t>(bitsChanged) & 15, 4);
    }
    else if (bitsChanged <= Bits8)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::PointIDXOR8);
        m_data[m_position] = static_cast<uint8_t>(bitsChanged);
        m_position++;
    }
    else if (bitsChanged <= Bits12)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::PointIDXOR12);
        WriteBits(static_cast<uint8_t>(bitsChanged) & 15, 4);

        m_data[m_position] = static_cast<uint8_t>(bitsChanged >> 4);
        m_position++;
    }
    else if (bitsChanged <= Bits16)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::PointIDXOR16);
        m_data[m_position] = static_cast<uint8_t>(bitsChanged);
        m_data[m_position + 1] = static_cast<uint8_t>(bitsChanged >> 8);
        m_position += 2;
    }
    else if (bitsChanged <= Bits20)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::PointIDXOR20);
        WriteBits(static_cast<uint8_t>(bitsChanged) & 15, 4);

        m_data[m_position] = static_cast<uint8_t>(bitsChanged >> 4);
        m_data[m_position + 1] = static_cast<uint8_t>(bitsChanged >> 12);
        m_position += 2;
    }
    else if (bitsChanged <= Bits24)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::PointIDXOR24);
        m_data[m_position] = static_cast<uint8_t>(bitsChanged);
        m_data[m_position + 1] = static_cast<uint8_t>(bitsChanged >> 8);
        m_data[m_position + 2] = static_cast<uint8_t>(bitsChanged >> 16);
        m_position += 3;
    }
    else
    {
        m_lastPoint->WriteCode(TSSCCodeWords::PointIDXOR32);
        m_data[m_position] = static_cast<uint8_t>(bitsChanged);
        m_data[m_position + 1] = static_cast<uint8_t>(bitsChanged >> 8);
        m_data[m_position + 2] = static_cast<uint8_t>(bitsChanged >> 16);
        m_data[m_position + 3] = static_cast<uint8_t>(bitsChanged >> 24);
        m_position += 4;
    }

    m_lastPoint->PrevNextPointID1 = id;
}

void TSSCEncoder::WriteTimestampChange(const int64_t timestamp)
{
    if (m_prevTimestamp2 == timestamp)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::Timestamp2);
    }
    else if (m_prevTimestamp1 < timestamp)
    {
        if (m_prevTimestamp1 + m_prevTimeDelta1 == timestamp)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::TimeDelta1Forward);
        }
        else if (m_prevTimestamp1 + m_prevTimeDelta2 == timestamp)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::TimeDelta2Forward);
        }
        else if (m_prevTimestamp1 + m_prevTimeDelta3 == timestamp)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::TimeDelta3Forward);
        }
        else if (m_prevTimestamp1 + m_prevTimeDelta4 == timestamp)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::TimeDelta4Forward);
        }
        else
        {
            m_lastPoint->WriteCode(TSSCCodeWords::TimeXOR7Bit);
            Encode7BitUInt64(m_data, m_position, static_cast<uint64_t>(timestamp ^ m_prevTimestamp1));
        }
    }
    else
    {
        if (m_prevTimestamp1 - m_prevTimeDelta1 == timestamp)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::TimeDelta1Reverse);
        }
        else if (m_prevTimestamp1 - m_prevTimeDelta2 == timestamp)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::TimeDelta2Reverse);
        }
        else if (m_prevTimestamp1 - m_prevTimeDelta3 == timestamp)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::TimeDelta3Reverse);
        }
        else if (m_prevTimestamp1 - m_prevTimeDelta4 == timestamp)
        {
            m_lastPoint->WriteCode(TSSCCodeWords::TimeDelta4Reverse);
        }
        else
        {
            m_lastPoint->WriteCode(TSSCCodeWords::TimeXOR7Bit);
            Encode7BitUInt64(m_data, m_position, static_cast<uint64_t>(timestamp ^ m_prevTimestamp1));
        }
    }

    // Save the smallest delta time
    const int64_t minDelta = abs(m_prevTimestamp1 - timestamp);

    if (minDelta < m_prevTimeDelta4
        && minDelta != m_prevTimeDelta1
        && minDelta != m_prevTimeDelta2
        && minDelta != m_prevTimeDelta3)
    {
        if (minDelta < m_prevTimeDelta1)
        {
            m_prevTimeDelta4 = m_prevTimeDelta3;
            m_prevTimeDelta3 = m_prevTimeDelta2;
            m_prevTimeDelta2 = m_prevTimeDelta1;
            m_prevTimeDelta1 = minDelta;
        }
        else if (minDelta < m_prevTimeDelta2)
        {
            m_prevTimeDelta4 = m_prevTimeDelta3;
            m_prevTimeDelta3 = m_prevTimeDelta2;
            m_prevTimeDelta2 = minDelta;
        }
        else if (minDelta < m_prevTimeDelta3)
        {
            m_prevTimeDelta4 = m_prevTimeDelta3;
            m_prevTimeDelta3 = minDelta;
        }
        else
        {
            m_prevTimeDelta4 = minDelta;
        }
    }

    m_prevTimestamp2 = m_prevTimestamp1;
    m_prevTimestamp1 = timestamp;
}

void TSSCEncoder::WriteQualityChange(const uint32_t quality, const TSSCPointMetadataPtr& point)
{
    // Quality flags deviate very little, so the value is only encoded when it deviates
    // from previous value. When value is added to the stream, the value is compressed
    // using 7-bit variable-length quantity encoding. More specifically, the algorithm
    // writes out an integer 7-bits at a time with the high-bit of the current output
    // byte only being set when the next byte should also be read.
    // See https://en.wikipedia.org/wiki/Variable-length_quantity
    if (point->PrevQuality2 == quality)
    {
        m_lastPoint->WriteCode(TSSCCodeWords::Quality2);
    }
    else
    {
        m_lastPoint->WriteCode(TSSCCodeWords::Quality7Bit32);
        Encode7BitUInt32(m_data, m_position, quality);
    }
    
    point->PrevQuality2 = point->PrevQuality1;
    point->PrevQuality1 = quality;
}

void TSSCEncoder::ClearBitStream()
{
    m_bitStreamBufferIndex = -1;
    m_bitStreamCacheBitCount = 0;
    m_bitStreamCache = 0;
}

void TSSCEncoder::WriteBits(const int32_t code, const int32_t length)
{
    if (m_bitStreamBufferIndex < 0)
        m_bitStreamBufferIndex = static_cast<int32_t>(m_position++);

    m_bitStreamCache = m_bitStreamCache << length | code;
    m_bitStreamCacheBitCount += length;

    if (m_bitStreamCacheBitCount > 7)
        BitStreamEnd();
}

void TSSCEncoder::BitStreamFlush()
{
    if (m_bitStreamCacheBitCount > 0)
    {
        if (m_bitStreamBufferIndex < 0)
            m_bitStreamBufferIndex = static_cast<int32_t>(m_position++);

        m_lastPoint->WriteCode(TSSCCodeWords::EndOfStream);

        if (m_bitStreamCacheBitCount > 7)
            BitStreamEnd();

        if (m_bitStreamCacheBitCount > 0)
        {
            // Make up 8 bits by padding.
            m_bitStreamCache <<= 8 - m_bitStreamCacheBitCount;
            m_data[m_bitStreamBufferIndex] = static_cast<uint8_t>(m_bitStreamCache);
            m_bitStreamCache = 0;
            m_bitStreamBufferIndex = -1;
            m_bitStreamCacheBitCount = 0;
        }
    }
}

void TSSCEncoder::BitStreamEnd()
{
    while (m_bitStreamCacheBitCount > 7)
    {
        m_data[m_bitStreamBufferIndex] = static_cast<uint8_t>(m_bitStreamCache >> (m_bitStreamCacheBitCount - 8));
        m_bitStreamCacheBitCount -= 8;

        if (m_bitStreamCacheBitCount > 0)
            m_bitStreamBufferIndex = static_cast<int32_t>(m_position++);
        else
            m_bitStreamBufferIndex = -1;
    }
}

void Encode7BitUInt32(uint8_t* stream, uint32_t& position, const uint32_t value)
{
    if (value < 128UL)
    {
        stream[position] = static_cast<uint8_t>(value);
        ++position;
        return;
    }

    stream[position] = static_cast<uint8_t>(value | 128);

    if (value < 16384UL)
    {
        stream[position + 1] = static_cast<uint8_t>(value >> 7);
        position += 2;
        return;
    }

    stream[position + 1] = static_cast<uint8_t>(value >> 7 | 128);

    if (value < 2097152UL)
    {
        stream[position + 2] = static_cast<uint8_t>(value >> 14);
        position += 3;
        return;
    }

    stream[position + 2] = static_cast<uint8_t>(value >> 14 | 128);

    if (value < 268435456UL)
    {
        stream[position + 3] = static_cast<uint8_t>(value >> 21);
        position += 4;
        return;
    }

    stream[position + 3] = static_cast<uint8_t>(value >> 21 | 128);
    stream[position + 4] = static_cast<uint8_t>(value >> 28);

    position += 5;
}

void Encode7BitUInt64(uint8_t* stream, uint32_t& position, uint64_t value)
{
    if (value < 128UL)
    {
        stream[position] = static_cast<uint8_t>(value);
        ++position;
        return;
    }

    stream[position] = static_cast<uint8_t>(value | 128);
    
    if (value < 16384UL)
    {
        stream[position + 1] = static_cast<uint8_t>(value >> 7);
        position += 2;
        return;
    }
    
    stream[position + 1] = static_cast<uint8_t>(value >> 7 | 128);
    
    if (value < 2097152UL)
    {
        stream[position + 2] = static_cast<uint8_t>(value >> 14);
        position += 3;
        return;
    }
    
    stream[position + 2] = static_cast<uint8_t>(value >> 14 | 128);
    
    if (value < 268435456UL)
    {
        stream[position + 3] = static_cast<uint8_t>(value >> 21);
        position += 4;
        return;
    }
    
    stream[position + 3] = static_cast<uint8_t>(value >> 21 | 128);
    
    if (value < 34359738368UL)
    {
        stream[position + 4] = static_cast<uint8_t>(value >> 28);
        position += 5;
        return;
    }
    
    stream[position + 4] = static_cast<uint8_t>(value >> 28 | 128);
    
    if (value < 4398046511104UL)
    {
        stream[position + 5] = static_cast<uint8_t>(value >> 35);
        position += 6;
        return;
    }
    
    stream[position + 5] = static_cast<uint8_t>(value >> 35 | 128);
    
    if (value < 562949953421312UL)
    {
        stream[position + 6] = static_cast<uint8_t>(value >> 42);
        position += 7;
        return;
    }
    
    stream[position + 6] = static_cast<uint8_t>(value >> 42 | 128);
    
    if (value < 72057594037927936UL)
    {
        stream[position + 7] = static_cast<uint8_t>(value >> 49);
        position += 8;
        return;
    }
    
    stream[position + 7] = static_cast<uint8_t>(value >> 49 | 128);
    stream[position + 8] = static_cast<uint8_t>(value >> 56);
    
    position += 9;
}