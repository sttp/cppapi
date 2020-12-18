//******************************************************************************************************
//  TSSCPointMetadata.h - Gbtc
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

#pragma once

#include "../TransportTypes.h"

namespace sttp {
namespace transport {
namespace tssc
{
    // The encoding commands supported by TSSC
    struct TSSCCodeWords
    {
        static const uint8_t EndOfStream = 0;

        static const uint8_t PointIDXOR4 = 1;
        static const uint8_t PointIDXOR8 = 2;
        static const uint8_t PointIDXOR12 = 3;
        static const uint8_t PointIDXOR16 = 4;
        static const uint8_t PointIDXOR20 = 5;
        static const uint8_t PointIDXOR24 = 6;
        static const uint8_t PointIDXOR32 = 7;

        static const uint8_t TimeDelta1Forward = 8;
        static const uint8_t TimeDelta2Forward = 9;
        static const uint8_t TimeDelta3Forward = 10;
        static const uint8_t TimeDelta4Forward = 11;
        static const uint8_t TimeDelta1Reverse = 12;
        static const uint8_t TimeDelta2Reverse = 13;
        static const uint8_t TimeDelta3Reverse = 14;
        static const uint8_t TimeDelta4Reverse = 15;
        static const uint8_t Timestamp2 = 16;
        static const uint8_t TimeXOR7Bit = 17;

        static const uint8_t Quality2 = 18;
        static const uint8_t Quality7Bit32 = 19;

        static const uint8_t Value1 = 20;
        static const uint8_t Value2 = 21;
        static const uint8_t Value3 = 22;
        static const uint8_t ValueZero = 23;
        static const uint8_t ValueXOR4 = 24;
        static const uint8_t ValueXOR8 = 25;
        static const uint8_t ValueXOR12 = 26;
        static const uint8_t ValueXOR16 = 27;
        static const uint8_t ValueXOR20 = 28;
        static const uint8_t ValueXOR24 = 29;
        static const uint8_t ValueXOR28 = 30;
        static const uint8_t ValueXOR32 = 31;
    };

    // The metadata kept for each pointID.
    class TSSCPointMetadata
    {
    private:
        static const uint8_t CommandStatsLength = 32;

        uint8_t m_commandStats[CommandStatsLength];
        int32_t m_commandsSentSinceLastChange;

        //Bit codes for the 4 modes of encoding. 
        uint8_t m_mode;

        //(Mode 1 means no prefix.)
        uint8_t m_mode21;

        uint8_t m_mode31;
        uint8_t m_mode301;

        uint8_t m_mode41;
        uint8_t m_mode401;
        uint8_t m_mode4001;

        int32_t m_startupMode;

        std::function<void(int32_t, int32_t)> m_writeBits;
        std::function<int32_t()> m_readBit;
        std::function<int32_t()> m_readBits5;

        void UpdatedCodeStatistics(int32_t code);
        void AdaptCommands();

        TSSCPointMetadata(
            std::function<void(int32_t, int32_t)> writeBits,
            std::function<int32_t()> readBit,
            std::function<int32_t()> readBits5
        );

    public:
        TSSCPointMetadata(
            std::function<void(int32_t, int32_t)> writeBits
        );

        TSSCPointMetadata(
            std::function<int32_t()> readBit,
            std::function<int32_t()> readBits5
        );

        int32_t PrevNextPointID1;

        uint32_t PrevQuality1;
        uint32_t PrevQuality2;
        uint32_t PrevValue1;
        uint32_t PrevValue2;
        uint32_t PrevValue3;

        void WriteCode(int32_t code);

        int32_t ReadCode();
    };

    typedef SharedPtr<TSSCPointMetadata> TSSCPointMetadataPtr;
}}}