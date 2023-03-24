//******************************************************************************************************
//  DataSubscriber.cpp - Gbtc
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
//  03/26/2012 - Stephen C. Wills
//       Generated original version of source code.
//  03/22/2018 - J. Ritchie Carroll
//         Updated DataSubscriber callback function signatures to always include instance reference.
//
//******************************************************************************************************

#include "DataSubscriber.h"
#include "Constants.h"
#include "CompactMeasurement.h"
#include "../Convert.h"
#include "../EndianConverter.h"
#include "../Version.h"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace sttp;
using namespace sttp::transport;
using namespace sttp::transport::tssc;

constexpr float32_t MissingCacheWarningInterval = 20.0;
const DataSubscriberPtr DataSubscriber::NullPtr = nullptr;

DataSubscriber::DataSubscriber() :
    m_subscriberID(Empty::Guid),
    m_compressPayloadData(true),
    m_compressMetadata(true),
    m_compressSignalIndexCache(true),
    m_version(2),
    m_connected(false),
    m_listening(false),
    m_validated(false),
    m_subscribed(false),
    m_disconnecting(false),
    m_disconnected(false),
    m_userData(nullptr),
    m_totalCommandChannelBytesReceived(0UL),
    m_totalDataChannelBytesReceived(0UL),
    m_totalMeasurementsReceived(0UL),
    m_assemblySource(STTP_TITLE),
    m_assemblyVersion(STTP_VERSION),
    m_assemblyUpdatedOn(STTP_UPDATEDON),
    m_signalIndexCache{ SignalIndexCache::NullPtr, SignalIndexCache::NullPtr },
    m_cacheIndex(0),
    m_timeIndex(0),
    m_baseTimeOffsets{ 0, 0 },
    m_lastMissingCacheWarning(DateTime::MinValue),
    m_tsscResetRequested(false),
    m_tsscLastOOSReport(DateTime::MinValue),
    m_commandChannelSocket(m_commandChannelService),
    m_clientAcceptor(m_commandChannelService),
    m_readBuffer(Common::MaxPacketSize),
    m_writeBuffer(Common::MaxPacketSize),
    m_dataChannelSocket(m_dataChannelService)
{
}

// Destructor calls disconnect to clean up after itself.
DataSubscriber::~DataSubscriber() noexcept
{
    try
    {
        m_disposing = true;
        m_connector.Cancel();
        Disconnect(true, false, true);

        // Allow a moment for connection terminated event to complete
        ThreadSleep(10);
    }
    catch (...)
    {
        // ReSharper disable once CppRedundantControlFlowJump
        return;
    }
}

DataSubscriber::CallbackDispatcher::CallbackDispatcher() :
    Source(nullptr),
    Data(nullptr),
    Function(nullptr)
{
}

// All callbacks are run from the callback thread from here.
void DataSubscriber::RunCallbackThread()
{
    while (true)
    {
        m_callbackQueue.WaitForData();

        if (IsDisconnecting())
            break;

        const CallbackDispatcher dispatcher = m_callbackQueue.Dequeue();

        if (dispatcher.Function != nullptr)
            dispatcher.Function(dispatcher.Source, *dispatcher.Data);
    }
}

// All responses received from the server are handled by this thread with the
// exception of data packets which may or may not be handled by this thread.
void DataSubscriber::RunCommandChannelResponseThread()
{
    async_read(m_commandChannelSocket, buffer(m_readBuffer, Common::PayloadHeaderSize), [this]<typename T0, typename T1>(T0 && error, T1 && bytesTransferred)
    {
        ReadPayloadHeader(error, bytesTransferred);
    });

    m_commandChannelService.run();
}

// Callback for async read of the payload header.
void DataSubscriber::ReadPayloadHeader(const ErrorCode& error, const size_t bytesTransferred)
{
    if (IsDisconnecting())
        return;

    if (error == error::connection_aborted || error == error::connection_reset || error == error::eof)
    {
        HandleSocketError();
        return;
    }

    if (error)
    {
        DispatchErrorMessage("Error reading data from command channel: " + string(SystemError(error).what()));
        return;
    }

    // Gather statistics
    m_totalCommandChannelBytesReceived += Common::PayloadHeaderSize;

    const uint32_t packetSize = EndianConverter::ToBigEndian<uint32_t>(m_readBuffer.data(), 0);

    if (!m_validated)
    {
        if (m_version > 2)
        {
            // We need to check for a valid initial payload header size before attempting to resize
            // the payload buffer, especially when subscriber may be in listening mode. The very first
            // response received from the publisher should be the succeeded or failed response command
            // for the DefineOperationalModes command sent by the subscriber. The packet payload size
            // for this response, succeed or fail, will be a short message. Longer message sizes would
            // be considered suspect data, likely from a non-STTP based client connection. In context
            // of this initial response message, anything larger than 8KB of payload is considered
            // suspect and will be evaluated as a non-STTP type response.
            constexpr uint32_t MaxInitialPacketSize = Common::ResponseHeaderSize + 8192;

            if (packetSize > MaxInitialPacketSize)
            {
                DispatchErrorMessage("Possible invalid protocol detected from \"" + m_connectionID + "\": encountered request for " + ToString(packetSize) + " byte initial packet size -- connection likely from non-STTP client, disconnecting.");
                auto _ = Thread([this] { Disconnect(false, false, false); });
                return;
            }
        }
        else
        {
            // Older versions of STTP did not provide a response to define operational modes - in this
            // case common first response was for metadata refresh, which may be larger than 8KB
            m_validated = true;
        }
    }

    if (packetSize > ConvertUInt32(m_readBuffer.size()))
        m_readBuffer.resize(packetSize);

    // Read packet (payload body)
    // This read method is guaranteed not to return until the
    // requested size has been read or an error has occurred.
    async_read(m_commandChannelSocket, buffer(m_readBuffer, packetSize), [this]<typename T0, typename T1>(T0 && error, T1 && bytesTransferred) // NOLINT
    {
        ReadPacket(error, bytesTransferred);
    });
}

// Callback for async read of packets.
void DataSubscriber::ReadPacket(const ErrorCode& error, const size_t bytesTransferred)
{
    if (IsDisconnecting())
        return;

    if (error == error::connection_aborted || error == error::connection_reset || error == error::eof)
    {
        HandleSocketError();
        return;
    }

    if (error)
    {
        DispatchErrorMessage("Error reading data from command channel: " + string(SystemError(error).what()));
        return;
    }

    // Gather statistics
    m_totalCommandChannelBytesReceived += bytesTransferred;

    // Process response
    ProcessServerResponse(m_readBuffer.data(), 0, ConvertUInt32(bytesTransferred));

    // Read next payload header
    async_read(m_commandChannelSocket, buffer(m_readBuffer, Common::PayloadHeaderSize), [this]<typename T0, typename T1>(T0 && error, T1 && bytesTransferred) // NOLINT
    {
        ReadPayloadHeader(error, bytesTransferred);
    });
}

