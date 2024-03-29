//******************************************************************************************************
//  SignalIndexCache.cpp - Gbtc
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
//  02/07/2019 - J. Ritchie Carroll
//       Moved parse functionality into class, added generate functionality.
//
//******************************************************************************************************

#include "SignalIndexCache.h"
#include "DataPublisher.h"
#include "../Convert.h"
#include "../EndianConverter.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;
using namespace sttp::transport::tssc;

const SignalIndexCachePtr SignalIndexCache::NullPtr = nullptr;

SignalIndexCache::SignalIndexCache() :
    m_binaryLength(28),
    m_tsscDecoder(TSSCDecoder::NullPtr)
{
}

// Adds a measurement key to the cache.
void SignalIndexCache::AddMeasurementKey(const int32_t signalIndex, const Guid& signalID, const string& source, const uint64_t id, const uint32_t charSizeEstimate)
{
    m_reference.insert_or_assign(signalIndex, ConvertUInt32(m_signalIDList.size()));
    m_signalIDList.push_back(signalID);
    m_sourceList.push_back(source);
    m_idList.push_back(id);
    m_signalIDCache.insert_or_assign(signalID, signalIndex);

    // Char size here helps provide a rough-estimate on binary length used to reserve
    // bytes for a vector, if exact size is needed call RecalculateBinaryLength first
    m_binaryLength += ConvertUInt32(32 + source.size() * charSizeEstimate);
}

// Empties the cache.
void SignalIndexCache::Clear()
{
    m_reference.clear();
    m_signalIDList.clear();
    m_sourceList.clear();
    m_idList.clear();
    m_signalIDCache.clear();
}

// Determines whether an element with the given runtime ID exists in the signal index cache.
bool SignalIndexCache::Contains(const int32_t signalIndex) const
{
    return m_reference.contains(signalIndex);
}

// Gets the globally unique signal ID associated with the given 16-bit runtime ID.
Guid SignalIndexCache::GetSignalID(const int32_t signalIndex) const
{
    const auto result = m_reference.find(signalIndex);

    if (result != m_reference.end())
    {
        const uint32_t vectorIndex = result->second;
        return m_signalIDList[vectorIndex];
    }

    return Empty::Guid;
}

unordered_set<Guid> SignalIndexCache::GetSignalIDs() const
{
    return unordered_set<Guid>(m_signalIDList.begin(), m_signalIDList.end());
}

bool SignalIndexCache::GetSignalIDs(vector<Guid>& signalIDs) const
{
    signalIDs.clear();

    for (Guid signalID : m_signalIDList)
        signalIDs.emplace_back(signalID);

    return !signalIDs.empty();
}

// Gets the first half of the human-readable measurement
// key associated with the given 32-bit runtime ID.
const string& SignalIndexCache::GetSource(const int32_t signalIndex) const
{
    const auto result = m_reference.find(signalIndex);

    if (result != m_reference.end())
    {
        const uint32_t vectorIndex = result->second;
        return m_sourceList[vectorIndex];
    }

    return Empty::String;
}

// Gets the second half of the human-readable measurement
// key associated with the given 32-bit runtime ID.
uint64_t SignalIndexCache::GetID(const int32_t signalIndex) const
{
    const auto result = m_reference.find(signalIndex);

    if (result != m_reference.end())
    {
        const uint32_t vectorIndex = result->second;
        return m_idList[vectorIndex];
    }

    return UInt64::MaxValue;
}

// Gets the globally unique signal ID as well as the human-readable
// measurement key associated with the given 32-bit runtime ID.
bool SignalIndexCache::GetMeasurementKey(const int32_t signalIndex, Guid& signalID, string& source, uint64_t& id) const
{
    const auto result = m_reference.find(signalIndex);

    if (result != m_reference.end())
    {
        const uint32_t vectorIndex = result->second;

        signalID = m_signalIDList[vectorIndex];
        source = m_sourceList[vectorIndex];
        id = m_idList[vectorIndex];

        return true;
    }

    return false;
}

// Gets the 16-bit runtime ID associated with the given globally unique signal ID.
int32_t SignalIndexCache::GetSignalIndex(const Guid& signalID) const
{
    const auto result = m_signalIDCache.find(signalID);
    int32_t runtimeID = -1;

    if (result != m_signalIDCache.end())
        runtimeID = result->second;

    return runtimeID;
}

uint32_t SignalIndexCache::Count() const
{
    return ConvertUInt32(m_signalIDCache.size());
}

uint32_t SignalIndexCache::GetBinaryLength() const
{
    return m_binaryLength;
}

SignalIndexCachePtr SignalIndexCache::GetReference()
{
    return shared_from_this();
}

