//******************************************************************************************************
//  EndianConverter.h - Gbtc
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
//  03/19/2012 - Stephen C. Wills
//       Generated original version of source code.
//  01/26/2019 - J. Ritchie Carroll
//      Added static converters for buffers that use default instance.
//
//******************************************************************************************************

#pragma once

#include "CommonTypes.h"

namespace sttp
{
    // Converts values between the system's native byte
    // order and the big and little endian byte orders.
    class EndianConverter final
    {
    private:
        int m_nativeOrder;

        // Swaps the bytes in a character array.
        // Used for conversion between different byte orders.
        static void ByteSwap(uint8_t* value, uint32_t length);

    public:
        // Creates a new instance.
        EndianConverter();

        // Converts between big endian and
        // the system's native byte order.
        template<class T>
        T ConvertBigEndian(T value) const
        {
            if (m_nativeOrder != EndianConverter::BigEndian)
                ByteSwap(reinterpret_cast<uint8_t*>(&value), sizeof(T));

            return value;
        }

        // Converts between little endian and
        // the system's native byte order.
        template<class T>
        T ConvertLittleEndian(T value) const
        {
            if (m_nativeOrder != EndianConverter::LittleEndian)
                ByteSwap(reinterpret_cast<uint8_t*>(&value), sizeof(T));

            return value;
        }

        // Returns an integer value indicating which byte order is
        // the native byte order used by the system. The value is
        // defined by the BigEndian and LittleEndian class constants.
        int NativeOrder() const;

        // Class constant defining the big endian byte order.
        static constexpr int BigEndian = 0;

        // Class constant defining the little endian byte order.
        static constexpr int LittleEndian = 1;

        static EndianConverter Default;
        static bool IsLittleEndian();
        static bool IsBigEndian();

        template<class T>
        static T ToBigEndian(const uint8_t* buffer, const uint32_t startIndex)
        {
            return Default.ConvertBigEndian(*reinterpret_cast<T*>(const_cast<uint8_t*>(buffer + startIndex)));
        }

        template<class T>
        static T ToLittleEndian(const uint8_t* buffer, const uint32_t startIndex)
        {
            return Default.ConvertLittleEndian(*reinterpret_cast<T*>(const_cast<uint8_t*>(buffer + startIndex)));
        }

        template<class T>
        static uint32_t WriteBigEndianBytes(std::vector<uint8_t>& buffer, T value)
        {
            static const uint32_t length = sizeof(T);
            value = Default.ConvertBigEndian(value);
            const uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);

            WriteBytes(buffer, bytes, 0, length);

            return length;
        }

        template<class T>
        static uint32_t WriteLittleEndianBytes(std::vector<uint8_t>& buffer, T value)
        {
            static const uint32_t length = sizeof(T);
            value = Default.ConvertLittleEndian(value);
            const uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);

            WriteBytes(buffer, bytes, 0, length);

            return length;
        }
    };
}