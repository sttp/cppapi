//******************************************************************************************************
//  DataPublisher.h - Gbtc
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
//  10/25/2018 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "../CommonTypes.h"
#include "../ThreadSafeQueue.h"
#include "../ThreadPool.h"
#include "../data/DataSet.h"
#include "SubscriberConnection.h"
#include "SubscriberConnector.h"
#include "RoutingTables.h"
#include "TransportTypes.h"
#include "Constants.h"

namespace sttp::filterexpressions
{
    class ExpressionTree;
    typedef SharedPtr<ExpressionTree> ExpressionTreePtr;
}

namespace sttp::transport
{
    class DataPublisher;
    typedef SharedPtr<DataPublisher> DataPublisherPtr;

    class DataPublisher final : public DataClient, public EnableSharedThisPtr<DataPublisher> // NOLINT
    {
    public:
        // Function pointer types
        typedef std::function<void(DataPublisher*, const std::vector<uint8_t>&)> DispatcherFunction;
        typedef std::function<void(DataPublisher*, const std::string&)> MessageCallback;
        typedef std::function<void(DataPublisher*, const SubscriberConnectionPtr&)> SubscriberConnectionCallback;
        typedef std::function<void(DataPublisher*, const SubscriberConnectionPtr&, uint8_t, const std::vector<uint8_t>&)> UserCommandCallback;

    private:
        // Structure used to dispatch
        // callbacks on the callback thread.
        struct CallbackDispatcher
        {
            DataPublisher* Source;
            SharedPtr<std::vector<uint8_t>> Data;
            DispatcherFunction Function;

            CallbackDispatcher();
        };

        Guid m_nodeID;
        data::DataSetPtr m_metadata;
        data::DataSetPtr m_filteringMetadata;
        RoutingTables m_routingTables;
        std::unordered_set<SubscriberConnectionPtr> m_subscriberConnections;
        SharedMutex m_subscriberConnectionsLock;
        SecurityMode m_securityMode;
        int32_t m_maximumAllowedConnections;
        bool m_isMetadataRefreshAllowed;
        bool m_isNaNValueFilterAllowed;
        bool m_isNaNValueFilterForced;
        bool m_supportsTemporalSubscriptions;
        bool m_useBaseTimeOffsets;
        uint32_t m_cipherKeyRotationPeriod;
        std::atomic_bool m_reverseConnection;
        std::atomic_bool m_started;
        std::atomic_bool m_shuttingDown;
        std::atomic_bool m_stopped;
        Mutex m_connectActionMutex;
        Mutex m_shutDownThreadMutex;
        Thread m_shutdownThread;
        ThreadPool m_threadPool;
        void* m_userData;

        // Dispatch reference - unordered map needed to manage reference
        // counts on subscriber connections since these are persisted
        std::unordered_map<SubscriberConnectionPtr, uint32_t> m_subscriberConnectionDispatchRefs;
        Mutex m_subscriberConnectionDispatchRefsLock;

        // Callback queue
        Thread m_callbackThread;
        ThreadSafeQueue<CallbackDispatcher> m_callbackQueue;

        // Command channel
        Thread m_commandChannelAcceptThread;
        IOContext m_commandChannelService;
        TcpAcceptor m_clientAcceptor;

        // Command channel handlers
        void StartAccept();
        void AcceptConnection(const SubscriberConnectionPtr& connection, const ErrorCode& error);
        void ConnectionTerminated(const SubscriberConnectionPtr& connection);
        void RemoveConnection(const SubscriberConnectionPtr& connection);

        // Callbacks
        MessageCallback m_statusMessageCallback;
        MessageCallback m_errorMessageCallback;
        SubscriberConnectionCallback m_clientConnectedCallback;
        SubscriberConnectionCallback m_clientDisconnectedCallback;
        SubscriberConnectionCallback m_processingIntervalChangeRequestedCallback;
        SubscriberConnectionCallback m_temporalSubscriptionRequestedCallback;
        SubscriberConnectionCallback m_temporalSubscriptionCanceledCallback;
        UserCommandCallback m_userCommandCallback;
        ClientConnectionTerminatedCallback m_autoReconnectCallback;

        SubscriberConnection* AddDispatchReference(SubscriberConnectionPtr connectionRef);
        SubscriberConnectionPtr ReleaseDispatchReference(SubscriberConnection* connectionPtr);

