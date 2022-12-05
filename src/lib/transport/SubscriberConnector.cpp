//******************************************************************************************************
//  SubscriberConnector.cpp - Gbtc
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
//  12/02/2022 - rcarroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include "SubscriberConnector.h"
#include "TransportTypes.h"
#include "DataSubscriber.h"
#include "DataPublisher.h"

using namespace std;
using namespace boost;
using namespace sttp::transport;

DataClient::DataClient() :
    m_disposing(false)
{
}

DataClient::~DataClient()
{
}

void DataClient::SetSubscriptionInfo(const SubscriptionInfo& info)
{
    // For publisher implementation of DataClient, this is has no meaning - so default
    // implementation is empty function
}

SubscriberConnector::SubscriberConnector() :
    m_port(0),
    m_timer(nullptr),
    m_maxRetries(-1),
    m_retryInterval(2000),
    m_maxRetryInterval(120000),
    m_connectAttempt(0),
    m_connectionRefused(false),
    m_autoReconnect(true),
    m_cancel(false)
{
}

SubscriberConnector::~SubscriberConnector() noexcept
{
    try
    {
        Cancel();
    }
    catch (...)
    {
        // ReSharper disable once CppRedundantControlFlowJump
        return;
    }
}

// Auto-reconnect handler.
void SubscriberConnector::AutoReconnect(DataClient* client)
{
    SubscriberConnector& connector = client->GetSubscriberConnector();

    if (connector.m_cancel || client->m_disposing)
        return;

    // Make sure to wait on any running reconnect to complete...
    connector.m_autoReconnectThread.join();

    connector.m_autoReconnectThread = Thread([&connector, client]
    {
        // Reset connection attempt counter if last attempt was not refused
        if (!connector.m_connectionRefused)
            connector.ResetConnection();

        if (connector.m_maxRetries != -1 && connector.m_connectAttempt >= connector.m_maxRetries)
        {
            if (connector.m_clientErrorMessageCallback != nullptr)
                connector.m_clientErrorMessageCallback(client, "Maximum connection retries attempted. Auto-reconnect canceled.");
            return;
        }

        // Apply exponential back-off algorithm for retry attempt delays
        const int32_t exponent = (connector.m_connectAttempt > 13 ? 13 : connector.m_connectAttempt) - 1;
        int32_t retryInterval = connector.m_connectAttempt > 0 ? connector.m_retryInterval * static_cast<int32_t>(pow(2, exponent)) : 0;

        if (retryInterval > connector.m_maxRetryInterval)
            retryInterval = connector.m_maxRetryInterval;

        // Notify the user that we are attempting to reconnect.
        if (connector.m_clientErrorMessageCallback != nullptr)
        {
            stringstream errorMessageStream;

            errorMessageStream << "Connection";

            if (connector.m_connectAttempt > 0)
                errorMessageStream << " attempt " << connector.m_connectAttempt + 1;

            errorMessageStream << " to \"" << connector.m_hostname << ":" << connector.m_port << "\" was terminated. ";

            if (retryInterval > 0)
                errorMessageStream << "Attempting to reconnect in " << retryInterval / 1000.0 << " seconds...";
            else
                errorMessageStream << "Attempting to reconnect...";

            connector.m_clientErrorMessageCallback(client, errorMessageStream.str());
        }

        connector.m_timer = Timer::WaitTimer(retryInterval);
        connector.m_timer->Wait();

        if (connector.m_cancel || client->m_disposing)
            return;

        if (connector.Connect(*client, true) == ConnectCanceled)
            return;

        // Notify the user that reconnect attempt was completed.
        if (!connector.m_cancel && connector.m_clientReconnectCallback != nullptr)
            connector.m_clientReconnectCallback(client);
    });
}

// Registers a callback to provide error messages each time
// the client fails to connect during a connection sequence.
void SubscriberConnector::RegisterErrorMessageCallback(const ClientErrorMessageCallback& errorMessageCallback)
{
    m_clientErrorMessageCallback = errorMessageCallback;
}

void SubscriberConnector::RegisterErrorMessageCallback(const SubscriberErrorMessageCallback& errorMessageCallback)
{
    m_subscriberErrorMessageCallback = errorMessageCallback;
    m_clientErrorMessageCallback = [this](DataClient* client, const string& message)
    {
        m_subscriberErrorMessageCallback(dynamic_cast<DataSubscriber*>(client), message);
    };
}

void SubscriberConnector::RegisterErrorMessageCallback(const PublisherErrorMessageCallback& errorMessageCallback)
{
    m_publisherErrorMessageCallback = errorMessageCallback;
    m_clientErrorMessageCallback = [this](DataClient* client, const string& message)
    {
        m_publisherErrorMessageCallback(dynamic_cast<DataPublisher*>(client), message);
    };
}

// Registers a callback to notify after an automatic reconnection attempt has been made.
void SubscriberConnector::RegisterReconnectCallback(const ClientReconnectCallback& reconnectCallback)
{
    m_clientReconnectCallback = reconnectCallback;
}

void SubscriberConnector::RegisterReconnectCallback(const SubscriberReconnectCallback& reconnectCallback)
{
    m_subscriberReconnectCallback = reconnectCallback;
    m_clientReconnectCallback = [this](DataClient* client)
    {
        m_subscriberReconnectCallback(dynamic_cast<DataSubscriber*>(client));
    };
}

void SubscriberConnector::RegisterReconnectCallback(const PublisherReconnectCallback& reconnectCallback)
{
    m_publisherReconnectCallback = reconnectCallback;
    m_clientReconnectCallback = [this](DataClient* client)
    {
        m_publisherReconnectCallback(dynamic_cast<DataPublisher*>(client));
    };
}