// If the user defines a separate UDP channel for their
// subscription, data packets get handled from this thread.
void DataSubscriber::RunDataChannelResponseThread()
{
    vector<uint8_t> buffer(Common::MaxPacketSize);

    udp::endpoint endpoint(m_hostAddress, 0);
    ErrorCode error;
    stringstream errorMessageStream;

    while (true)
    {
        const uint32_t length = ConvertUInt32(m_dataChannelSocket.receive_from(asio::buffer(buffer), endpoint, 0, error));

        if (IsDisconnecting())
            break;

        if (error)
        {
            DispatchErrorMessage("Error reading data from command channel: " + string(SystemError(error).what()));
            break;
        }

        // Gather statistics
        m_totalDataChannelBytesReceived += length;

        ProcessServerResponse(buffer.data(), 0, length);
    }
}

// Processes a response sent by the server. Response codes are defined in the header file "Constants.h".
void DataSubscriber::ProcessServerResponse(uint8_t* buffer, const uint32_t offset, const uint32_t length)
{
    // Note: internal payload size at buffer[2:6] ignored - future versions of STTP will exclude this
    uint8_t* packetBodyStart = buffer + Common::ResponseHeaderSize;
    const uint32_t packetBodyLength = length - Common::ResponseHeaderSize;

    const uint8_t responseCode = buffer[0];
    const uint8_t commandCode = buffer[1];

    if (!m_validated)
    {
        if (responseCode != ServerResponse::NoOP && (commandCode != ServerCommand::DefineOperationalModes || responseCode != ServerResponse::Succeeded && responseCode != ServerResponse::Failed))
        {
            DispatchErrorMessage("Possible invalid protocol detected from \"" + m_connectionID + "\": encountered unexpected initial command / response code: " + ServerCommand::ToString(commandCode) + " / " + ServerResponse::ToString(responseCode) + " -- connection likely from non-STTP client, disconnecting.");
            auto _ = Thread([this] { Disconnect(false, false, false); });
            return;
        }
    }

    switch (responseCode)
    {
    case ServerResponse::Succeeded:
        HandleSucceeded(commandCode, packetBodyStart, 0, packetBodyLength);
        break;

    case ServerResponse::Failed:
        HandleFailed(commandCode, packetBodyStart, 0, packetBodyLength);
        break;

    case ServerResponse::DataPacket:
        HandleDataPacket(packetBodyStart, 0, packetBodyLength);
        break;

    case ServerResponse::DataStartTime:
        HandleDataStartTime(packetBodyStart, 0, packetBodyLength);
        break;

    case ServerResponse::ProcessingComplete:
        HandleProcessingComplete(packetBodyStart, 0, packetBodyLength);
        break;

    case ServerResponse::UpdateSignalIndexCache:
        HandleUpdateSignalIndexCache(packetBodyStart, 0, packetBodyLength);
        break;

    case ServerResponse::UpdateBaseTimes:
        HandleUpdateBaseTimes(packetBodyStart, 0, packetBodyLength);
        break;

    case ServerResponse::ConfigurationChanged:
        HandleConfigurationChanged(packetBodyStart, 0, packetBodyLength);
        break;

    case ServerResponse::BufferBlock:
        HandleBufferBlock(packetBodyStart, 0, packetBodyLength);
        break;

    case ServerResponse::NoOP:
        break;

    default:
        DispatchErrorMessage("Encountered unexpected server response code: " + ServerResponse::ToString(responseCode) + " from \"" + m_connectionID + "\"");
        break;
    }
}

// Handles success messages received from the server.
void DataSubscriber::HandleSucceeded(const uint8_t commandCode, uint8_t* data, const uint32_t offset, const uint32_t length)
{
    const uint32_t messageLength = ConvertUInt32(length / sizeof(char));

    const std::function sendResponse = [&]()
    {
        if (data != nullptr)
        {
            char* messageStart = reinterpret_cast<char*>(data + offset);
            const char* messageEnd = messageStart + messageLength;
            stringstream messageStream;

            messageStream << "Received success code in response to server command " << ServerCommand::ToString(commandCode) << ": ";

            for (char* messageIter = messageStart; messageIter < messageEnd; ++messageIter)
                messageStream << *messageIter;

            DispatchStatusMessage(messageStream.str());
        }
    };

    switch (commandCode)
    {
    case ServerCommand::DefineOperationalModes:
        m_validated = true;
        m_defineOpModesCompleted.Set();
        sendResponse();
        break;
    case ServerCommand::MetadataRefresh:
        // Metadata refresh message is not sent with a
        // message, but rather the metadata itself.
        HandleMetadataRefresh(data, offset, length);
        break;
    case ServerCommand::Subscribe:
    case ServerCommand::Unsubscribe:
        // Do not break on these messages because there is
        // still an associated message to be processed.
        m_subscribed = (commandCode == ServerCommand::Subscribe);
        [[fallthrough]];
    case ServerCommand::UpdateProcessingInterval:
    case ServerCommand::RotateCipherKeys:
        // Each of these responses come with a message that will
        // be delivered to the user via the status message callback.
        sendResponse();
        break;
    default:
        // If we don't know what the message is, we can't interpret
        // the data sent with the packet. Deliver an error message
        // to the user via the error message callback.
        DispatchErrorMessage("Received success code in response to unknown server command " + ServerCommand::ToString(commandCode));
        break;
    }
}

// Handles failure messages from the server.
void DataSubscriber::HandleFailed(const uint8_t commandCode, uint8_t* data, const uint32_t offset, const uint32_t length)
{
    if (data == nullptr)
        return;

    const uint32_t messageLength = ConvertUInt32(length / sizeof(char));
    stringstream messageStream;

    char* messageStart = reinterpret_cast<char*>(data + offset);
    const char* messageEnd = messageStart + messageLength;

    if (commandCode == ServerCommand::DefineOperationalModes)
    {
        m_validated = false;
        m_defineOpModesCompleted.Set();
        auto _ = Thread([this] { Disconnect(false, false, false); });
    }

    if (commandCode == ServerCommand::Connect)
        m_connector.SetConnectionRefused(true);
    else
        messageStream << "Received failure code from server command " << ServerCommand::ToString(commandCode) << ": ";

    for (char* messageIter = messageStart; messageIter < messageEnd; ++messageIter)
        messageStream << *messageIter;

    DispatchErrorMessage(messageStream.str());
}

// Handles metadata refresh messages from the server.
void DataSubscriber::HandleMetadataRefresh(const uint8_t* data, const uint32_t offset, const uint32_t length)
{
    Dispatch(&MetadataDispatcher, data, offset, length);
}

// Handles data start time reported by the server at the beginning of a subscription.
void DataSubscriber::HandleDataStartTime(const uint8_t* data, const uint32_t offset, const uint32_t length)
{
    Dispatch(&DataStartTimeDispatcher, data, offset, length);
}

// Handles processing complete message sent by the server at the end of a temporal session.
void DataSubscriber::HandleProcessingComplete(const uint8_t* data, const uint32_t offset, const uint32_t length)
{
    Dispatch(&ProcessingCompleteDispatcher, data, offset, length);
}

