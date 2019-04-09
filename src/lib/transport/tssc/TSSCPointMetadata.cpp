//******************************************************************************************************
//  TSSCPointMetadata.cpp - Gbtc
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
//  04/09/2019 - J. Ritchie Carroll
//       Migrated code to C++.
//
//******************************************************************************************************

#include "TSSCPointMetadata.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;
using namespace sttp::transport::tssc;

// Define "instances" of all static constants so values can be passed by reference and found by linker
const uint8_t TSSCCodeWords::EndOfStream;
const uint8_t TSSCCodeWords::PointIDXOR4;
const uint8_t TSSCCodeWords::PointIDXOR8;
const uint8_t TSSCCodeWords::PointIDXOR12;
const uint8_t TSSCCodeWords::PointIDXOR16;
const uint8_t TSSCCodeWords::TimeDelta1Forward;
const uint8_t TSSCCodeWords::TimeDelta2Forward;
const uint8_t TSSCCodeWords::TimeDelta3Forward;
const uint8_t TSSCCodeWords::TimeDelta4Forward;
const uint8_t TSSCCodeWords::TimeDelta1Reverse;
const uint8_t TSSCCodeWords::TimeDelta2Reverse;
const uint8_t TSSCCodeWords::TimeDelta3Reverse;
const uint8_t TSSCCodeWords::TimeDelta4Reverse;
const uint8_t TSSCCodeWords::Timestamp2;
const uint8_t TSSCCodeWords::TimeXOR7Bit;
const uint8_t TSSCCodeWords::Quality2;
const uint8_t TSSCCodeWords::Quality7Bit32;
const uint8_t TSSCCodeWords::Value1;
const uint8_t TSSCCodeWords::Value2;
const uint8_t TSSCCodeWords::Value3;
const uint8_t TSSCCodeWords::ValueZero;
const uint8_t TSSCCodeWords::ValueXOR4;
const uint8_t TSSCCodeWords::ValueXOR8;
const uint8_t TSSCCodeWords::ValueXOR12;
const uint8_t TSSCCodeWords::ValueXOR16;
const uint8_t TSSCCodeWords::ValueXOR20;
const uint8_t TSSCCodeWords::ValueXOR24;
const uint8_t TSSCCodeWords::ValueXOR28;
const uint8_t TSSCCodeWords::ValueXOR32;

TSSCPointMetadata::TSSCPointMetadata(function<void(int32_t, int32_t)> writeBits) : //-V730
    TSSCPointMetadata::TSSCPointMetadata(
        std::move(writeBits),
        function<int32_t()>(nullptr),
        function<int32_t()>(nullptr))
{
}

TSSCPointMetadata::TSSCPointMetadata(function<int32_t()> readBit, function<int32_t()> readBits5) : //-V730
    TSSCPointMetadata::TSSCPointMetadata(
        function<void(int32_t, int32_t)>(nullptr),
        std::move(readBit),
        std::move(readBits5))
{
}

TSSCPointMetadata::TSSCPointMetadata(
    function<void(int32_t, int32_t)> writeBits,
    function<int32_t()> readBit,
    function<int32_t()> readBits5) :
    m_commandsSentSinceLastChange(0),
    m_mode(4),
    m_mode21(0),
    m_mode31(0),
    m_mode301(0),
    m_mode41(TSSCCodeWords::Value1),
    m_mode401(TSSCCodeWords::Value2),
    m_mode4001(TSSCCodeWords::Value3),
    m_startupMode(0),
    m_writeBits(std::move(writeBits)),
    m_readBit(std::move(readBit)),
    m_readBits5(std::move(readBits5)),
    PrevNextPointId1(0),
    PrevQuality1(0),
    PrevQuality2(0),
    PrevValue1(0),
    PrevValue2(0),
    PrevValue3(0)
{
    for (uint8_t i = 0; i < CommandStatsLength; i++)
        m_commandStats[i] = 0;
}

