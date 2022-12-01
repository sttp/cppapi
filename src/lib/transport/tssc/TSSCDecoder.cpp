//******************************************************************************************************
//  TSSCDecoder.cpp - Gbtc
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
#include "TSSCDecoder.h"
#include "../Constants.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;
using namespace sttp::transport::tssc;

uint32_t Decode7BitUInt32(const uint8_t* stream, uint32_t& position);
uint64_t Decode7BitUInt64(const uint8_t* stream, uint32_t& position);

const TSSCDecoderPtr TSSCDecoder::NullPtr = nullptr;

TSSCDecoder::TSSCDecoder() :
    m_data(nullptr),
    m_position(0),
    m_lastPosition(0),
    m_prevTimestamp1(0LL),
    m_prevTimestamp2(0LL),
    m_prevTimeDelta1(Int64::MaxValue),
    m_prevTimeDelta2(Int64::MaxValue),
    m_prevTimeDelta3(Int64::MaxValue),
    m_prevTimeDelta4(Int64::MaxValue),    
    m_bitStreamCount(0),
    m_bitStreamCache(0),
    m_sequenceNumber(0)
{
    m_lastPoint = NewTSSCPointMetadata();
}

void TSSCDecoder::Reset()
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

TSSCPointMetadataPtr TSSCDecoder::NewTSSCPointMetadata()
{
    return NewSharedPtr<TSSCPointMetadata>([this]() { return ReadBit(); }, [this]() { return ReadBits5(); });
}

void TSSCDecoder::SetBuffer(uint8_t* data, const uint32_t offset, const uint32_t length)
{
    ClearBitStream();
    m_data = data;
    m_position = offset;
    m_lastPosition = length;
}

