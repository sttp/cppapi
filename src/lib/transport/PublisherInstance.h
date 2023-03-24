//******************************************************************************************************
//  PublisherInstance.h - Gbtc
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
//  12/05/2018 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "DataPublisher.h"

namespace sttp::transport
{
    class PublisherInstance // NOLINT
    {
    private:
        // Publication members
        DataPublisherPtr m_publisher;
        std::string m_hostname;
        uint16_t m_port;
        int32_t m_maxRetries;
        int32_t m_retryInterval;
        int32_t m_maxRetryInterval;
        bool m_autoReconnect;
        sttp::Thread m_connectThread;
        void* m_userData;

        // Internal subscription event handlers
        static void HandleReconnect(const DataPublisher* source);
        static void HandleStatusMessage(const DataPublisher* source, const std::string& message);
        static void HandleErrorMessage(const DataPublisher* source, const std::string& message);
        static void HandleClientConnected(const DataPublisher* source, const SubscriberConnectionPtr& connection);
        static void HandleClientDisconnected(const DataPublisher* source, const SubscriberConnectionPtr& connection);
        static void HandleProcessingIntervalChangeRequested(const DataPublisher* source, const SubscriberConnectionPtr& connection);
        static void HandleTemporalSubscriptionRequested(const DataPublisher* source, const SubscriberConnectionPtr& connection);
        static void HandleTemporalSubscriptionCanceled(const DataPublisher* source, const SubscriberConnectionPtr& connection);
        static void HandleReceivedUserCommand(const DataPublisher* source, const SubscriberConnectionPtr& connection, uint8_t command, const std::vector<uint8_t>& buffer);

#ifdef SWIG
    public:
#else
    protected:
#endif
        virtual void SetupSubscriberConnector(SubscriberConnector& connector);
        virtual void StatusMessage(const std::string& message);   // Defaults output to cout
        virtual void ErrorMessage(const std::string& message);    // Defaults output to cerr
        virtual void ClientConnected(const SubscriberConnectionPtr& connection);
        virtual void ClientDisconnected(const SubscriberConnectionPtr& connection);
        virtual void ProcessingIntervalChangeRequested(const SubscriberConnectionPtr& connection);
        virtual void TemporalSubscriptionRequested(const SubscriberConnectionPtr& connection);
        virtual void TemporalSubscriptionCanceled(const SubscriberConnectionPtr& connection);
        virtual void HandleUserCommand(const SubscriberConnectionPtr& connection, uint8_t command, const std::vector<uint8_t>& buffer);

    public:
        PublisherInstance();
        virtual ~PublisherInstance();

        // Define metadata from existing metadata tables
        void DefineMetadata(const std::vector<DeviceMetadataPtr>& deviceMetadata, const std::vector<MeasurementMetadataPtr>& measurementMetadata, const std::vector<PhasorMetadataPtr>& phasorMetadata, int32_t versionNumber = 0) const;

        // Define metadata from existing dataset
        void DefineMetadata(const sttp::data::DataSetPtr& metadata) const;

        // Gets primary metadata. This dataset contains all the normalized metadata tables that define
        // the available detail about the data points that can be subscribed to by clients.
        const sttp::data::DataSetPtr& GetMetadata() const;

        // Gets filtering metadata. This dataset, derived from primary metadata, contains a flattened
        // table used to subscribe to a filtered set of points with an expression, e.g.:
        // FILTER ActiveMeasurements WHERE SignalType LIKE '%PHA'
        const sttp::data::DataSetPtr& GetFilteringMetadata() const;

        // Filters primary MeasurementDetail metadata returning values as measurement metadata records
        std::vector<MeasurementMetadataPtr> FilterMetadata(const std::string& filterExpression) const;

        // Starts publisher using specified connection info in listening connection mode
        // Returns true if publisher was successfully started
        // Throws PublisherException for implementation logic errors
        virtual bool Start(const sttp::TcpEndPoint& endPoint);
        virtual bool Start(uint16_t port, bool ipV6 = false);                       // Bind to default NIC
        virtual bool Start(const std::string& networkInterfaceIP, uint16_t port);   // Bind to specified NIC IP, format determines IP version
        
