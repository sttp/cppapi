//******************************************************************************************************
//  DataSubscriber.h - Gbtc
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
//       Updated DataSubscriber callback function signatures to always include instance reference.
//
//******************************************************************************************************

#pragma once

#include "../ThreadSafeQueue.h"
#include "../ManualResetEvent.h"
#include "TransportTypes.h"
#include "SubscriptionInfo.h"
#include "SubscriberConnector.h"
#include "SignalIndexCache.h"

namespace sttp::transport
{
    class DataSubscriber;
    typedef SharedPtr<DataSubscriber> DataSubscriberPtr;

    class DataSubscriber final : public DataClient // NOLINT
    {
    public:
        // Function pointer types
        typedef std::function<void(DataSubscriber*, const std::vector<uint8_t>&)> DispatcherFunction;
        typedef std::function<void(DataSubscriber*, const std::string&)> MessageCallback;
        typedef std::function<void(DataSubscriber*, int64_t)> DataStartTimeCallback;
        typedef std::function<void(DataSubscriber*, const std::vector<uint8_t>&)> MetadataCallback;
        typedef std::function<void(DataSubscriber*, const SignalIndexCachePtr&)> SubscriptionUpdatedCallback;
        typedef std::function<void(DataSubscriber*, const std::vector<MeasurementPtr>&)> NewMeasurementsCallback;
        typedef std::function<void(DataSubscriber*)> ConfigurationChangedCallback;
        typedef std::function<void(DataSubscriber*)> ConnectionEstablishedCallback;
        typedef std::function<void(DataSubscriber*)> ConnectionTerminatedCallback;

    private:
        // Structure used to dispatch
        // callbacks on the callback thread.
        struct CallbackDispatcher
        {
            DataSubscriber* Source;
            SharedPtr<std::vector<uint8_t>> Data;
            DispatcherFunction Function;

            CallbackDispatcher();
        };

        SubscriberConnector m_connector;
        SubscriptionInfo m_subscriptionInfo;
        IPAddress m_hostAddress;
        Guid m_subscriberID;
        bool m_compressPayloadData;
        bool m_compressMetadata;
        bool m_compressSignalIndexCache;
        uint8_t m_version;
        std::atomic_bool m_connected;
        std::atomic_bool m_listening;
        ManualResetEvent m_defineOpModesCompleted;
        std::atomic_bool m_validated;
        std::atomic_bool m_subscribed;
        std::string m_connectionID;

        Mutex m_connectActionMutex;
        Thread m_connectionTerminationThread;
        Mutex m_disconnectThreadMutex;
        Thread m_disconnectThread;
        std::atomic_bool m_disconnecting;
        std::atomic_bool m_disconnected;

        void* m_userData;

        // Statistics counters
        uint64_t m_totalCommandChannelBytesReceived;
        uint64_t m_totalDataChannelBytesReceived;
        uint64_t m_totalMeasurementsReceived;

        // Assembly info
        std::string m_assemblySource;
        std::string m_assemblyVersion;
        std::string m_assemblyUpdatedOn;

        // Measurement parsing
        SignalIndexCachePtr m_signalIndexCache[2];
        Mutex m_signalIndexCacheMutex;
        int32_t m_cacheIndex;
        int32_t m_timeIndex;
        int64_t m_baseTimeOffsets[2];
        datetime_t m_lastMissingCacheWarning;
        bool m_tsscResetRequested;
        datetime_t m_tsscLastOOSReport;
        Mutex m_tsscLastOOSReportMutex;

        // Dispatch reference - unordered set works fine for signal
        // index cache since each new call will be for a new instance
        std::unordered_set<SignalIndexCachePtr> m_signalIndexCacheDispatchRefs;

        // Callback thread members
        Thread m_callbackThread;
        ThreadSafeQueue<CallbackDispatcher> m_callbackQueue;