        // Dispatchers
        void Dispatch(const DispatcherFunction& function);
        void Dispatch(const DispatcherFunction& function, const uint8_t* data, uint32_t offset, uint32_t length);
        void DispatchStatusMessage(const std::string& message);
        void DispatchErrorMessage(const std::string& message);
        void DispatchClientConnected(SubscriberConnection* connection);
        void DispatchClientDisconnected(SubscriberConnection* connection);
        void DispatchProcessingIntervalChangeRequested(SubscriberConnection* connection);
        void DispatchTemporalSubscriptionRequested(SubscriberConnection* connection);
        void DispatchTemporalSubscriptionCanceled(SubscriberConnection* connection);
        void DispatchUserCommand(SubscriberConnection* connection, uint8_t command, const uint8_t* data, uint32_t length);

        static void StatusMessageDispatcher(DataPublisher* source, const std::vector<uint8_t>& buffer);
        static void ErrorMessageDispatcher(DataPublisher* source, const std::vector<uint8_t>& buffer);
        static void ClientConnectedDispatcher(DataPublisher* source, const std::vector<uint8_t>& buffer);
        static void ClientDisconnectedDispatcher(DataPublisher* source, const std::vector<uint8_t>& buffer);
        static void ProcessingIntervalChangeRequestedDispatcher(DataPublisher* source, const std::vector<uint8_t>& buffer);
        static void TemporalSubscriptionRequestedDispatcher(DataPublisher* source, const std::vector<uint8_t>& buffer);
        static void TemporalSubscriptionCanceledDispatcher(DataPublisher* source, const std::vector<uint8_t>& buffer);
        static void UserCommandDispatcher(DataPublisher* source, const std::vector<uint8_t>& buffer);
        static int32_t GetColumnIndex(const data::DataTablePtr& table, const std::string& columnName);

        void WaitOnShutDownThread();
        void ShutDown(bool joinThread);
        bool IsShuttingDown() const { return m_shuttingDown || m_stopped; }

        SubscriberConnectionPtr GetSingleConnection();
        void Connect(const std::string& hostname, uint16_t port, bool autoReconnecting) override;

    public:
        // Creates a new instance of the data publisher.
        DataPublisher();

        // Releases all threads and sockets
        // tied up by the publisher.
        ~DataPublisher() noexcept override;

        // Iterator handler delegates
        typedef std::function<void(SubscriberConnectionPtr, void* userData)> SubscriberConnectionIteratorHandlerFunction;

        // Defines metadata from existing metadata records
        void DefineMetadata(const std::vector<DeviceMetadataPtr>& deviceMetadata, const std::vector<MeasurementMetadataPtr>& measurementMetadata, const std::vector<PhasorMetadataPtr>& phasorMetadata, int32_t versionNumber = 0);

        // Defines metadata from an existing dataset
        void DefineMetadata(const data::DataSetPtr& metadata);

        // Gets primary metadata. This dataset contains all the normalized metadata tables that define
        // the available detail about the data points that can be subscribed to by clients.
        const data::DataSetPtr& GetMetadata() const;

        // Gets filtering metadata. This dataset, derived from primary metadata, contains a flattened
        // table used to subscribe to a filtered set of points with an expression, e.g.:
        // FILTER ActiveMeasurements WHERE SignalType LIKE '%PHA'
        const data::DataSetPtr& GetFilteringMetadata() const;

        // Filters primary MeasurementDetail metadata returning values as measurement metadata records
        std::vector<MeasurementMetadataPtr> FilterMetadata(const std::string& filterExpression) const;

        // Starts DataPublisher using specified connection info.
        void Start(const TcpEndPoint& endpoint);
        void Start(uint16_t port, bool ipV6 = false);                       // Bind to default NIC
        void Start(const std::string& networkInterfaceIP, uint16_t port);   // Bind to specified NIC IP, format determines IP version

        // Synchronously establishes reverse connection to a DataSubscriber.
        void Connect(const std::string& hostname, uint16_t port);

        // Get subscriber connector used to assign settings for in reverse connection mode.
        SubscriberConnector& GetSubscriberConnector() override;

        // Determines if a DataPublisher is currently connected to a DataSubscriber for reverse connection mode.
        bool IsConnected() override;

        // Determines if DataPublisher is setup in reverse connection mode.
        bool IsReverseConnection() const;

        // Shuts down DataPublisher
        //
        // The method does not return until all connections have been closed
        // and all threads spawned by the publisher have shut down gracefully
        void Stop();

        // Determines if DataPublisher has been started
        bool IsStarted() const;

        void PublishMeasurements(const std::vector<Measurement>& measurements);
        void PublishMeasurements(const std::vector<MeasurementPtr>& measurements);