        // Shuts down publisher, listening or reverse connection mode
        virtual void Stop();

        // Initialize publisher as a reverse connection with host name, port - this function must be
        // called before the Connect method is called
        void Initialize(const std::string& hostname, uint16_t port);

        // Gets or sets flag that determines if auto-reconnect is enabled for reverse connection mode
        bool GetAutoReconnect() const;
        void SetAutoReconnect(bool autoReconnect);

        // Gets or sets maximum connection retries for reverse connection mode - set to -1 for infinite
        int32_t GetMaxRetries() const;
        void SetMaxRetries(int32_t maxRetries);

        // Gets or sets initial delay, in milliseconds, between connection retries for reverse connection
        // mode - delays will be an increasing multiple of this interval at each connection retry
        int32_t GetRetryInterval() const;
        void SetRetryInterval(int32_t retryInterval);

        // Gets or sets maximum retry interval, in milliseconds, for reverse connection mode - retry
        // attempt intervals use exponential back-off algorithm up to this defined maximum
        int32_t GetMaxRetryInterval() const;
        void SetMaxRetryInterval(int32_t maxRetryInterval);

        // Synchronously connects to an STTP subscriber using a reverse connection
        // This establishes an automatic reconnect cycle when GetAutoReconnect is true
        // Returns true if publisher was successfully connected; otherwise, false
        // Throws PublisherException for implementation logic errors
        virtual bool Connect();

        // Asynchronously connects to an STTP subscriber using a reverse connection
        // This establishes an automatic reconnect cycle when GetAutoReconnect is true
        // Throws PublisherException for implementation logic errors
        void ConnectAsync();

        // Determines if publisher is currently connected to a subscriber for reverse connection mode
        bool IsConnected() const;

        // Determines if publisher is setup in reverse connection mode
        bool IsReverseConnection() const;

        // Determines if publisher has been started, listening or reverse connection mode
        bool IsStarted() const;

        void PublishMeasurements(const SimpleMeasurement* measurements, int32_t count) const;
        void PublishMeasurements(const std::vector<Measurement>& measurements) const;
        void PublishMeasurements(const std::vector<MeasurementPtr>& measurements) const;

        // Node ID defines a unique identification for the publisher
        // instance that gets included in published metadata so that clients
        // can easily distinguish the source of the measurements
        const sttp::Guid& GetNodeID() const;
        void SetNodeID(const sttp::Guid& nodeID) const;

        SecurityMode GetSecurityMode() const;
        void SetSecurityMode(SecurityMode securityMode) const;

        int32_t GetMaximumAllowedConnections() const;
        void SetMaximumAllowedConnections(int32_t value) const;

        bool IsMetadataRefreshAllowed() const;
        void SetMetadataRefreshAllowed(bool allowed) const;

        bool IsNaNValueFilterAllowed() const;
        void SetNaNValueFilterAllowed(bool allowed) const;

        bool IsNaNValueFilterForced() const;
        void SetNaNValueFilterForced(bool forced) const;

        bool GetSupportsTemporalSubscriptions() const;
        void SetSupportsTemporalSubscriptions(bool value) const;

        uint32_t GetCipherKeyRotationPeriod() const;
        void SetCipherKeyRotationPeriod(uint32_t period) const;

        uint16_t GetPort() const;
        bool IsIPv6() const;

        // Gets or sets user defined data reference for PublisherInstance
        void* GetUserData() const;
        void SetUserData(void* userData);

        // Statistical functions
        uint64_t GetTotalCommandChannelBytesSent() const;
        uint64_t GetTotalDataChannelBytesSent() const;
        uint64_t GetTotalMeasurementsSent() const;

        // Safely get list of subscriber connections. Vector will be cleared then appended to,
        // returns true if any connections were added
        bool TryGetSubscriberConnections(std::vector<SubscriberConnectionPtr>& subscriberConnections) const;

        void DisconnectSubscriber(const SubscriberConnectionPtr& connection) const;
        void DisconnectSubscriber(const sttp::Guid& instanceID) const;
    };
}