// Cache signal IDs sent by the server into the signal index cache.
void DataSubscriber::HandleUpdateSignalIndexCache(const uint8_t* data, uint32_t offset, uint32_t length)
{
    if (data == nullptr)
        return;

    vector<uint8_t> uncompressedBuffer;
    int32_t cacheIndex = 0;

    // Get active cache index
    if (m_version > 1)
    {
        if (data[0] > 0)
            cacheIndex = 1;

        offset++;
        length--;
    }

    if (m_compressSignalIndexCache)
    {
        const MemoryStream memoryStream(data, offset, length);

        // Perform zlib decompression on buffer
        StreamBuffer streamBuffer;

        streamBuffer.push(GZipDecompressor());
        streamBuffer.push(memoryStream);

        CopyStream(&streamBuffer, uncompressedBuffer);
    }
    else
    {
        WriteBytes(uncompressedBuffer, data, offset, length);
    }

    SignalIndexCachePtr signalIndexCache = NewSharedPtr<SignalIndexCache>();
    signalIndexCache->Decode(uncompressedBuffer, m_subscriberID);

    m_signalIndexCacheMutex.lock();
    m_signalIndexCache[cacheIndex].swap(signalIndexCache);
    m_cacheIndex = cacheIndex;
    m_signalIndexCacheMutex.unlock();

    if (m_version > 1)
        SendServerCommand(ServerCommand::ConfirmUpdateSignalIndexCache);

    DispatchSubscriptionUpdated(AddDispatchReference(m_signalIndexCache[cacheIndex]));
}

// Updates base time offsets.
void DataSubscriber::HandleUpdateBaseTimes(uint8_t* data, const uint32_t offset, const uint32_t length)
{
    if (data == nullptr)
        return;

    int32_t* timeIndexPtr = reinterpret_cast<int32_t*>(data + offset);
    const int64_t* timeOffsetsPtr = reinterpret_cast<int64_t*>(timeIndexPtr + 1); //-V1032

    m_timeIndex = EndianConverter::Default.ConvertBigEndian(*timeIndexPtr);
    m_baseTimeOffsets[0] = EndianConverter::Default.ConvertBigEndian(timeOffsetsPtr[0]);
    m_baseTimeOffsets[1] = EndianConverter::Default.ConvertBigEndian(timeOffsetsPtr[1]);

    DispatchStatusMessage("Received new base time offset from publisher: " + ToString(FromTicks(m_baseTimeOffsets[m_timeIndex ^ 1])));
}

// Handles configuration changed message sent by the server at the end of a temporal session.
void DataSubscriber::HandleConfigurationChanged(uint8_t* data, const uint32_t offset, const uint32_t length)
{
    Dispatch(&ConfigurationChangedDispatcher);
}

// Handles data packets from the server. Decodes the measurements and provides them to the user via the new measurements callback.
void DataSubscriber::HandleDataPacket(uint8_t* data, uint32_t offset, const uint32_t length)
{
    const NewMeasurementsCallback newMeasurementsCallback = m_newMeasurementsCallback;

    if (newMeasurementsCallback != nullptr)
    {
        const SubscriptionInfo& info = m_subscriptionInfo;
        int64_t frameLevelTimestamp = -1;

        bool includeTime = info.IncludeTime;

        // Read data packet flags
        const uint8_t dataPacketFlags = data[offset];
        offset++;

        // Read frame-level timestamp, if available
        if (dataPacketFlags & DataPacketFlags::Synchronized)
        {
            frameLevelTimestamp = EndianConverter::ToBigEndian<int64_t>(data, offset);
            offset += 8;
            includeTime = false;
        }

        // Read measurement count and gather statistics
        const uint32_t count = EndianConverter::ToBigEndian<uint32_t>(data, offset);
        m_totalMeasurementsReceived += count;
        offset += 4; //-V112

        vector<MeasurementPtr> measurements;
        int32_t cacheIndex = 0;

        if (dataPacketFlags & DataPacketFlags::CacheIndex)
            cacheIndex = 1;

        m_signalIndexCacheMutex.lock();
        const SignalIndexCachePtr signalIndexCache = m_signalIndexCache[cacheIndex];
        m_signalIndexCacheMutex.unlock();

        if (signalIndexCache == nullptr)
            return;

        if (dataPacketFlags & DataPacketFlags::Compressed)
            ParseTSSCMeasurements(signalIndexCache, data, offset, length, measurements);
        else
            ParseCompactMeasurements(signalIndexCache, data, offset, length, includeTime, info.UseMillisecondResolution, frameLevelTimestamp, measurements);

        newMeasurementsCallback(this, measurements);
    }
}

void DataSubscriber::ParseTSSCMeasurements(const SignalIndexCachePtr& signalIndexCache, uint8_t* data, uint32_t offset, const uint32_t length, vector<MeasurementPtr>& measurements)
{
    TSSCDecoderPtr decoder = signalIndexCache->m_tsscDecoder;
    bool newDecoder = false;

    // Use TSSC to decompress measurements
    if (decoder == nullptr)
    {
        signalIndexCache->m_tsscDecoder = NewSharedPtr<TSSCDecoder>();
        decoder = signalIndexCache->m_tsscDecoder;
        decoder->ResetSequenceNumber();
        newDecoder = true;
    }

    string errorMessage;

    if (data[offset] != 85)
        throw SubscriberException("TSSC version not recognized: " + ToHex(data[offset]));

    offset++;

    const uint16_t sequenceNumber = EndianConverter::ToBigEndian<uint16_t>(data, offset);
    offset += 2;

    if (sequenceNumber == 0)
    {
        if (!newDecoder)
        {
            if (decoder->GetSequenceNumber() > 0)
            {
                stringstream statusMessageStream;
                statusMessageStream << "TSSC algorithm reset before sequence number: ";
                statusMessageStream << decoder->GetSequenceNumber();
                DispatchStatusMessage(statusMessageStream.str());
            }

            signalIndexCache->m_tsscDecoder = NewSharedPtr<TSSCDecoder>();
            decoder = signalIndexCache->m_tsscDecoder;
            decoder->ResetSequenceNumber();
        }

        m_tsscResetRequested = false;
        m_tsscLastOOSReportMutex.lock();
        m_tsscLastOOSReport = DateTime::MinValue;
        m_tsscLastOOSReportMutex.unlock();
    }

    if (decoder->GetSequenceNumber() != sequenceNumber)
    {
        if (!m_tsscResetRequested)
        {
            m_tsscLastOOSReportMutex.lock();

            if (TimeSince(m_tsscLastOOSReport) > 2.0F)
            {
                DispatchErrorMessage("TSSC is out of sequence. Expecting: " + ToString(decoder->GetSequenceNumber()) + ", Received: " + ToString(sequenceNumber));
                m_tsscLastOOSReport = UtcNow();
            }

            m_tsscLastOOSReportMutex.unlock();
        }

        // Ignore packets until the reset has occurred.
        return;
    }

    try
    {
        decoder->SetBuffer(data, offset, length);

        Guid signalID;
        string measurementSource;
        uint64_t measurementID;
        int32_t id;
        int64_t time;
        uint32_t quality;
        float32_t value;

        while (decoder->TryGetMeasurement(id, time, quality, value))
        {
            if (signalIndexCache->GetMeasurementKey(id, signalID, measurementSource, measurementID))
            {
                MeasurementPtr measurement = NewSharedPtr<Measurement>();

                measurement->SignalID = signalID;
                measurement->Source = measurementSource;
                measurement->ID = measurementID;
                measurement->Timestamp = time;
                measurement->Flags = static_cast<MeasurementStateFlags>(quality);
                measurement->Value = static_cast<float64_t>(value);

                measurements.push_back(measurement);
            }
        }
    }
    catch (SubscriberException& ex)
    {
        errorMessage = ex.what();
    }
    catch (...)
    {
        errorMessage = current_exception_diagnostic_information(true);
    }

    if (errorMessage.length() > 0)
    {
        DispatchErrorMessage("Failed to parse TSSC measurements - disconnecting: " + errorMessage);
        HandleSocketError();
        return;
    }

    decoder->IncrementSequenceNumber();
}

