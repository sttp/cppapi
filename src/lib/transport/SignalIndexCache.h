//******************************************************************************************************
//  SignalIndexCache.h - Gbtc
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
//  03/08/2012 - Stephen C. Wills
//       Generated original version of source code.
//  02/07/2019 - J. Ritchie Carroll
//       Moved parse functionality into class, added generate functionality.
//
//******************************************************************************************************

#pragma once

#include "../CommonTypes.h"
#include "tssc/TSSCDecoder.h"
#include <unordered_set>

namespace sttp::transport
{
    class SubscriberConnection;
    typedef sttp::SharedPtr<SubscriberConnection> SubscriberConnectionPtr;

    class SignalIndexCache;
    typedef sttp::SharedPtr<SignalIndexCache> SignalIndexCachePtr;

    class DataSubscriber;

    // Maps 16-bit runtime IDs to 128-bit globally unique IDs.
    // Additionally provides reverse lookup and an extra mapping
    // to human-readable measurement keys.
    class SignalIndexCache final : public sttp::EnableSharedThisPtr<SignalIndexCache> // NOLINT
    {
    private:
        std::unordered_map<int32_t, uint32_t> m_reference;
        std::vector<sttp::Guid> m_signalIDList;
        std::vector<std::string> m_sourceList;
        std::vector<uint64_t> m_idList;
        std::unordered_map<sttp::Guid, int32_t> m_signalIDCache;
        uint32_t m_binaryLength;
        tssc::TSSCDecoderPtr m_tsscDecoder;

    public:
        SignalIndexCache();

        // Adds a measurement key to the cache.
        void AddMeasurementKey(int32_t signalIndex, const sttp::Guid& signalID, const std::string& source, uint64_t id, uint32_t charSizeEstimate = 1U);

        // Empties the cache.
        void Clear();

        // Determines whether an element with the given runtime ID exists in the signal index cache.
        bool Contains(int32_t signalIndex) const;

        // Gets the globally unique signal ID associated with the given 16-bit runtime ID.
        sttp::Guid GetSignalID(int32_t signalIndex) const;

        // Gets the full list of signal IDs as an unordered set
        std::unordered_set<sttp::Guid> GetSignalIDs() const;

        // Populates given list with a copy of signal IDs
        bool GetSignalIDs(std::vector<sttp::Guid>& signalIDs) const;

        // Gets the first half of the human-readable measurement
        // key associated with the given 16-bit runtime ID.
        const std::string& GetSource(int32_t signalIndex) const;

        // Gets the second half of the human-readable measurement
        // key associated with the given 16-bit runtime ID.
        uint64_t GetID(int32_t signalIndex) const;

        // Gets the globally unique signal ID as well as the human-readable
        // measurement key associated with the given 16-bit runtime ID.
        bool GetMeasurementKey(int32_t signalIndex, sttp::Guid& signalID, std::string& source, uint64_t& id) const;

        // Gets the 16-bit runtime ID associated with the given globally unique signal ID.
        int32_t GetSignalIndex(const sttp::Guid& signalID) const;

        // Gets the mapped signal count
        uint32_t Count() const;

        // Gets an estimated binary size of a serialized signal index cache useful for pre-allocating
        // a vector size, for an exact size call RecalculateBinaryLength first
        uint32_t GetBinaryLength() const;

        SignalIndexCachePtr GetReference();

        void RecalculateBinaryLength(const SubscriberConnection& connection);

        void Decode(const std::vector<uint8_t>& buffer, Guid& subscriberID);

        void Encode(const SubscriberConnection& connection, std::vector<uint8_t>& buffer) const;

        static const SignalIndexCachePtr NullPtr;

        friend class DataSubscriber;
    };
}

// Setup standard hash code for SignalIndexCachePtr
template<>
struct std::hash<sttp::transport::SignalIndexCachePtr>
{
    size_t operator () (const sttp::transport::SignalIndexCachePtr& signalIndexCache) const noexcept
    {
        return boost::hash<sttp::transport::SignalIndexCachePtr>()(signalIndexCache);
    }
};