        // Command channel
        Thread m_commandChannelResponseThread;
        IOContext m_commandChannelService;
        TcpSocket m_commandChannelSocket;
        Thread m_commandChannelAcceptThread;
        TcpAcceptor m_clientAcceptor;
        std::vector<uint8_t> m_readBuffer;
        std::vector<uint8_t> m_writeBuffer;

        // Data channel
        Thread m_dataChannelResponseThread;
        IOContext m_dataChannelService;
        UdpSocket m_dataChannelSocket;

        // Callbacks
        MessageCallback m_statusMessageCallback;
        MessageCallback m_errorMessageCallback;
        DataStartTimeCallback m_dataStartTimeCallback;
        MetadataCallback m_metadataCallback;
        SubscriptionUpdatedCallback m_subscriptionUpdatedCallback;
        NewMeasurementsCallback m_newMeasurementsCallback;
        MessageCallback m_processingCompleteCallback;
        ConfigurationChangedCallback m_configurationChangedCallback;
        ConnectionEstablishedCallback m_connectionEstablishedCallback;
        ConnectionTerminatedCallback m_connectionTerminatedCallback;
        ClientConnectionTerminatedCallback m_autoReconnectCallback;

        // Threads
        void RunCallbackThread();
        void RunCommandChannelResponseThread();
        void RunDataChannelResponseThread();

        // Command channel callbacks
        void ReadPayloadHeader(const ErrorCode& error, size_t bytesTransferred);
        void ReadPacket(const ErrorCode& error, size_t bytesTransferred);
        void WriteHandler(const ErrorCode& error, size_t bytesTransferred);
        void StartAccept();
        void AcceptConnection(TcpSocket* commandChannelSocket, const ErrorCode& error);

        // Server response handlers
        void ProcessServerResponse(uint8_t* buffer, uint32_t offset, uint32_t length);
        void HandleSucceeded(uint8_t commandCode, uint8_t* data, uint32_t offset, uint32_t length);
        void HandleFailed(uint8_t commandCode, uint8_t* data, uint32_t offset, uint32_t length);
        void HandleMetadataRefresh(const uint8_t* data, uint32_t offset, uint32_t length);
        void HandleDataStartTime(const uint8_t* data, uint32_t offset, uint32_t length);
        void HandleProcessingComplete(const uint8_t* data, uint32_t offset, uint32_t length);
        void HandleUpdateSignalIndexCache(const uint8_t* data, uint32_t offset, uint32_t length);
        void HandleUpdateBaseTimes(uint8_t* data, uint32_t offset, uint32_t length);
        void HandleConfigurationChanged(uint8_t* data, uint32_t offset, uint32_t length);
        void HandleDataPacket(uint8_t* data, uint32_t offset, uint32_t length);
        void ParseTSSCMeasurements(const SignalIndexCachePtr& signalIndexCache, uint8_t* data, uint32_t offset, uint32_t length, std::vector<MeasurementPtr>& measurements);
        void ParseCompactMeasurements(const SignalIndexCachePtr& signalIndexCache, const uint8_t* data, uint32_t offset, uint32_t length, bool includeTime, bool useMillisecondResolution, int64_t frameLevelTimestamp, std::vector<MeasurementPtr>& measurements);
        void HandleBufferBlock(uint8_t* data, uint32_t offset, uint32_t length);

        SignalIndexCache* AddDispatchReference(SignalIndexCachePtr signalIndexCacheRef);
        SignalIndexCachePtr ReleaseDispatchReference(SignalIndexCache* signalIndexCachePtr);

        // Dispatchers
        void Dispatch(const DispatcherFunction& function);
        void Dispatch(const DispatcherFunction& function, const uint8_t* data, uint32_t offset, uint32_t length);
        void DispatchSubscriptionUpdated(SignalIndexCache* signalIndexCache);
        void DispatchStatusMessage(const std::string& message);
        void DispatchErrorMessage(const std::string& message);