bool TSSCDecoder::TryGetMeasurement(int32_t& id, int64_t& timestamp, uint32_t& quality, float32_t& value)
{
    if (m_position == m_lastPosition && BitStreamIsEmpty())
    {
        ClearBitStream();
        id = 0;
        timestamp = 0;
        quality = 0;
        value = 0.0F;
        return false;
    }

    // Given that the incoming pointID is not known in advance, the current
    // measurement will contain the encoding details for the next.

    // General compression strategy is to use delta-encoding for each
    // measurement component value that is received with the same identity.
    // See https://en.wikipedia.org/wiki/Delta_encoding

    // Delta-encoding sizes are embedded in the stream as type-specific
    // codes using as few bits as possible

    // Read next code for measurement ID decoding
    int32_t code = m_lastPoint->ReadCode();

    if (code == TSSCCodeWords::EndOfStream)
    {
        ClearBitStream();
        id = 0;
        timestamp = 0;
        quality = 0;
        value = 0.0F;
        return false;
    }

    // Decode measurement ID and read next code for timestamp decoding
    if (code <= TSSCCodeWords::PointIDXOR32)
    {
        DecodePointID(static_cast<uint8_t>(code), m_lastPoint);
        code = m_lastPoint->ReadCode();
        
        if (code < TSSCCodeWords::TimeDelta1Forward)
        {
            stringstream errorMessageStream;

            errorMessageStream << "Expecting code >= ";
            errorMessageStream << static_cast<int>(TSSCCodeWords::TimeDelta1Forward);
            errorMessageStream << " Received ";
            errorMessageStream << static_cast<int>(code);
            errorMessageStream << " at position ";
            errorMessageStream << static_cast<int>(m_position);
            errorMessageStream << " with last position ";
            errorMessageStream << static_cast<int>(m_lastPosition);

            throw SubscriberException(errorMessageStream.str());
        }
    }

    // Assign decoded measurement ID to out parameter
    id = m_lastPoint->PrevNextPointID1;

    // Setup tracking for metadata associated with measurement ID and next point to decode
    const int32_t pointCount = ConvertInt32(m_points.size());
    TSSCPointMetadataPtr nextPoint = id >= pointCount ? nullptr : m_points[id];
    
    if (nextPoint == nullptr)
    {
        nextPoint = NewTSSCPointMetadata();

        if (id >= pointCount)
            m_points.resize(static_cast<size_t>(id) + 1, nullptr);

        m_points[id] = nextPoint;
        
        nextPoint->PrevNextPointID1 = id + 1;
    }

    // Decode measurement timestamp and read next code for quality flags decoding
    if (code <= TSSCCodeWords::TimeXOR7Bit)
    {
        timestamp = DecodeTimestamp(static_cast<uint8_t>(code));
        code = m_lastPoint->ReadCode();

        if (code < TSSCCodeWords::Quality2)
        {
            stringstream errorMessageStream;

            errorMessageStream << "Expecting code >= ";
            errorMessageStream << static_cast<int>(TSSCCodeWords::Quality2);
            errorMessageStream << " Received ";
            errorMessageStream << static_cast<int>(code);
            errorMessageStream << " at position ";
            errorMessageStream << static_cast<int>(m_position);
            errorMessageStream << " with last position ";
            errorMessageStream << static_cast<int>(m_lastPosition);

            throw SubscriberException(errorMessageStream.str());
        }
    }
    else
    {
        timestamp = m_prevTimestamp1;
    }

    // Decode measurement quality flags and read next code for measurement value decoding
    if (code <= TSSCCodeWords::Quality7Bit32)
    {
        quality = DecodeQuality(static_cast<uint8_t>(code), nextPoint);
        code = m_lastPoint->ReadCode();
        
        if (code < TSSCCodeWords::Value1)
        {
            stringstream errorMessageStream;

            errorMessageStream << "Expecting code >= ";
            errorMessageStream << static_cast<int>(TSSCCodeWords::Value1);
            errorMessageStream << " Received ";
            errorMessageStream << static_cast<int>(code);
            errorMessageStream << " at position ";
            errorMessageStream << static_cast<int>(m_position);
            errorMessageStream << " with last position ";
            errorMessageStream << static_cast<int>(m_lastPosition);

            throw SubscriberException(errorMessageStream.str());
        }
    }
    else
    {
        quality = nextPoint->PrevQuality1;
    }

    // Since measurement value will almost always change, this is not put inside a function call.
    uint32_t valueRaw;

    // Decode measurement value
    if (code == TSSCCodeWords::Value1)
    {
        valueRaw = nextPoint->PrevValue1;
    }
    else if (code == TSSCCodeWords::Value2)
    {
        valueRaw = nextPoint->PrevValue2;
        nextPoint->PrevValue2 = nextPoint->PrevValue1;
        nextPoint->PrevValue1 = valueRaw;
    }
    else if (code == TSSCCodeWords::Value3)
    {
        valueRaw = nextPoint->PrevValue3;
        nextPoint->PrevValue3 = nextPoint->PrevValue2;
        nextPoint->PrevValue2 = nextPoint->PrevValue1;
        nextPoint->PrevValue1 = valueRaw;
    }
    else if (code == TSSCCodeWords::ValueZero)
    {
        valueRaw = 0;
        nextPoint->PrevValue3 = nextPoint->PrevValue2;
        nextPoint->PrevValue2 = nextPoint->PrevValue1;
        nextPoint->PrevValue1 = valueRaw;
    }
    else
    {
        switch (code)
        {
            case TSSCCodeWords::ValueXOR4:
                valueRaw = static_cast<uint32_t>(ReadBits4()) ^ nextPoint->PrevValue1;
                break;
            case TSSCCodeWords::ValueXOR8:
                valueRaw = static_cast<uint32_t>(m_data[m_position]) ^ nextPoint->PrevValue1;
                m_position++;
                break;
            case TSSCCodeWords::ValueXOR12:
                valueRaw = static_cast<uint32_t>(ReadBits4()) ^ static_cast<uint32_t>(m_data[m_position] << 4) ^ nextPoint->PrevValue1;
                m_position++;
                break;
            case TSSCCodeWords::ValueXOR16:
                valueRaw = static_cast<uint32_t>(m_data[m_position]) ^ static_cast<uint32_t>(m_data[m_position + 1] << 8) ^ nextPoint->PrevValue1;
                m_position += 2;
                break;
            case TSSCCodeWords::ValueXOR20:
                valueRaw = static_cast<uint32_t>(ReadBits4()) ^ static_cast<uint32_t>(m_data[m_position] << 4) ^ static_cast<uint32_t>(m_data[m_position + 1] << 12) ^ nextPoint->PrevValue1;
                m_position += 2;
                break;
            case TSSCCodeWords::ValueXOR24:
                valueRaw = static_cast<uint32_t>(m_data[m_position]) ^ static_cast<uint32_t>(m_data[m_position + 1] << 8) ^ static_cast<uint32_t>(m_data[m_position + 2] << 16) ^ nextPoint->PrevValue1;
                m_position += 3;
                break;
            case TSSCCodeWords::ValueXOR28:
                valueRaw = static_cast<uint32_t>(ReadBits4()) ^ static_cast<uint32_t>(m_data[m_position] << 4) ^ static_cast<uint32_t>(m_data[m_position + 1] << 12) ^ static_cast<uint32_t>(m_data[m_position + 2] << 20) ^ nextPoint->PrevValue1;
                m_position += 3;
                break;
            case TSSCCodeWords::ValueXOR32:
                valueRaw = static_cast<uint32_t>(m_data[m_position]) ^ static_cast<uint32_t>(m_data[m_position + 1] << 8) ^ static_cast<uint32_t>(m_data[m_position + 2] << 16) ^ static_cast<uint32_t>(m_data[m_position + 3] << 24) ^ nextPoint->PrevValue1;
                m_position += 4;
                break;
            default:
                stringstream errorMessageStream;

                errorMessageStream << "Invalid code received ";
                errorMessageStream << static_cast<int>(code);
                errorMessageStream << " at position ";
                errorMessageStream << static_cast<int>(m_position);
                errorMessageStream << " with last position ";
                errorMessageStream << static_cast<int>(m_lastPosition);

                throw SubscriberException(errorMessageStream.str());
        }

        nextPoint->PrevValue3 = nextPoint->PrevValue2;
        nextPoint->PrevValue2 = nextPoint->PrevValue1;
        nextPoint->PrevValue1 = valueRaw;
    }

    // Assign decoded measurement value to out parameter
    value = *reinterpret_cast<float32_t*>(&valueRaw);
    m_lastPoint = nextPoint;

    return true;
}