int SubscriberConnector::Connect(DataClient& client, const SubscriptionInfo& info)
{
    if (m_cancel)
        return ConnectCanceled;

    client.SetSubscriptionInfo(info);
    return Connect(client, false);
}

// Begin connection sequence.
int SubscriberConnector::Connect(DataClient& client, bool autoReconnecting)
{
    if (m_autoReconnect)
        client.RegisterAutoReconnectCallback(&AutoReconnect);

    m_cancel = false;

    while (!client.m_disposing)
    {
        if (m_maxRetries != -1 && m_connectAttempt >= m_maxRetries)
        {
            if (m_clientErrorMessageCallback != nullptr)
                m_clientErrorMessageCallback(&client, "Maximum connection retries attempted. Auto-reconnect canceled.");
            break;
        }

        string errorMessage;
        bool connected = false;

        try
        {
            m_connectAttempt++;

            if (client.m_disposing)
                return ConnectCanceled;

            client.Connect(m_hostname, m_port, autoReconnecting);

            connected = true;
            break;
        }
        catch (SubscriberException& ex)
        {
            errorMessage = ex.what();
        }
        catch (PublisherException& ex)
        {
            errorMessage = ex.what();
        }
        catch (SystemError& ex)
        {
            errorMessage = ex.what();
        }
        catch (...)
        {
            errorMessage = current_exception_diagnostic_information(true);
        }

        if (!connected && !client.m_disposing)
        {
            // Apply exponential back-off algorithm for retry attempt delays
            const int32_t exponent = (m_connectAttempt > 13 ? 13 : m_connectAttempt) - 1;
            int32_t retryInterval = m_connectAttempt > 0 ? m_retryInterval * static_cast<int32_t>(pow(2, exponent)) : 0;
            autoReconnecting = true;

            if (retryInterval > m_maxRetryInterval)
                retryInterval = m_maxRetryInterval;

            if (m_clientErrorMessageCallback != nullptr)
            {
                stringstream errorMessageStream;
                errorMessageStream << "Connection";

                if (m_connectAttempt > 0)
                    errorMessageStream << " attempt " << m_connectAttempt + 1;

                errorMessageStream << " to \"" << m_hostname << ":" << m_port << "\" failed: " << errorMessage << ". ";

                if (retryInterval > 0)
                    errorMessageStream << "Attempting to reconnect in " << retryInterval / 1000.0 << " seconds...";
                else
                    errorMessageStream << "Attempting to reconnect...";

                m_clientErrorMessageCallback(&client, errorMessageStream.str());
            }

            if (retryInterval > 0)
            {
                m_timer = Timer::WaitTimer(retryInterval);
                m_timer->Wait();

                if (m_cancel)
                    return ConnectCanceled;
            }
        }
    }

    return client.m_disposing ? ConnectCanceled : client.IsConnected() ? ConnectSuccess : ConnectFailed;
}

// Cancel all current and future connection sequences.
void SubscriberConnector::Cancel()
{
    m_cancel = true;

    // Cancel any waiting timer operations by setting immediate timer expiration
    if (m_timer != nullptr)
        m_timer->Stop();

    m_autoReconnectThread.join();
}

// Sets the hostname of the publisher to connect to.
void SubscriberConnector::SetHostname(const string& hostname)
{
    m_hostname = hostname;
}

// Sets the port that the publisher is listening on.
void SubscriberConnector::SetPort(const uint16_t port)
{
    m_port = port;
}

// Sets the maximum number of retries during a connection sequence.
void SubscriberConnector::SetMaxRetries(const int32_t maxRetries)
{
    m_maxRetries = maxRetries;
}

// Sets the interval of idle time (in milliseconds) between connection attempts.
void SubscriberConnector::SetRetryInterval(const int32_t retryInterval)
{
    m_retryInterval = retryInterval;
}

// Sets maximum retry interval - connection retry attempts use exponential
// back-off algorithm up to this defined maximum.
void SubscriberConnector::SetMaxRetryInterval(const int32_t maxRetryInterval)
{
    m_maxRetryInterval = maxRetryInterval;
}

// Sets the flag that determines whether the client should
// automatically attempt to reconnect when the connection is terminated.
void SubscriberConnector::SetAutoReconnect(const bool autoReconnect)
{
    m_autoReconnect = autoReconnect;
}

// Sets flag indicating connection was refused.
void SubscriberConnector::SetConnectionRefused(const bool connectionRefused)
{
    m_connectionRefused = connectionRefused;
}

// Resets connector for a new connection
void SubscriberConnector::ResetConnection()
{
    m_connectAttempt = 0;
    m_cancel = false;
}

// Gets the hostname of the publisher to connect to.
string SubscriberConnector::GetHostname() const
{
    return m_hostname;
}

// Gets the port that the publisher is listening on.
uint16_t SubscriberConnector::GetPort() const
{
    return m_port;
}

// Gets the maximum number of retries during a connection sequence.
int32_t SubscriberConnector::GetMaxRetries() const
{
    return m_maxRetries;
}

// Gets the interval of idle time between connection attempts.
int32_t SubscriberConnector::GetRetryInterval() const
{
    return m_retryInterval;
}

// Gets maximum retry interval - connection retry attempts use exponential
// back-off algorithm up to this defined maximum.
int32_t SubscriberConnector::GetMaxRetryInterval() const
{
    return m_maxRetryInterval;
}

// Gets the flag that determines whether the client should
// automatically attempt to reconnect when the connection is terminated.
bool SubscriberConnector::GetAutoReconnect() const
{
    return m_autoReconnect;
}

// Gets flag indicating connection was refused.
bool SubscriberConnector::GetConnectionRefused() const
{
    return m_connectionRefused;
}