void DataSubscriber::ParseCompactMeasurements(const SignalIndexCachePtr& signalIndexCache, const uint8_t* data, uint32_t offset, const uint32_t length, const bool includeTime, const bool useMillisecondResolution, const int64_t frameLevelTimestamp, vector<MeasurementPtr>& measurements)
{
    const MessageCallback errorMessageCallback = m_errorMessageCallback;

    if (signalIndexCache == nullptr)
    {
        if (TimeSince(m_lastMissingCacheWarning) >= MissingCacheWarningInterval)
        {
            // Warning message for missing signal index cache
            if (m_lastMissingCacheWarning > DateTime::MinValue)
                DispatchStatusMessage("WARNING: Signal index cache has not arrived. No compact measurements can be parsed.");

            m_lastMissingCacheWarning = UtcNow();
        }

        return;
    }

    // Create measurement parser
    const CompactMeasurement parser(signalIndexCache, m_baseTimeOffsets, includeTime, useMillisecondResolution);

    while (length != offset)
    {
        MeasurementPtr measurement;

        if (!parser.TryParseMeasurement(data, offset, length, measurement))
        {
            if (errorMessageCallback != nullptr)
                errorMessageCallback(this, "Error parsing measurement");

            break;
        }

        if (frameLevelTimestamp > -1)
            measurement->Timestamp = frameLevelTimestamp;

        measurements.push_back(measurement);
    }
}

// Handles buffer blocks from the server.
void DataSubscriber::HandleBufferBlock(uint8_t* data, uint32_t offset, uint32_t length)
{
    DispatchErrorMessage("WARNING: This STTP data subscriber implementation does not currently handle buffer blocks - initiating unsubscribe.");
    SendServerCommand(ServerCommand::Unsubscribe);
}

SignalIndexCache* DataSubscriber::AddDispatchReference(SignalIndexCachePtr signalIndexCacheRef) // NOLINT
{
    SignalIndexCache* signalIndexCachePtr = signalIndexCacheRef.get();

    // Note that no lock is needed for m_signalIndexCacheDispatchRefs set since all calls from
    // HandleUpdateSignalIndexCache are from a single connection and are processed sequentially

    // Hold onto signal index cache shared pointer until it's delivered
    m_signalIndexCacheDispatchRefs.emplace(signalIndexCacheRef);

    return signalIndexCachePtr;
}

SignalIndexCachePtr DataSubscriber::ReleaseDispatchReference(SignalIndexCache* signalIndexCachePtr)
{
    SignalIndexCachePtr signalIndexCacheRef = signalIndexCachePtr->GetReference();

    // Remove used reference to signal index cache pointer
    m_signalIndexCacheDispatchRefs.erase(signalIndexCacheRef);

    return signalIndexCacheRef;
}

// Dispatches the given function to the callback thread.
void DataSubscriber::Dispatch(const DispatcherFunction& function)
{
    Dispatch(function, nullptr, 0, 0);
}

// Dispatches the given function to the callback thread and provides the given data to that function when it is called.
void DataSubscriber::Dispatch(const DispatcherFunction& function, const uint8_t* data, const uint32_t offset, const uint32_t length)
{
    CallbackDispatcher dispatcher;
    const SharedPtr<vector<uint8_t>> dataVector = NewSharedPtr<vector<uint8_t>>();

    dataVector->resize(length);

    if (data != nullptr)
    {
        for (uint32_t i = 0; i < length; ++i)
            dataVector->at(i) = data[offset + i];
    }

    dispatcher.Source = this;
    dispatcher.Data = dataVector;
    dispatcher.Function = function;

    m_callbackQueue.Enqueue(dispatcher);
}

void DataSubscriber::DispatchSubscriptionUpdated(SignalIndexCache* signalIndexCache)
{
    Dispatch(&SubscriptionUpdatedDispatcher, reinterpret_cast<uint8_t*>(&signalIndexCache), 0, sizeof(SignalIndexCache**));
}

// Invokes the status message callback on the callback thread and provides the given message to it.
void DataSubscriber::DispatchStatusMessage(const string& message)
{
    const uint32_t messageSize = ConvertUInt32((message.size() + 1) * sizeof(char));
    Dispatch(&StatusMessageDispatcher, reinterpret_cast<const uint8_t*>(message.c_str()), 0, messageSize);
}

// Invokes the error message callback on the callback thread and provides the given message to it.
void DataSubscriber::DispatchErrorMessage(const string& message)
{
    const uint32_t messageSize = ConvertUInt32((message.size() + 1) * sizeof(char));
    Dispatch(&ErrorMessageDispatcher, reinterpret_cast<const uint8_t*>(message.c_str()), 0, messageSize);
}

// Dispatcher function for status messages. Decodes the message and provides it to the user via the status message callback.
void DataSubscriber::StatusMessageDispatcher(DataSubscriber* source, const vector<uint8_t>& buffer)
{
    if (source == nullptr || buffer.empty())
        return;

    const MessageCallback statusMessageCallback = source->m_statusMessageCallback;

    if (statusMessageCallback != nullptr)
        statusMessageCallback(source, reinterpret_cast<const char*>(buffer.data()));
}

// Dispatcher function for error messages. Decodes the message and provides it to the user via the error message callback.
void DataSubscriber::ErrorMessageDispatcher(DataSubscriber* source, const vector<uint8_t>& buffer)
{
    if (source == nullptr || buffer.empty())
        return;

    const MessageCallback errorMessageCallback = source->m_errorMessageCallback;

    if (errorMessageCallback != nullptr)
        errorMessageCallback(source, reinterpret_cast<const char*>(buffer.data()));
}

// Dispatcher function for data start time. Decodes the start time and provides it to the user via the data start time callback.
void DataSubscriber::DataStartTimeDispatcher(DataSubscriber* source, const vector<uint8_t>& buffer)
{
    if (source == nullptr || buffer.empty())
        return;

    const DataStartTimeCallback dataStartTimeCallback = source->m_dataStartTimeCallback;

    if (dataStartTimeCallback != nullptr)
    {
        const int64_t dataStartTime = EndianConverter::ToBigEndian<int64_t>(buffer.data(), 0);
        dataStartTimeCallback(source, dataStartTime);
    }
}

// Dispatcher function for metadata. Provides encoded metadata to the user via the metadata callback.
void DataSubscriber::MetadataDispatcher(DataSubscriber* source, const vector<uint8_t>& buffer)
{
    if (source == nullptr) // Empty buffer OK
        return;

    const MetadataCallback metadataCallback = source->m_metadataCallback;

    if (metadataCallback != nullptr)
        metadataCallback(source, buffer);
}