void TSSCDecoder::DecodePointID(const uint8_t code, const TSSCPointMetadataPtr& lastPoint)
{
    switch (code)
    {
        case TSSCCodeWords::PointIDXOR4:
            lastPoint->PrevNextPointID1 = ReadBits4() ^ lastPoint->PrevNextPointID1;
            break;
        case TSSCCodeWords::PointIDXOR8:
            lastPoint->PrevNextPointID1 = m_data[m_position] ^ lastPoint->PrevNextPointID1;
            m_position += 1;
            break;
        case TSSCCodeWords::PointIDXOR12:
            lastPoint->PrevNextPointID1 = ReadBits4() ^ m_data[m_position] << 4 ^ lastPoint->PrevNextPointID1;
            m_position += 1;
            break;
        case TSSCCodeWords::PointIDXOR16:
            lastPoint->PrevNextPointID1 = m_data[m_position] ^ m_data[m_position + 1] << 8 ^ lastPoint->PrevNextPointID1;
            m_position += 2;
            break;
        case TSSCCodeWords::PointIDXOR20:
            lastPoint->PrevNextPointID1 = ReadBits4() ^ m_data[m_position] << 4 ^ m_data[m_position + 1] << 12 ^ lastPoint->PrevNextPointID1;
            m_position += 2;
            break;
        case TSSCCodeWords::PointIDXOR24:
            lastPoint->PrevNextPointID1 = m_data[m_position] ^ m_data[m_position + 1] << 8 ^ m_data[m_position + 2] << 16 ^ lastPoint->PrevNextPointID1;
            m_position += 3;
            break;
        case TSSCCodeWords::PointIDXOR32:
            lastPoint->PrevNextPointID1 = m_data[m_position] ^ m_data[m_position + 1] << 8 ^ m_data[m_position + 2] << 16 ^ m_data[m_position + 3] << 24 ^ lastPoint->PrevNextPointID1;
            m_position += 4;
            break;
        default:
            stringstream errorMessageStream;

            errorMessageStream << "Invalid code received ";
            errorMessageStream << static_cast<int>(code);
            errorMessageStream << " at position ";
            errorMessageStream << static_cast<int>(m_position);
            errorMessageStream << " with last position ";
            errorMessageStream << static_cast<int>(m_lastPosition);

            throw SubscriberException(errorMessageStream.str());
    }
}

int64_t TSSCDecoder::DecodeTimestamp(const uint8_t code)
{
    int64_t timestamp;

    switch (code)
    {
        case TSSCCodeWords::TimeDelta1Forward:
            timestamp = m_prevTimestamp1 + m_prevTimeDelta1;
            break;
        case TSSCCodeWords::TimeDelta2Forward:
            timestamp = m_prevTimestamp1 + m_prevTimeDelta2;
            break;
        case TSSCCodeWords::TimeDelta3Forward:
            timestamp = m_prevTimestamp1 + m_prevTimeDelta3;
            break;
        case TSSCCodeWords::TimeDelta4Forward:
            timestamp = m_prevTimestamp1 + m_prevTimeDelta4;
            break;
        case TSSCCodeWords::TimeDelta1Reverse:
            timestamp = m_prevTimestamp1 - m_prevTimeDelta1;
            break;
        case TSSCCodeWords::TimeDelta2Reverse:
            timestamp = m_prevTimestamp1 - m_prevTimeDelta2;
            break;
        case TSSCCodeWords::TimeDelta3Reverse:
            timestamp = m_prevTimestamp1 - m_prevTimeDelta3;
            break;
        case TSSCCodeWords::TimeDelta4Reverse:
            timestamp = m_prevTimestamp1 - m_prevTimeDelta4;
            break;
        case TSSCCodeWords::Timestamp2:
            timestamp = m_prevTimestamp2;
            break;
        default:
            timestamp = m_prevTimestamp1 ^ static_cast<int64_t>(Decode7BitUInt64(&m_data[0], m_position));
            break;
    }

    // Save the smallest delta time
    const int64_t minDelta = abs(m_prevTimestamp1 - timestamp);

    if (minDelta < m_prevTimeDelta4 && minDelta != m_prevTimeDelta1 && minDelta != m_prevTimeDelta2 && minDelta != m_prevTimeDelta3)
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

    return timestamp;
}