        static void StatusMessageDispatcher(DataSubscriber* source, const std::vector<uint8_t>& buffer);
        static void ErrorMessageDispatcher(DataSubscriber* source, const std::vector<uint8_t>& buffer);
        static void DataStartTimeDispatcher(DataSubscriber* source, const std::vector<uint8_t>& buffer);
        static void MetadataDispatcher(DataSubscriber* source, const std::vector<uint8_t>& buffer);
        static void SubscriptionUpdatedDispatcher(DataSubscriber* source, const std::vector<uint8_t>& buffer);
        static void ProcessingCompleteDispatcher(DataSubscriber* source, const std::vector<uint8_t>& buffer);
        static void ConfigurationChangedDispatcher(DataSubscriber* source, const std::vector<uint8_t>& buffer);

        void Connect(const std::string& hostname, uint16_t port, bool autoReconnecting) override;
        void Disconnect(bool joinThread, bool autoReconnecting, bool includeListener);
        bool IsDisconnecting() const { return m_disconnecting || m_disconnected; }

        void SetupConnection();
        void EstablishConnection(const TcpEndPoint& endPoint, bool listening);

        // The connection terminated callback is a special case that
        // must be called on its own separate thread so that it can
        // safely close all sockets and stop all subscriber threads
        // (including the callback thread) before executing the callback.
        void ConnectionTerminatedDispatcher();
        void WaitOnDisconnectThread();
        void HandleSocketError();

    public:
        // Creates a new instance of the data subscriber.
        DataSubscriber();

        // Releases all threads and sockets
        // tied up by the subscriber.
        ~DataSubscriber() noexcept override;

        // Callback registration
        //
        // Callback functions are defined with the following signatures:
        //   void HandleStatusMessage(DataSubscriber* source, const string& message)
        //   void HandleErrorMessage(DataSubscriber* source, const string& message)
        //   void HandleDataStartTime(DataSubscriber* source, int64_t startTime)
        //   void HandleMetadata(DataSubscriber* source, const vector<uint8_t>& metadata)
        //   void HandleSubscriptionUpdated(DataSubscriber* source, const SignalIndexCachePtr& signalIndexCache)
        //   void HandleNewMeasurements(DataSubscriber* source, const vector<MeasurementPtr>& newMeasurements)
        //   void HandleProcessingComplete(DataSubscriber* source, const string& message)
        //   void HandleConfigurationChanged(DataSubscriber* source)
        //   void HandleConnectionEstablished(DataSubscriber* source)
        //   void HandleConnectionTerminated(DataSubscriber* source)
        //   void HandleAutoReconnect(DataClient* source)
        //
        // Metadata is provided to the user as zlib-compressed XML,
        // and must be decompressed and interpreted before it can be used.
        void RegisterStatusMessageCallback(const MessageCallback& statusMessageCallback);
        void RegisterErrorMessageCallback(const MessageCallback& errorMessageCallback);
        void RegisterDataStartTimeCallback(const DataStartTimeCallback& dataStartTimeCallback);
        void RegisterMetadataCallback(const MetadataCallback& metadataCallback);
        void RegisterSubscriptionUpdatedCallback(const SubscriptionUpdatedCallback& subscriptionUpdatedCallback);
        void RegisterNewMeasurementsCallback(const NewMeasurementsCallback& newMeasurementsCallback);
        void RegisterProcessingCompleteCallback(const MessageCallback& processingCompleteCallback);
        void RegisterConfigurationChangedCallback(const ConfigurationChangedCallback& configurationChangedCallback);
        void RegisterConnectionEstablishedCallback(const ConnectionEstablishedCallback& connectionEstablishedCallback);
        void RegisterConnectionTerminatedCallback(const ConnectionTerminatedCallback& connectionTerminatedCallback);
        void RegisterAutoReconnectCallback(const ClientConnectionTerminatedCallback& autoReconnectCallback) override;

        const Guid& GetSubscriberID() const;