        // Node ID defines a unique identification for the DataPublisher
        // instance that gets included in published metadata so that clients
        // can easily distinguish the source of the measurements
        const Guid& GetNodeID() const;
        void SetNodeID(const Guid& value);

        SecurityMode GetSecurityMode() const;
        void SetSecurityMode(SecurityMode value);

        // Gets or sets value that defines the maximum number of allowed connections, -1 = no limit
        int32_t GetMaximumAllowedConnections() const;
        void SetMaximumAllowedConnections(int32_t value);

        // Gets or sets flag that determines if metadata refresh is allowed by subscribers
        bool GetIsMetadataRefreshAllowed() const;
        void SetIsMetadataRefreshAllowed(bool value);

        // Gets or sets flag that determines if NaN value filter is allowed by subscribers
        bool GetIsNaNValueFilterAllowed() const;
        void SetNaNValueFilterAllowed(bool value);

        // Gets or sets flag that determines if NaN value filter is forced by publisher, regardless of subscriber request
        bool GetIsNaNValueFilterForced() const;
        void SetIsNaNValueFilterForced(bool value);

        bool GetSupportsTemporalSubscriptions() const;
        void SetSupportsTemporalSubscriptions(bool value);

        uint32_t GetCipherKeyRotationPeriod() const;
        void SetCipherKeyRotationPeriod(uint32_t value);

        // Gets or sets flag that determines if base time offsets should be used in compact format
        bool GetUseBaseTimeOffsets() const;
        void SetUseBaseTimeOffsets(bool value);

        uint16_t GetPort() const;
        bool IsIPv6() const;

        // Gets or sets user defined data reference
        void* GetUserData() const;
        void SetUserData(void* userData);

        // Statistical functions
        uint64_t GetTotalCommandChannelBytesSent();
        uint64_t GetTotalDataChannelBytesSent();
        uint64_t GetTotalMeasurementsSent();

        // Callback registration
        //
        // Callback functions are defined with the following signatures:
        //   void HandleStatusMessage(DataPublisher* source, const string& message)
        //   void HandleErrorMessage(DataPublisher* source, const string& message)
        //   void HandleClientConnected(DataPublisher* source, const SubscriberConnectionPtr& connection);
        //   void HandleClientDisconnected(DataPublisher* source, const SubscriberConnectionPtr& connection);
        //   void HandleProcessingIntervalChangeRequested(DataPublisher* source, const SubscriberConnectionPtr& connection);
        //   void HandleTemporalSubscriptionRequested(DataPublisher* source, const SubscriberConnectionPtr& connection);
        //   void HandleTemporalSubscriptionCanceled(DataPublisher* source, const SubscriberConnectionPtr& connection);
        //   void HandleUserCommand(DataPublisher* source, const SubscriberConnectionPtr& connection, uint32_t command, const std::vector<uint8_t>& buffer);
        //   void HandleAutoReconnect(DataClient* source)
        void RegisterStatusMessageCallback(const MessageCallback& statusMessageCallback);
        void RegisterErrorMessageCallback(const MessageCallback& errorMessageCallback);
        void RegisterClientConnectedCallback(const SubscriberConnectionCallback& clientConnectedCallback);
        void RegisterClientDisconnectedCallback(const SubscriberConnectionCallback& clientDisconnectedCallback);
        void RegisterProcessingIntervalChangeRequestedCallback(const SubscriberConnectionCallback& processingIntervalChangeRequestedCallback);
        void RegisterTemporalSubscriptionRequestedCallback(const SubscriberConnectionCallback& temporalSubscriptionRequestedCallback);
        void RegisterTemporalSubscriptionCanceledCallback(const SubscriberConnectionCallback& temporalSubscriptionCanceledCallback);
        void RegisterUserCommandCallback(const UserCommandCallback& userCommandCallback);
        void RegisterAutoReconnectCallback(const ClientConnectionTerminatedCallback& autoReconnectCallback) override;

        // SubscriberConnection iteration function - note that full lock will be maintained on source collection
        // for the entire call, so keep work time minimized or clone collection before work
        void IterateSubscriberConnections(const SubscriberConnectionIteratorHandlerFunction& iteratorHandler, void* userData);

        void DisconnectSubscriber(const SubscriberConnectionPtr& connection);
        void DisconnectSubscriber(const Guid& instanceID);

        static const DataPublisherPtr NullPtr;

        friend class SubscriberConnection;
        friend class PublisherInstance;
    };
}