uint32_t TSSCDecoder::DecodeQuality(const uint8_t code, const TSSCPointMetadataPtr& nextPoint)
{
    uint32_t quality;

    if (code == TSSCCodeWords::Quality2)
    {
        quality = nextPoint->PrevQuality2;
    }
    else
    {
        quality = Decode7BitUInt32(&m_data[0], m_position);
    }

    nextPoint->PrevQuality2 = nextPoint->PrevQuality1;
    nextPoint->PrevQuality1 = quality;

    return quality;
}

bool TSSCDecoder::BitStreamIsEmpty() const
{
    return m_bitStreamCount == 0;
}

void TSSCDecoder::ClearBitStream()
{
    m_bitStreamCount = 0;
    m_bitStreamCache = 0;
}

int32_t TSSCDecoder::ReadBit()
{
    if (m_bitStreamCount == 0)
    {
        m_bitStreamCount = 8;
        m_bitStreamCache = static_cast<int32_t>(m_data[m_position++]);
    }

    m_bitStreamCount--;
    
    return m_bitStreamCache >> m_bitStreamCount & 1;
}

int32_t TSSCDecoder::ReadBits4()
{
    return ReadBit() << 3 | ReadBit() << 2 | ReadBit() << 1 | ReadBit();
}

int32_t TSSCDecoder::ReadBits5()
{
    return ReadBit() << 4 | ReadBit() << 3 | ReadBit() << 2 | ReadBit() << 1 | ReadBit();
}

uint16_t TSSCDecoder::GetSequenceNumber() const
{
    return m_sequenceNumber;
}

void TSSCDecoder::ResetSequenceNumber()
{
    m_sequenceNumber = 0;
}

void TSSCDecoder::IncrementSequenceNumber()
{
    m_sequenceNumber++;

    // Do not increment to 0 on roll-over
    if (m_sequenceNumber == 0)
        m_sequenceNumber = 1;
}

uint32_t Decode7BitUInt32(const uint8_t* stream, uint32_t& position)
{
    stream += position;    
    uint32_t value = *stream;
    
    if (value < 128)
    {
        position++;
        return value;
    }
    
    value ^= static_cast<uint32_t>(stream[1]) << 7;
    
    if (value < 16384)
    {
        position += 2;
        return value ^ 0x80;
    }
    
    value ^= static_cast<uint32_t>(stream[2]) << 14;
    
    if (value < 2097152)
    {
        position += 3;
        return value ^ 0x4080;
    }
    
    value ^= static_cast<uint32_t>(stream[3]) << 21;
    
    if (value < 268435456)
    {
        position += 4;
        return value ^ 0x204080;
    }
    
    value ^= static_cast<uint32_t>(stream[4])  << 28;
    position += 5;
    
    return value ^ 0x10204080;
}

uint64_t Decode7BitUInt64(const uint8_t* stream, uint32_t& position)
{
    stream += position;
    uint64_t value = *stream;

    if (value < 128UL)
    {
        ++position;
        return value;
    }
    
    value ^= static_cast<uint64_t>(stream[1]) << 7;
    
    if (value < 16384UL)
    {
        position += 2;
        return value ^ 0x80UL;
    }
    
    value ^= static_cast<uint64_t>(stream[2]) << 14;
    
    if (value < 2097152UL)
    {
        position += 3;
        return value ^ 0x4080UL;
    }
    
    value ^= static_cast<uint64_t>(stream[3]) << 21;
    
    if (value < 268435456UL)
    {
        position += 4;
        return value ^ 0x204080UL;
    }
    
    value ^= static_cast<uint64_t>(stream[4]) << 28;
    
    if (value < 34359738368UL)
    {
        position += 5;
        return value ^ 0x10204080UL;
    }
    
    value ^= static_cast<uint64_t>(stream[5]) << 35;
    
    if (value < 4398046511104UL)
    {
        position += 6;
        return value ^ 0x810204080UL;
    }
    
    value ^= static_cast<uint64_t>(stream[6]) << 42;
    
    if (value < 562949953421312UL)
    {
        position += 7;
        return value ^ 0x40810204080UL;
    }
    
    value ^= static_cast<uint64_t>(stream[7]) << 49;
    
    if (value < 72057594037927936UL)
    {
        position += 8;
        return value ^ 0x2040810204080UL;
    }
    
    value ^= static_cast<uint64_t>(stream[8]) << 56;    
    position += 9;

    return value ^ 0x102040810204080UL;
}