        // Gets or sets value that determines whether
        // payload data is compressed using TSSC.
        bool IsPayloadDataCompressed() const;
        void SetPayloadDataCompressed(bool compressed);

        // Gets or sets value that determines whether the
        // metadata transfer is compressed using GZip.
        bool IsMetadataCompressed() const;
        void SetMetadataCompressed(bool compressed);

        // Gets or sets value that determines whether the
        // signal index cache is compressed using GZip.
        bool IsSignalIndexCacheCompressed() const;
        void SetSignalIndexCacheCompressed(bool compressed);

        // Gets or sets STTP protocol version to use.
        uint8_t GetVersion() const;
        void SetVersion(uint8_t version);

        // Gets or sets user defined data reference
        void* GetUserData() const;
        void SetUserData(void* userData);

        SubscriberConnector& GetSubscriberConnector() override;

        void SetSubscriptionInfo(const SubscriptionInfo& info) override;
        const SubscriptionInfo& GetSubscriptionInfo() const;

        // Synchronously connects to publisher.
        void Connect(const std::string& hostname, uint16_t port);

        // Establish a reverse listening connection for subscriber using specified connection info
        void Listen(const sttp::TcpEndPoint& endPoint);
        void Listen(uint16_t port, bool ipV6 = false);                       // Bind to default NIC
        void Listen(const std::string& networkInterfaceIP, uint16_t port);   // Bind to specified NIC IP, format determines IP version

        // Disconnects from the publisher.
        //
        // The method does not return until all connections have been
        // closed and all threads spawned by the subscriber have shut
        // down gracefully (with the exception of the thread that
        // executes the connection terminated callback).
        void Disconnect();

        // Subscribe to measurements to start receiving data.
        void Subscribe();
        void Subscribe(const SubscriptionInfo& info);

        // Cancel the current subscription to stop receiving data.
        void Unsubscribe();

        // Send a command to the server.
        //
        // Command codes can be found in the "Constants.h" header file.
        // For example:
        //   ServerCommand::Connect
        //   ServerCommand::MetadataRefresh
        //   ServerCommand::Subscribe
        //   ServerCommand::Unsubscribe
        void SendServerCommand(uint8_t commandCode);
        void SendServerCommand(uint8_t commandCode, const std::string& message);
        void SendServerCommand(uint8_t commandCode, const uint8_t* data, uint32_t offset, uint32_t length);

        // Convenience method to send the currently defined and/or supported
        // operational modes to the server. Supported operational modes are
        // UTF-8 encoding, common serialization format, and optional metadata compression.
        void SendOperationalModes();

        // Functions for statistics gathering
        uint64_t GetTotalCommandChannelBytesReceived() const;
        uint64_t GetTotalDataChannelBytesReceived() const;
        uint64_t GetTotalMeasurementsReceived() const;

        // Determines if a DataSubscriber is currently connected to a DataPublisher.
        bool IsConnected() override;

        // Determines if a DataSubscriber is currently listening for a DataPublisher
        // connection, i.e., DataSubscriber is in reverse connection mode.
        bool IsListening() const;

        // Waits for publisher response to define operational modes request.
        bool WaitForOperationalModesResponse(int32_t timeout = 5000);

        // Gets user readable status message for current response to define operational modes command request.
        std::string OperationalModesStatus() const;

        // Determines if a DataSubscriber connection has been validated as an STTP connection.
        bool IsValidated() const;

        // Determines if a DataSubscriber is currently subscribed to a data stream.
        bool IsSubscribed() const;

        // Gets the connection identification for the current connection.
        std::string GetConnectionID() const;

        // Version info functions
        void GetAssemblyInfo(std::string& source, std::string& version, std::string& updatedOn) const;
        void SetAssemblyInfo(const std::string& source, const std::string& version, const std::string& updatedOn);

        static const DataSubscriberPtr NullPtr;

        friend class SubscriberConnector;
    };
}