void DataSubscriber::SubscriptionUpdatedDispatcher(DataSubscriber* source, const std::vector<uint8_t>& buffer)
{
    if (source == nullptr || buffer.empty())
        return;

    SignalIndexCache* signalIndexCachePtr = *reinterpret_cast<SignalIndexCache**>(const_cast<uint8_t*>(buffer.data()));

    if (signalIndexCachePtr != nullptr)
    {
        const SubscriptionUpdatedCallback subscriptionUpdated = source->m_subscriptionUpdatedCallback;
        const SignalIndexCachePtr signalIndexCacheRef = source->ReleaseDispatchReference(signalIndexCachePtr); //-V821

        if (subscriptionUpdated != nullptr)
            subscriptionUpdated(source, signalIndexCacheRef);
    }
}

// Dispatcher for processing complete message that is sent by the server at the end of a temporal session.
void DataSubscriber::ProcessingCompleteDispatcher(DataSubscriber* source, const vector<uint8_t>& buffer)
{
    if (source == nullptr) // Empty buffer OK
        return;

    const MessageCallback processingCompleteCallback = source->m_processingCompleteCallback;

    if (processingCompleteCallback != nullptr)
    {
        stringstream messageStream;

        for (uint32_t i = 0; i < ConvertUInt32(buffer.size()); ++i)
            messageStream << buffer[i];

        processingCompleteCallback(source, messageStream.str());
    }
}

// Dispatcher for processing configuration changed message that is sent by the server when measurement availability has changed
void DataSubscriber::ConfigurationChangedDispatcher(DataSubscriber* source, const vector<uint8_t>& buffer)
{
    if (source == nullptr) // Empty buffer OK
        return;

    const ConfigurationChangedCallback configurationChangedCallback = source->m_configurationChangedCallback;

    if (configurationChangedCallback != nullptr)
        configurationChangedCallback(source);
}

// Registers the status message callback.
void DataSubscriber::RegisterStatusMessageCallback(const MessageCallback& statusMessageCallback)
{
    m_statusMessageCallback = statusMessageCallback;
}

// Registers the error message callback.
void DataSubscriber::RegisterErrorMessageCallback(const MessageCallback& errorMessageCallback)
{
    m_errorMessageCallback = errorMessageCallback;
}

// Registers the data start time callback.
void DataSubscriber::RegisterDataStartTimeCallback(const DataStartTimeCallback& dataStartTimeCallback)
{
    m_dataStartTimeCallback = dataStartTimeCallback;
}

// Registers the metadata callback.
void DataSubscriber::RegisterMetadataCallback(const MetadataCallback& metadataCallback)
{
    m_metadataCallback = metadataCallback;
}

// Registers the subscription updated callback.
void DataSubscriber::RegisterSubscriptionUpdatedCallback(const SubscriptionUpdatedCallback& subscriptionUpdatedCallback)
{
    m_subscriptionUpdatedCallback = subscriptionUpdatedCallback;
}

// Registers the new measurements callback.
void DataSubscriber::RegisterNewMeasurementsCallback(const NewMeasurementsCallback& newMeasurementsCallback)
{
    m_newMeasurementsCallback = newMeasurementsCallback;
}

// Registers the processing complete callback.
void DataSubscriber::RegisterProcessingCompleteCallback(const MessageCallback& processingCompleteCallback)
{
    m_processingCompleteCallback = processingCompleteCallback;
}

// Registers the configuration changed callback.
void DataSubscriber::RegisterConfigurationChangedCallback(const ConfigurationChangedCallback& configurationChangedCallback)
{
    m_configurationChangedCallback = configurationChangedCallback;
}

// Registers the connection established callback.
void DataSubscriber::RegisterConnectionEstablishedCallback(const ConnectionEstablishedCallback& connectionEstablishedCallback)
{
    m_connectionEstablishedCallback = connectionEstablishedCallback;
}

// Registers the connection terminated callback.
void DataSubscriber::RegisterConnectionTerminatedCallback(const ConnectionTerminatedCallback& connectionTerminatedCallback)
{
    m_connectionTerminatedCallback = connectionTerminatedCallback;
}

// Registers the auto-reconnect callback.
void DataSubscriber::RegisterAutoReconnectCallback(const ClientConnectionTerminatedCallback& autoReconnectCallback)
{
    m_autoReconnectCallback = autoReconnectCallback;
}

const Guid& DataSubscriber::GetSubscriberID() const
{
    return m_subscriberID;
}

// Returns true if payload data is compressed (TSSC only).
bool DataSubscriber::IsPayloadDataCompressed() const
{
    return m_compressPayloadData;
}

// Set the value which determines whether payload data is compressed.
void DataSubscriber::SetPayloadDataCompressed(const bool compressed)
{
    // This operational mode can only be changed before connect - dynamic updates not supported
    m_compressPayloadData = compressed;
}

// Returns true if metadata exchange is compressed (GZip only).
bool DataSubscriber::IsMetadataCompressed() const
{
    return m_compressMetadata;
}

// Set the value which determines whether metadata exchange is compressed.
void DataSubscriber::SetMetadataCompressed(const bool compressed)
{
    m_compressMetadata = compressed;
}

// Returns true if signal index cache exchange is compressed (GZip only).
bool DataSubscriber::IsSignalIndexCacheCompressed() const
{
    return m_compressSignalIndexCache;
}

// Set the value which determines whether signal index cache exchange is compressed.
void DataSubscriber::SetSignalIndexCacheCompressed(const bool compressed)
{
    m_compressSignalIndexCache = compressed;
}

uint8_t DataSubscriber::GetVersion() const
{
    return m_version;
}

void DataSubscriber::SetVersion(const uint8_t version)
{
    if (version < 1 || version > 3)
        throw invalid_argument("This STTP data subscriber implementation only supports version 1 to 3 of the protocol");

    m_version = version;
}

// Gets user defined data reference
void* DataSubscriber::GetUserData() const
{
    return m_userData;
}

// Sets user defined data reference
void DataSubscriber::SetUserData(void* userData)
{
    m_userData = userData;
}

SubscriberConnector& DataSubscriber::GetSubscriberConnector()
{
    return m_connector;
}

// Returns the subscription info object used to define the most recent subscription.
const SubscriptionInfo& DataSubscriber::GetSubscriptionInfo() const
{
    return m_subscriptionInfo;
}

void DataSubscriber::SetSubscriptionInfo(const SubscriptionInfo& info)
{
    m_subscriptionInfo = info;
}

// Dispatcher for connection terminated. This is called from its own separate thread
// in order to cleanly shut down the subscriber in case the connection was terminated
// by the peer. Additionally, this allows the user to automatically reconnect in their
// callback function without having to spawn their own separate thread.
void DataSubscriber::ConnectionTerminatedDispatcher()
{
    Disconnect(false, true, false);
}

void DataSubscriber::WaitOnDisconnectThread()
{
    ScopeLock lock(m_disconnectThreadMutex);
    m_disconnectThread.join();
}

// Synchronously connects to publisher.
// public:
void DataSubscriber::Connect(const string& hostname, const uint16_t port)
{
    // User requests to connect are not an auto-reconnect attempt
    Connect(hostname, port, false);
}