void SignalIndexCache::RecalculateBinaryLength(const SubscriberConnection& connection)
{
    uint32_t binaryLength = 28;

    for (size_t i = 0; i < m_signalIDList.size(); i++)
        binaryLength += ConvertUInt32(32 + connection.EncodeString(m_sourceList[i]).size()); //-V127

    m_binaryLength = binaryLength;
}

void SignalIndexCache::Decode(const vector<uint8_t>& buffer, Guid& subscriberID)
{
    const uint8_t* data = buffer.data();
    stringstream sourceStream;

    // Skip 4-byte length and parse subscriber ID
    subscriberID = ParseGuid(data + 4);

    const uint32_t* referenceCountPtr = reinterpret_cast<const uint32_t*>(data + 20);
    const uint32_t referenceCount = EndianConverter::Default.ConvertBigEndian(*referenceCountPtr);

    // Set up signalIndexPtr before entering the loop
    const int32_t* signalIndexPtr = reinterpret_cast<const int32_t*>(referenceCountPtr + 1);

    for (uint32_t i = 0; i < referenceCount; ++i)
    {
        // Begin setting up pointers
        const uint8_t* signalIDPtr = reinterpret_cast<const uint8_t*>(signalIndexPtr + 1);
        const uint32_t* sourceSizePtr = reinterpret_cast<const uint32_t*>(signalIDPtr + 16);

        // Get the source size now so we can use it to find the ID
        const uint32_t sourceSize = ConvertUInt32(EndianConverter::Default.ConvertBigEndian(*sourceSizePtr) / sizeof(char));

        // Continue setting up pointers
        const char* sourcePtr = reinterpret_cast<const char*>(sourceSizePtr + 1);
        const uint64_t* idPtr = reinterpret_cast<const uint64_t*>(sourcePtr + sourceSize);

        // Build string from binary data -- NOTE: this presumes subscriber code is always UTF8
        for (const char* sourceIter = sourcePtr; sourceIter < sourcePtr + sourceSize; ++sourceIter)
            sourceStream << *sourceIter;

        // Set values for measurement key
        const int32_t signalIndex = EndianConverter::Default.ConvertBigEndian(*signalIndexPtr);
        const Guid signalID = ParseGuid(signalIDPtr);
        const string source = sourceStream.str();
        const uint64_t id = EndianConverter::Default.ConvertBigEndian(*idPtr);

        // Add measurement key to the cache
        AddMeasurementKey(signalIndex, signalID, source, id);

        // Advance signalIndexPtr to the next signal
        // index and clear out the string stream
        signalIndexPtr = reinterpret_cast<const int32_t*>(idPtr + 1);
        sourceStream.str("");
    }

    // There is additional data here about unauthorized signal IDs
    // that may need to be parsed in the future...
}

void SignalIndexCache::Encode(const SubscriberConnection& connection, vector<uint8_t>& buffer) const
{
    const uint32_t binaryLengthLocation = ConvertUInt32(buffer.size());
    uint32_t binaryLength = 28; // cache size + subscriber ID + ref count + unauthorized count

    // Reserve space for binary byte length of cache
    WriteBytes(buffer, static_cast<uint32_t>(0));

    // Encode subscriber ID
    WriteBytes(buffer, connection.GetSubscriberID());

    // Encode number of references
    EndianConverter::WriteBigEndianBytes(buffer, ConvertInt32(m_reference.size()));

    for (size_t i = 0; i < m_signalIDList.size(); i++)
    {
        const Guid signalID = m_signalIDList[i];
    	const int32_t runtimeID = GetSignalIndex(signalID);

    	if (runtimeID == -1)
            continue;

        // Encode run-time signal index
        EndianConverter::WriteBigEndianBytes(buffer, runtimeID);

        // Encode signal ID
        WriteBytes(buffer, signalID);

        // Encode source
        vector<uint8_t> sourceBytes = connection.EncodeString(m_sourceList[i]);
        EndianConverter::WriteBigEndianBytes(buffer, ConvertInt32(sourceBytes.size()));
        WriteBytes(buffer, sourceBytes);

        // Encode ID
        EndianConverter::WriteBigEndianBytes(buffer, m_idList[i]);

        // Update binary length
        binaryLength += ConvertUInt32(32 + sourceBytes.size()); //-V127
    }

    // For now, not reporting unauthorized IDs, may need to add in the future
    EndianConverter::WriteBigEndianBytes(buffer, static_cast<uint32_t>(0));

    // Update binary byte length
    *reinterpret_cast<uint32_t*>(&buffer[binaryLengthLocation]) = EndianConverter::Default.ConvertBigEndian(binaryLength);
}