void TSSCPointMetadata::WriteCode(int32_t code)
{
    switch (m_mode)
    {
        case 1:
            m_writeBits(code, 5);
            break;
        case 2:
            if (code == m_mode21)
                m_writeBits(1, 1);
            else
                m_writeBits(code, 6);
            break;
        case 3:
            if (code == m_mode31)
                m_writeBits(1, 1);
            else if (code == m_mode301)
                m_writeBits(1, 2);
            else
                m_writeBits(code, 7);
            break;
        case 4:
            if (code == m_mode41)
                m_writeBits(1, 1);
            else if (code == m_mode401)
                m_writeBits(1, 2);
            else if (code == m_mode4001)
                m_writeBits(1, 3);
            else
                m_writeBits(code, 8);
            break;
        default:
            throw PublisherException("Coding Error");
    }

    UpdatedCodeStatistics(code);
}

int32_t TSSCPointMetadata::ReadCode()
{
    int32_t code;

    switch (m_mode)
    {
        case 1:
            code = m_readBits5();
            break;
        case 2:
            if (m_readBit() == 1)
                code = m_mode21;
            else
                code = m_readBits5();
            break;
        case 3:
            if (m_readBit() == 1)
                code = m_mode31;
            else if (m_readBit() == 1)
                code = m_mode301;
            else
                code = m_readBits5();
            break;
        case 4:
            if (m_readBit() == 1)
                code = m_mode41;
            else if (m_readBit() == 1)
                code = m_mode401;
            else if (m_readBit() == 1)
                code = m_mode4001;
            else
                code = m_readBits5();
            break;
        default:
            throw SubscriberException("Unsupported compression mode");
    }

    UpdatedCodeStatistics(code);
    return code;
}

void TSSCPointMetadata::UpdatedCodeStatistics(int32_t code)
{
    m_commandsSentSinceLastChange++;
    m_commandStats[code]++;

    if (m_startupMode == 0 && m_commandsSentSinceLastChange > 5)
    {
        m_startupMode++;
        AdaptCommands();
    }
    else if (m_startupMode == 1 && m_commandsSentSinceLastChange > 20)
    {
        m_startupMode++;
        AdaptCommands();
    }
    else if (m_startupMode == 2 && m_commandsSentSinceLastChange > 100)
    {
        AdaptCommands();
    }
}

void TSSCPointMetadata::AdaptCommands()
{
    uint8_t code1 = 0;
    int32_t count1 = 0;

    uint8_t code2 = 1;
    int32_t count2 = 0;

    uint8_t code3 = 2;
    int32_t count3 = 0;

    int32_t total = 0;

    for (int32_t i = 0; i < CommandStatsLength; i++)
    {
        const int32_t count = m_commandStats[i];
        m_commandStats[i] = 0;

        total += count;

        if (count > count3)
        {
            if (count > count1)
            {
                code3 = code2;
                count3 = count2;

                code2 = code1;
                count2 = count1;

                code1 = static_cast<uint8_t>(i);
                count1 = count;
            }
            else if (count > count2)
            {
                code3 = code2;
                count3 = count2;

                code2 = static_cast<uint8_t>(i);
                count2 = count;
            }
            else
            {
                code3 = static_cast<uint8_t>(i);
                count3 = count;
            }
        }
    }

    const int32_t mode1Size = total * 5;
    const int32_t mode2Size = count1 * 1 + (total - count1) * 6;
    const int32_t mode3Size = count1 * 1 + count2 * 2 + (total - count1 - count2) * 7;
    const int32_t mode4Size = count1 * 1 + count2 * 2 + count3 * 3 + (total - count1 - count2 - count3) * 8;

    int32_t minSize = Int32::MaxValue;

    minSize = min(minSize, mode1Size);
    minSize = min(minSize, mode2Size);
    minSize = min(minSize, mode3Size);
    minSize = min(minSize, mode4Size);

    if (minSize == mode1Size)
    {
        m_mode = 1;
    }
    else if (minSize == mode2Size)
    {
        m_mode = 2;
        m_mode21 = code1;
    }
    else if (minSize == mode3Size)
    {
        m_mode = 3;
        m_mode31 = code1;
        m_mode301 = code2;
    }
    else if (minSize == mode4Size)
    {
        m_mode = 4;
        m_mode41 = code1;
        m_mode401 = code2;
        m_mode4001 = code3;
    }
    else
    {
        if (m_writeBits == nullptr)
            throw SubscriberException("Coding Error");

        throw PublisherException("Coding Error");
    }

    m_commandsSentSinceLastChange = 0;
}