// private:
void DataSubscriber::Connect(const string& hostname, const uint16_t port, const bool autoReconnecting)
{
    // This function fails by exception, consumers should try/catch calls to Connect
    if (IsConnected())
        throw SubscriberException("DataSubscriber is already connected; disconnect first");

    if (IsListening())
        throw SubscriberException("Cannot start connection, DataSubscriber is already established with a listening mode reverse connection");

    // Make sure any pending disconnect has completed to make sure socket is closed
    WaitOnDisconnectThread();

    // Let any pending connect or disconnect operation complete before new connect
    // this prevents destruction disconnect before connection is completed
    ScopeLock lock(m_connectActionMutex);
    DnsResolver resolver(m_commandChannelService);
    const DnsResolver::query dnsQuery(hostname, ToString(port));
    ErrorCode error;

    // Initialize connection state
    SetupConnection();

    if (!autoReconnecting)
        m_connector.ResetConnection();

    m_connector.SetConnectionRefused(false);

    const TcpEndPoint hostEndpoint = connect(m_commandChannelSocket, resolver.resolve(dnsQuery), error);

    if (error)
        throw SystemError(error);

    if (!m_commandChannelSocket.is_open())
        throw SubscriberException("Failed to connect to host");

    m_hostAddress = hostEndpoint.address();

#if BOOST_LEGACY
    m_commandChannelService.reset();
#else
    m_commandChannelService.restart();
#endif

    EstablishConnection(hostEndpoint, false);
}

void DataSubscriber::Listen(const sttp::TcpEndPoint& endpoint)
{
    // This function fails by exception, consumers should try/catch calls to Start
    if (IsListening())
        throw SubscriberException("DataSubscriber is already listening; disconnect first");

    if (IsConnected())
        throw SubscriberException("Cannot start a listening mode reverse connection, DataSubscriber is already connected");

    // Make sure any pending disconnect has completed to make sure socket is closed
    WaitOnDisconnectThread();

#if BOOST_LEGACY
    m_commandChannelService.reset();
#else
    m_commandChannelService.restart();
#endif

    // TODO: Add TLS implementation options
    m_clientAcceptor = TcpAcceptor(m_commandChannelService, endpoint, false); //-V601

    // Run command channel accept thread
    m_commandChannelAcceptThread = Thread([this]
        {
            StartAccept();
            m_commandChannelService.run();
        });

    m_listening = true;
}

void DataSubscriber::Listen(const uint16_t port, const bool ipV6)
{
    Listen(TcpEndPoint(ipV6 ? tcp::v6() : tcp::v4(), port));
}

void DataSubscriber::Listen(const std::string& networkInterfaceIP, const uint16_t port)
{
    Listen(TcpEndPoint(make_address(networkInterfaceIP), port));
}

void DataSubscriber::StartAccept()
{
    TcpSocket commandChannelSocket(m_commandChannelService);

    m_clientAcceptor.async_accept(commandChannelSocket, [this, &commandChannelSocket]<typename T0>(T0 && error)
    {
        AcceptConnection(commandChannelSocket, error);
    });
}

void DataSubscriber::AcceptConnection(TcpSocket& commandChannelSocket, const ErrorCode& error)
{
    if (!IsListening())
        return;

    if (error)
    {
        DispatchErrorMessage("Error while attempting to accept DataPublisher connection for reverse connection: " + string(SystemError(error).what()));
    }
    else
    {
        // TODO: For secured connections, validate certificate and IP information here to assign subscriberID
        const TcpEndPoint endPoint = commandChannelSocket.remote_endpoint();

        // Will only accept one active connection at a time, this may be indicative
        // of a rouge connection attempt - consumer should log warning
        if (IsConnected())
        {
            string address = "<unknown>";
            string errorMessage = "closed.";

            try
            {
                address = ResolveDNSName(m_commandChannelService, endPoint);
                commandChannelSocket.close();
            }
            catch (SystemError& ex)
            {
                errorMessage = "close error: " + string(ex.what());
            }
            catch (...)
            {
                errorMessage = "close error: " + boost::current_exception_diagnostic_information(true);
            }

            DispatchErrorMessage("WARNING: Duplicate connection attempt detected from: \"" + address + "\". Existing data publisher connection already established, data subscriber will only accept one connection at a time - connection " + errorMessage);
        }
        else
        {
            // Let any pending connect or disconnect operation complete before new connect,
            // this prevents destruction disconnect before connection is completed
            ScopeLock lock(m_connectActionMutex);

            // Initialize connection state
            SetupConnection();

            // Hold on to primary socket
            m_commandChannelSocket = std::move(commandChannelSocket);

            // Create new command channel
            EstablishConnection(endPoint, true);
        }
    }

    StartAccept();
}

// Disconnects from the publisher.
// public:
void DataSubscriber::Disconnect()
{
    if (IsDisconnecting())
        return;

    // Disconnect method executes shutdown on a separate thread without stopping to prevent
    // issues where user may call disconnect method from a dispatched event thread. Also,
    // user requests to disconnect are not an auto-reconnect attempt and should initiate
    // shutdown of listening socket as well.
    Disconnect(false, false, true);
}

// private:
void DataSubscriber::Disconnect(const bool joinThread, const bool autoReconnecting, const bool includeListener)
{
    // Check if disconnect thread is running or subscriber has already disconnected
    if (IsDisconnecting())
    {
        if (!autoReconnecting && !m_listening && !m_disconnected)
            m_connector.Cancel();

        if (joinThread && !m_disconnected)
            WaitOnDisconnectThread();

        return;
    }

    // Notify running threads that the subscriber is disconnecting, i.e., disconnect thread is active
    m_disconnecting = true;
    m_connected = false;
    m_validated = false;

    if (includeListener)
        m_listening = false;

    m_defineOpModesCompleted.Set();
    m_defineOpModesCompleted.Reset();
    m_subscribed = false;

    {
        ScopeLock lock(m_disconnectThreadMutex);

        m_disconnectThread = Thread([this, autoReconnecting, includeListener]
        {
            try
            {
                // Let any pending connect operation complete before disconnect - prevents destruction disconnect before connection is completed
                if (!autoReconnecting)
                {
                    m_connector.Cancel();
                    m_connectionTerminationThread.join();
                    m_connectActionMutex.lock();
                }

                ErrorCode error;

                // Release queues and close sockets so that threads can shut down gracefully
                if (includeListener)
                    m_clientAcceptor.close(error);

                m_callbackQueue.Release();
                m_commandChannelSocket.close(error);
                m_dataChannelSocket.shutdown(UdpSocket::shutdown_receive, error);
                m_dataChannelSocket.close(error);

                // Join with all threads to guarantee their completion before returning control to the caller
                if (includeListener)
                    m_commandChannelAcceptThread.join();

                m_callbackThread.join();
                m_commandChannelResponseThread.join();
                m_dataChannelResponseThread.join();

                // Empty queues and reset them so they can be used again later if the user decides to reconnect
                m_callbackQueue.Clear();
                m_callbackQueue.Reset();

                // Notify consumers of disconnect
                if (m_connectionTerminatedCallback != nullptr)
                    m_connectionTerminatedCallback(this);

                if (!autoReconnecting)
                    m_commandChannelService.stop();
            }
            catch (SystemError& ex)
            {
                DispatchErrorMessage("Exception while disconnecting data subscriber: " + string(ex.what()));
            }
            catch (...)
            {
                DispatchErrorMessage("Exception while disconnecting data subscriber: " + boost::current_exception_diagnostic_information(true));
            }

            // Disconnect complete
            m_disconnected = true;
            m_disconnecting = false;

            if (autoReconnecting)
            {
                // Handling auto-connect callback separately from connection terminated callback
                // since they serve two different use cases and current implementation does not
                // support multiple callback registrations
                if (m_autoReconnectCallback != nullptr && !m_disposing)
                    m_autoReconnectCallback(this);
            }
            else
            {
                m_connectActionMutex.unlock();
            }
        });
    }

    if (joinThread)
        WaitOnDisconnectThread();
}

