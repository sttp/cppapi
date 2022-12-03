//******************************************************************************************************
//  SubscriberConnector.h - Gbtc
//
//  Copyright © 2022, Grid Protection Alliance.  All Rights Reserved.
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
//  12/02/2022 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "../Timer.h"
#include "SubscriptionInfo.h"

namespace sttp::transport
{
    class SubscriberConnector;

    class DataClient // NOLINT
    {
    public:
        typedef std::function<void(DataClient*)> ClientConnectionTerminatedCallback;

        DataClient();
        virtual ~DataClient() = 0;
        virtual SubscriberConnector& GetSubscriberConnector() = 0;
        virtual void Connect(const std::string& hostname, uint16_t port, bool autoReconnecting) = 0;
        virtual void SetSubscriptionInfo(const SubscriptionInfo& info);
        virtual void RegisterAutoReconnectCallback(const ClientConnectionTerminatedCallback& autoReconnectCallback) = 0;
        virtual bool IsConnected() const = 0;

        std::atomic_bool m_disposing;
    };

    class DataSubscriber;
    class DataPublisher;

    // Helper class to provide retry and auto-reconnect functionality to the subscriber.
    class SubscriberConnector final // NOLINT
    {
    public:
        typedef std::function<void(DataClient*, const std::string&)> ClientErrorMessageCallback;
        typedef std::function<void(DataSubscriber*, const std::string&)> SubscriberErrorMessageCallback;
        typedef std::function<void(DataPublisher*, const std::string&)> PublisherErrorMessageCallback;

        typedef std::function<void(DataClient*)> ClientReconnectCallback;
        typedef std::function<void(DataSubscriber*)> SubscriberReconnectCallback;
        typedef std::function<void(DataPublisher*)> PublisherReconnectCallback;

    private:
        ClientErrorMessageCallback m_clientErrorMessageCallback;
        SubscriberErrorMessageCallback m_subscriberErrorMessageCallback;
        PublisherErrorMessageCallback m_publisherErrorMessageCallback;

        ClientReconnectCallback m_clientReconnectCallback;
        SubscriberReconnectCallback m_subscriberReconnectCallback;
        PublisherReconnectCallback m_publisherReconnectCallback;

        std::string m_hostname;
        uint16_t m_port;
        TimerPtr m_timer;

        int32_t m_maxRetries;
        int32_t m_retryInterval;
        int32_t m_maxRetryInterval;
        int32_t m_connectAttempt;
        bool m_connectionRefused;
        bool m_autoReconnect;
        Thread m_autoReconnectThread;
        std::atomic_bool m_cancel;

        // Auto-reconnect handler.
        static void AutoReconnect(DataClient* client);

        int Connect(DataClient& client, bool autoReconnecting);

    public:
        static constexpr int ConnectSuccess = 1;
        static constexpr int ConnectFailed = 0;
        static constexpr int ConnectCanceled = -1;

        // Creates a new instance.
        SubscriberConnector();
        ~SubscriberConnector() noexcept;

        // Registers a callback to provide error messages each time
        // the subscriber fails to connect during a connection sequence.
        void RegisterErrorMessageCallback(const ClientErrorMessageCallback& errorMessageCallback);
        void RegisterErrorMessageCallback(const SubscriberErrorMessageCallback& errorMessageCallback);
        void RegisterErrorMessageCallback(const PublisherErrorMessageCallback& errorMessageCallback);

        // Registers a callback to notify after an automatic reconnection attempt has been made.
        // This callback will be called whether the connection was successful or not, so it is
        // recommended to check the connected state of the subscriber using the IsConnected() method.
        void RegisterReconnectCallback(const ClientReconnectCallback& reconnectCallback);
        void RegisterReconnectCallback(const SubscriberReconnectCallback& reconnectCallback);
        void RegisterReconnectCallback(const PublisherReconnectCallback& reconnectCallback);

        // Begin connection sequence
        int Connect(DataClient& client, const SubscriptionInfo& info);

        // Cancel all current and
        // future connection sequences.
        void Cancel();

        // Sets the hostname of the publisher to connect to.
        void SetHostname(const std::string& hostname);

        // Sets the port that the publisher is listening on.
        void SetPort(uint16_t port);

        // Sets the maximum number of retries during a connection sequence.
        void SetMaxRetries(int32_t maxRetries);

        // Sets the initial interval of idle time (in milliseconds) between connection attempts.
        void SetRetryInterval(int32_t retryInterval);

        // Sets maximum retry interval - connection retry attempts use exponential back-off
        // algorithm up to this defined maximum.
        void SetMaxRetryInterval(int32_t maxRetryInterval);

        // Sets flag that determines whether the subscriber should
        // automatically attempt to reconnect when the connection is terminated.
        void SetAutoReconnect(bool autoReconnect);

        // Sets flag indicating connection was refused.
        void SetConnectionRefused(bool connectionRefused);

        // Resets connector for a new connection
        void ResetConnection();

        // Getters for configurable settings.
        std::string GetHostname() const;
        uint16_t GetPort() const;
        int32_t GetMaxRetries() const;
        int32_t GetRetryInterval() const;
        int32_t GetMaxRetryInterval() const;
        bool GetAutoReconnect() const;
        bool GetConnectionRefused() const;
    };
}