void DataSubscriber::SetupConnection()
{
    m_validated = m_version < 3;
    m_defineOpModesCompleted.Reset();

    m_disconnected = false;
    m_subscribed = false;

    m_totalCommandChannelBytesReceived = 0UL;
    m_totalDataChannelBytesReceived = 0UL;
    m_totalMeasurementsReceived = 0UL;

    // TODO: Clear UDP key and initialization vectors
    // TODO: Clear buffer block expected sequence number
    // TODO: Reinitialize measurement metadata registry
}

void DataSubscriber::EstablishConnection(const TcpEndPoint& endPoint, const bool listening)
{
    m_connectionID = ResolveDNSName(m_commandChannelService, endPoint);

    if (listening)
        DispatchStatusMessage("Processing connection attempt from \"" + m_connectionID + "\" ...");

    m_callbackThread = Thread([this] { RunCallbackThread(); });
    m_commandChannelResponseThread = Thread([this] { RunCommandChannelResponseThread(); });
    m_connected = true;
    m_lastMissingCacheWarning = DateTime::MinValue;

    SendOperationalModes();

    // Notify consumers of connect
    if (m_connectionEstablishedCallback != nullptr)
        m_connectionEstablishedCallback(this);
}

void DataSubscriber::HandleSocketError()
{
    // Connection closed by peer; terminate connection
    m_connectionTerminationThread = Thread([this] { ConnectionTerminatedDispatcher(); });
}

void DataSubscriber::Subscribe(const SubscriptionInfo& info)
{
    SetSubscriptionInfo(info);
    Subscribe();
}

// Subscribe to publisher in order to start receiving data.
void DataSubscriber::Subscribe()
{
    stringstream connectionStream;
    vector<uint8_t> buffer;
    uint32_t bigEndianConnectionStringSize;

    // Make sure to unsubscribe before attempting another
    // subscription so we don't leave connections open
    if (m_subscribed)
        Unsubscribe();

    m_totalMeasurementsReceived = 0UL;

    connectionStream << "throttled=" << m_subscriptionInfo.Throttled << ";";
    connectionStream << "publishInterval=" << m_subscriptionInfo.PublishInterval << ";";
    connectionStream << "includeTime=" << m_subscriptionInfo.IncludeTime << ";";
    connectionStream << "enableTimeReasonabilityCheck=" << m_subscriptionInfo.EnableTimeReasonabilityCheck << ";";
    connectionStream << "lagTime=" << m_subscriptionInfo.LagTime << ";";
    connectionStream << "leadTime=" << m_subscriptionInfo.LeadTime << ";";
    connectionStream << "useLocalClockAsRealTime=" << m_subscriptionInfo.UseLocalClockAsRealTime << ";";
    connectionStream << "processingInterval=" << m_subscriptionInfo.ProcessingInterval << ";";
    connectionStream << "useMillisecondResolution=" << m_subscriptionInfo.UseMillisecondResolution << ";";
    connectionStream << "requestNaNValueFilter=" << m_subscriptionInfo.RequestNaNValueFilter << ";";
    connectionStream << "assemblyInfo={source=" << m_assemblySource << "; version=" << m_assemblyVersion << "; updatedOn=" << m_assemblyUpdatedOn << "};";

    if (!m_subscriptionInfo.FilterExpression.empty())
        connectionStream << "filterExpression={" << m_subscriptionInfo.FilterExpression << "};";

    if (m_subscriptionInfo.UdpDataChannel)
    {
        udp ipVersion = udp::v4();

        if (m_hostAddress.is_v6())
            ipVersion = udp::v6();

        // Attempt to bind to local UDP port
        m_dataChannelSocket.open(ipVersion);
        m_dataChannelSocket.bind(udp::endpoint(ipVersion, m_subscriptionInfo.DataChannelLocalPort));
        m_dataChannelResponseThread = Thread([this] { RunDataChannelResponseThread(); });

        if (!m_dataChannelSocket.is_open())
            throw SubscriberException("Failed to bind to local port");

        connectionStream << "dataChannel={localport=" << m_subscriptionInfo.DataChannelLocalPort << "};";
    }

    if (!m_subscriptionInfo.StartTime.empty())
        connectionStream << "startTimeConstraint=" << m_subscriptionInfo.StartTime << ";";

    if (!m_subscriptionInfo.StopTime.empty())
        connectionStream << "stopTimeConstraint=" << m_subscriptionInfo.StopTime << ";";

    if (!m_subscriptionInfo.ConstraintParameters.empty())
        connectionStream << "timeConstraintParameters=" << m_subscriptionInfo.ConstraintParameters << ";";

    if (!m_subscriptionInfo.ExtraConnectionStringParameters.empty())
        connectionStream << m_subscriptionInfo.ExtraConnectionStringParameters << ";";

    string connectionString = connectionStream.str();
    const uint8_t* connectionStringPtr = reinterpret_cast<uint8_t*>(connectionString.data());
    const uint32_t connectionStringSize = ConvertUInt32(connectionString.size() * sizeof(char));
    bigEndianConnectionStringSize = EndianConverter::Default.ConvertBigEndian(connectionStringSize);
    const uint8_t* bigEndianConnectionStringSizePtr = reinterpret_cast<uint8_t*>(&bigEndianConnectionStringSize);

    const uint32_t bufferSize = 5 + connectionStringSize;
    buffer.resize(bufferSize, 0);

    buffer[0] = DataPacketFlags::Compact;
    buffer[1] = bigEndianConnectionStringSizePtr[0];
    buffer[2] = bigEndianConnectionStringSizePtr[1];
    buffer[3] = bigEndianConnectionStringSizePtr[2];
    buffer[4] = bigEndianConnectionStringSizePtr[3];

    for (size_t i = 0; i < connectionStringSize; ++i)
        buffer[5 + i] = connectionStringPtr[i];

    SendServerCommand(ServerCommand::Subscribe, buffer.data(), 0, bufferSize);

    // Reset TSSC decompresser on successful (re)subscription
    m_tsscLastOOSReportMutex.lock();
    m_tsscLastOOSReport = DateTime::MinValue;
    m_tsscLastOOSReportMutex.unlock();
    m_tsscResetRequested = true;
}

// Unsubscribe from publisher to stop receiving data.
void DataSubscriber::Unsubscribe()
{
    ErrorCode error;

    m_disconnecting = true;
    m_dataChannelSocket.shutdown(UdpSocket::shutdown_receive, error);
    m_dataChannelSocket.close(error);
    m_dataChannelResponseThread.join();
    m_disconnecting = false;

    SendServerCommand(ServerCommand::Unsubscribe);
}

// Sends a command to the server.
void DataSubscriber::SendServerCommand(const uint8_t commandCode)
{
    SendServerCommand(commandCode, nullptr, 0, 0);
}

// Sends a command along with the given message to the server.
void DataSubscriber::SendServerCommand(const uint8_t commandCode, const string& message)
{
    // Note that for UTF-8 strings with extended characters, "size()" will properly
    // return byte-length, not number of characters, which makes the following code
    // accurate. That said, using the Windows command prompt with extended characters
    // will likely not display properly without call to "SetConsoleOutputCP(CP_UTF8)"
    vector<uint8_t> buffer;
    uint32_t bigEndianMessageSize;
    const uint8_t* messagePtr = reinterpret_cast<const uint8_t*>(message.data());
    const uint32_t messageSize = ConvertUInt32(message.size() * sizeof(char));

    bigEndianMessageSize = EndianConverter::Default.ConvertBigEndian(messageSize);
    const uint8_t* bigEndianMessageSizePtr = reinterpret_cast<uint8_t*>(&bigEndianMessageSize);

    const uint32_t bufferSize = 4 + messageSize;
    buffer.resize(bufferSize, 0);

    buffer[0] = bigEndianMessageSizePtr[0];
    buffer[1] = bigEndianMessageSizePtr[1];
    buffer[2] = bigEndianMessageSizePtr[2];
    buffer[3] = bigEndianMessageSizePtr[3];

    for (size_t i = 0; i < messageSize; ++i)
        buffer[4 + i] = messagePtr[i];

    SendServerCommand(commandCode, buffer.data(), 0, bufferSize);
}

// Sends a command along with the given data to the server.
void DataSubscriber::SendServerCommand(const uint8_t commandCode, const uint8_t* data, const uint32_t offset, const uint32_t length)
{
    if (!m_connected)
        return;

    if (!m_validated && commandCode != ServerCommand::DefineOperationalModes)
    {
        DispatchStatusMessage("WARNING: Cannot send any server command until requested operational modes for connection have been accepted.");
        return;
    }

    const uint32_t packetSize = length + 1;
    uint32_t bigEndianPacketSize = EndianConverter::Default.ConvertBigEndian(packetSize);
    const uint8_t* bigEndianPacketSizePtr = reinterpret_cast<uint8_t*>(&bigEndianPacketSize);
    const uint32_t commandBufferSize = packetSize + Common::PayloadHeaderSize;

    if (commandBufferSize > ConvertUInt32(m_writeBuffer.size()))
        m_writeBuffer.resize(commandBufferSize);

    // Insert packet size
    m_writeBuffer[0] = bigEndianPacketSizePtr[0];
    m_writeBuffer[1] = bigEndianPacketSizePtr[1];
    m_writeBuffer[2] = bigEndianPacketSizePtr[2];
    m_writeBuffer[3] = bigEndianPacketSizePtr[3];

    // Insert command code
    m_writeBuffer[4] = commandCode;

    if (data != nullptr)
    {
        for (size_t i = 0; i < length; ++i)
            m_writeBuffer[5 + i] = data[offset + i];
    }

    async_write(m_commandChannelSocket, buffer(m_writeBuffer, commandBufferSize), [this]<typename T0, typename T1>(T0 && error, T1 && bytesTransferred)
    {
        WriteHandler(error, bytesTransferred);
    });
}

void DataSubscriber::WriteHandler(const ErrorCode& error, const size_t bytesTransferred)
{
    if (IsDisconnecting())
        return;

    if (error == error::connection_aborted || error == error::connection_reset || error == error::eof)
    {
        HandleSocketError();
        return;
    }

    if (error)
        DispatchErrorMessage("Error reading data from command channel: " + string(SystemError(error).what()));
}

// Convenience method to send the currently defined
// and/or supported operational modes to the server.
void DataSubscriber::SendOperationalModes()
{
    uint32_t operationalModes = OperationalModes::NoFlags;
    uint32_t bigEndianOperationalModes;

    operationalModes |= OperationalModes::VersionMask & m_version;
    operationalModes |= OperationalEncoding::UTF8;

    if (m_version < 10)
        operationalModes |= CompressionModes::GZip;

    // TSSC compression only works with stateful connections
    if (m_compressPayloadData && !m_subscriptionInfo.UdpDataChannel)
    {
        operationalModes |= OperationalModes::CompressPayloadData;

        if (m_version < 10)
            operationalModes |= CompressionModes::TSSC;
    }

    if (m_compressMetadata)
        operationalModes |= OperationalModes::CompressMetadata;

    if (m_compressSignalIndexCache)
        operationalModes |= OperationalModes::CompressSignalIndexCache;

    bigEndianOperationalModes = EndianConverter::Default.ConvertBigEndian(operationalModes);
    SendServerCommand(ServerCommand::DefineOperationalModes, reinterpret_cast<uint8_t*>(&bigEndianOperationalModes), 0, 4);
}

// Gets the total number of bytes received via the command channel since last connection.
uint64_t DataSubscriber::GetTotalCommandChannelBytesReceived() const
{
    return m_totalCommandChannelBytesReceived;
}

// Gets the total number of bytes received via the data channel since last connection.
uint64_t DataSubscriber::GetTotalDataChannelBytesReceived() const
{
    if (m_subscriptionInfo.UdpDataChannel)
        return m_totalDataChannelBytesReceived;

    return m_totalCommandChannelBytesReceived;
}

// Gets the total number of measurements received since last subscription.
uint64_t DataSubscriber::GetTotalMeasurementsReceived() const
{
    return m_totalMeasurementsReceived;
}

// Indicates whether the subscriber is connected.
bool DataSubscriber::IsConnected()
{
    return m_connected;
}

// Indicates whether the subscriber is started in listening mode.
bool DataSubscriber::IsListening() const
{
    return m_listening;
}

bool DataSubscriber::WaitForOperationalModesResponse(const int32_t timeout)
{
    return m_defineOpModesCompleted.Wait(timeout);
}

std::string DataSubscriber::OperationalModesStatus() const
{
    if (m_validated)
        return "Accepted -- Connection Validated";

    if (m_defineOpModesCompleted.IsSet())
        return "Rejected -- Connection Canceled";

    return "Pending Response";
}

// Indicates if subscriber connection has been validated as an STTP connection.
bool DataSubscriber::IsValidated() const
{
    return m_validated;
}

// Indicates whether the subscriber is subscribed.
bool DataSubscriber::IsSubscribed() const
{
    return m_subscribed;
}

std::string DataSubscriber::GetConnectionID() const
{
    return m_connectionID;
}

void DataSubscriber::GetAssemblyInfo(string& source, string& version, string& updatedOn) const
{
    source = m_assemblySource;
    version = m_assemblyVersion;
    updatedOn = m_assemblyUpdatedOn;
}

void DataSubscriber::SetAssemblyInfo(const string& source, const string& version, const string& updatedOn)
{
    m_assemblySource = source;
    m_assemblyVersion = version;
    m_assemblyUpdatedOn = updatedOn;
}