//******************************************************************************************************
//  PublisherInstance.cpp - Gbtc
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
//  12/05/2018 - Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include "PublisherInstance.h"
#include "../Convert.h"

using namespace std;
using namespace pugi;
using namespace sttp;
using namespace sttp::data;
using namespace sttp::transport;
using namespace boost::asio::ip;

PublisherInstance::PublisherInstance() :
    m_hostname("localhost"),
    m_port(7165),
    m_maxRetries(-1),
    m_retryInterval(2000),
    m_maxRetryInterval(120000),
    m_autoReconnect(true),
    m_userData(nullptr)
{
    // Reference this PublisherInstance in DataPublisher user data
    m_publisher = NewSharedPtr<DataPublisher>();
    m_publisher->SetUserData(this);

    // Register callbacks
    m_publisher->RegisterStatusMessageCallback(&HandleStatusMessage);
    m_publisher->RegisterErrorMessageCallback(&HandleErrorMessage);
    m_publisher->RegisterClientConnectedCallback(&HandleClientConnected);
    m_publisher->RegisterClientDisconnectedCallback(&HandleClientDisconnected);
    m_publisher->RegisterProcessingIntervalChangeRequestedCallback(&HandleProcessingIntervalChangeRequested);
    m_publisher->RegisterTemporalSubscriptionRequestedCallback(&HandleTemporalSubscriptionRequested);
    m_publisher->RegisterTemporalSubscriptionCanceledCallback(&HandleTemporalSubscriptionCanceled);
    m_publisher->RegisterUserCommandCallback(&HandleReceivedUserCommand);
}

PublisherInstance::~PublisherInstance()
{
    try
    {
        if (m_publisher != nullptr)
            m_publisher->ShutDown(true);
    }
    catch (...)
    {
    }

    try
    {
        m_connectThread.join();
    }
    catch (...)
    {
        // ReSharper disable once CppRedundantControlFlowJump
        return;
    }
}

// private functions

// The following member methods are defined as static so they
// can be used as callback registrations for DataPublisher

void PublisherInstance::HandleReconnect(const DataPublisher* source)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    if (instance->IsConnected())
    {
        instance->ClientConnected(instance->m_publisher->GetSingleConnection());
    }
    else
    {
        instance->Stop();
        instance->StatusMessage("Connection retry attempts exceeded.");
    }
}

void PublisherInstance::HandleStatusMessage(const DataPublisher* source, const string& message)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->StatusMessage(message);
}

void PublisherInstance::HandleErrorMessage(const DataPublisher* source, const string& message)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->ErrorMessage(message);
}

void PublisherInstance::HandleClientConnected(const DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->ClientConnected(connection);
}

void PublisherInstance::HandleClientDisconnected(const DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->ClientDisconnected(connection);
}

void PublisherInstance::HandleProcessingIntervalChangeRequested(const DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->ProcessingIntervalChangeRequested(connection);
}

void PublisherInstance::HandleTemporalSubscriptionRequested(const DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->TemporalSubscriptionRequested(connection);
}

void PublisherInstance::HandleTemporalSubscriptionCanceled(const DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->TemporalSubscriptionCanceled(connection);
}

void PublisherInstance::HandleReceivedUserCommand(const DataPublisher* source, const SubscriberConnectionPtr& connection, const uint8_t command, const std::vector<uint8_t>& buffer)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->HandleUserCommand(connection, command, buffer);
}

// protected functions

// The following member methods are overridable by derived classes
// to allow for functionality customization

void PublisherInstance::SetupSubscriberConnector(SubscriberConnector& connector)
{
    // SubscriberConnector is another helper object which allows the
    // user to modify settings for auto-reconnects and retry cycles.

    // Register callbacks
    connector.RegisterErrorMessageCallback(&HandleErrorMessage);
    connector.RegisterReconnectCallback(&HandleReconnect);

    connector.SetHostname(m_hostname);
    connector.SetPort(m_port);
    connector.SetMaxRetries(m_maxRetries);
    connector.SetRetryInterval(m_retryInterval);
    connector.SetMaxRetryInterval(m_maxRetryInterval);
    connector.SetAutoReconnect(m_autoReconnect);
}

void PublisherInstance::StatusMessage(const string& message)
{
    cout << message << endl << endl;
}

void PublisherInstance::ErrorMessage(const string& message)
{
    cerr << message << endl << endl;
}

void PublisherInstance::ClientConnected(const SubscriberConnectionPtr& connection)
{
    stringstream message;

    if (IsReverseConnection())
        message << "Reverse connection to client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " established...";
    else
        message << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " connected...";

    StatusMessage(message.str());
}

void PublisherInstance::ClientDisconnected(const SubscriberConnectionPtr& connection)
{
    stringstream message;

    if (IsReverseConnection())
        message << "Reverse connection to client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " disconnected...";
    else
        message << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " disconnected...";

    StatusMessage(message.str());
}

void PublisherInstance::ProcessingIntervalChangeRequested(const SubscriberConnectionPtr& connection)
{
    stringstream message;

    message << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " has requested to change its temporal processing interval to " << ToString(connection->GetProcessingInterval()) << "ms";

    StatusMessage(message.str());
}

void PublisherInstance::TemporalSubscriptionRequested(const SubscriberConnectionPtr& connection)
{
    stringstream message;

    message << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " has requested a temporal subscription starting at " << ToString(connection->GetStartTimeConstraint());

    StatusMessage(message.str());
}

void PublisherInstance::TemporalSubscriptionCanceled(const SubscriberConnectionPtr& connection)
{
    stringstream message;

    message << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " has canceled the temporal subscription starting at " << ToString(connection->GetStartTimeConstraint());

    StatusMessage(message.str());
}

void PublisherInstance::HandleUserCommand(const SubscriberConnectionPtr& connection, const uint8_t command, const std::vector<uint8_t>& buffer)
{
    stringstream message;

    message << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " sent user-defined command \"" << ServerCommand::ToString(command) << "\" with " << buffer.size() << " bytes of payload";

    StatusMessage(message.str());
}

// public functions

void PublisherInstance::DefineMetadata(const vector<DeviceMetadataPtr>& deviceMetadata, const vector<MeasurementMetadataPtr>& measurementMetadata, const vector<PhasorMetadataPtr>& phasorMetadata, const int32_t versionNumber) const
{
    m_publisher->DefineMetadata(deviceMetadata, measurementMetadata, phasorMetadata, versionNumber);
}

void PublisherInstance::DefineMetadata(const DataSetPtr& metadata) const
{
    m_publisher->DefineMetadata(metadata);
}

const DataSetPtr& PublisherInstance::GetMetadata() const
{
    return m_publisher->GetMetadata();
}

const DataSetPtr& PublisherInstance::GetFilteringMetadata() const
{
    return m_publisher->GetFilteringMetadata();
}

vector<MeasurementMetadataPtr> PublisherInstance::FilterMetadata(const string& filterExpression) const
{
    return m_publisher->FilterMetadata(filterExpression);
}

bool PublisherInstance::Start(const TcpEndPoint& endpoint)
{
    if (IsStarted())
        throw PublisherException("Publisher is already started; stop first");

    if (IsReverseConnection())
        throw PublisherException("Cannot start a listening connection, publisher is already established in reverse connection mode");

    string errorMessage{};

    try
    {
        m_publisher->Start(endpoint);
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
        errorMessage = boost::current_exception_diagnostic_information(true);
    }

    if (!errorMessage.empty())
        ErrorMessage("Failed to listen on port " + ToString(endpoint.port()) + ": " + errorMessage);

    return m_publisher->IsStarted();
}

bool PublisherInstance::Start(const uint16_t port, const bool ipV6)
{
    return Start(TcpEndPoint(ipV6 ? tcp::v6() : tcp::v4(), port));
}

bool PublisherInstance::Start(const string& networkInterfaceIP, const uint16_t port)
{
    return Start(TcpEndPoint(make_address(networkInterfaceIP), port));
}

void PublisherInstance::Initialize(const std::string& hostname, const uint16_t port)
{
    m_publisher->m_reverseConnection = true;
    m_hostname = hostname;
    m_port = port;
}

bool PublisherInstance::GetAutoReconnect() const
{
    return m_autoReconnect;
}

void PublisherInstance::SetAutoReconnect(const bool autoReconnect)
{
    m_autoReconnect = autoReconnect;
}

int32_t PublisherInstance::GetMaxRetries() const
{
    return m_maxRetries;
}

void PublisherInstance::SetMaxRetries(const int32_t maxRetries)
{
    m_maxRetries = maxRetries;
}

int32_t PublisherInstance::GetRetryInterval() const
{
    return m_retryInterval;
}

void PublisherInstance::SetRetryInterval(const int32_t retryInterval)
{
    m_retryInterval = retryInterval;
}

int32_t PublisherInstance::GetMaxRetryInterval() const
{
    return m_maxRetryInterval;
}

void PublisherInstance::SetMaxRetryInterval(const int32_t maxRetryInterval)
{
    m_maxRetryInterval = maxRetryInterval;
}

bool PublisherInstance::Connect()
{
    if (IsStarted())
        throw PublisherException("Cannot start a reverse connection, publisher is already established in listening connection mode");

    if (IsConnected())
        throw PublisherException("Publisher is already connected; disconnect first");

    SubscriberConnector& connector = m_publisher->GetSubscriberConnector();
    connector.ResetConnection();

    // Set up helper objects (derived classes can override behavior and settings)
    SetupSubscriberConnector(connector);

    if (m_hostname.empty())
        throw PublisherException("No hostname specified for reverse connection: call Initialize before Connect");

    if (m_port == 0)
        throw PublisherException("No port specified for reverse connection: call Initialize before Connect");

    m_publisher->m_reverseConnection = true;

    // Connect to subscriber
    const int result = connector.Connect(*m_publisher);

    if (result == SubscriberConnector::ConnectSuccess)
    {
        ClientConnected(m_publisher->GetSingleConnection());
        return true;
    }

    if (result == SubscriberConnector::ConnectFailed)
        ErrorMessage("All connection attempts failed");

    return false;
}

void PublisherInstance::ConnectAsync()
{
    if (IsStarted())
        throw PublisherException("Cannot start a reverse connection, publisher is already established in listening connection mode");

    if (IsConnected())
        throw PublisherException("Publisher is already connected; disconnect first");

    m_connectThread = Thread([&]{ Connect(); });
}

void PublisherInstance::Stop()
{
    m_publisher->Stop();
}

bool PublisherInstance::IsConnected() const
{
    return m_publisher->IsConnected();
}

bool PublisherInstance::IsReverseConnection() const
{
    return m_publisher->IsReverseConnection();
}

bool PublisherInstance::IsStarted() const
{
    return m_publisher->IsStarted();
}

void PublisherInstance::PublishMeasurements(const SimpleMeasurement* measurements, const int32_t count) const
{
    vector<MeasurementPtr> measurementPtrs(count);

    for (int32_t i = 0; i < count; i++)
        measurementPtrs[i] = ToPtr(measurements[i]);

    m_publisher->PublishMeasurements(measurementPtrs);
}

void PublisherInstance::PublishMeasurements(const vector<Measurement>& measurements) const
{
    m_publisher->PublishMeasurements(measurements);
}

void PublisherInstance::PublishMeasurements(const vector<MeasurementPtr>& measurements) const
{
    m_publisher->PublishMeasurements(measurements);
}

const sttp::Guid& PublisherInstance::GetNodeID() const
{
    return m_publisher->GetNodeID();
}

void PublisherInstance::SetNodeID(const sttp::Guid& nodeID) const
{
    m_publisher->SetNodeID(nodeID);
}

SecurityMode PublisherInstance::GetSecurityMode() const
{
    return m_publisher->GetSecurityMode();
}

void PublisherInstance::SetSecurityMode(const SecurityMode securityMode) const
{
    m_publisher->SetSecurityMode(securityMode);
}

int32_t PublisherInstance::GetMaximumAllowedConnections() const
{
    return m_publisher->GetMaximumAllowedConnections();
}

void PublisherInstance::SetMaximumAllowedConnections(const int32_t value) const
{
    m_publisher->SetMaximumAllowedConnections(value);
}

bool PublisherInstance::IsMetadataRefreshAllowed() const
{
    return m_publisher->GetIsMetadataRefreshAllowed();
}

void PublisherInstance::SetMetadataRefreshAllowed(const bool allowed) const
{
    m_publisher->SetIsMetadataRefreshAllowed(allowed);
}

bool PublisherInstance::IsNaNValueFilterAllowed() const
{
    return m_publisher->GetIsNaNValueFilterAllowed();
}

void PublisherInstance::SetNaNValueFilterAllowed(const bool allowed) const
{
    m_publisher->SetNaNValueFilterAllowed(allowed);
}

bool PublisherInstance::IsNaNValueFilterForced() const
{
    return m_publisher->GetIsNaNValueFilterForced();
}

void PublisherInstance::SetNaNValueFilterForced(const bool forced) const
{
    m_publisher->SetIsNaNValueFilterForced(forced);
}

bool PublisherInstance::GetSupportsTemporalSubscriptions() const
{
    return m_publisher->GetSupportsTemporalSubscriptions();
}

void PublisherInstance::SetSupportsTemporalSubscriptions(const bool value) const
{
    m_publisher->SetSupportsTemporalSubscriptions(value);
}

uint32_t PublisherInstance::GetCipherKeyRotationPeriod() const
{
    return m_publisher->GetCipherKeyRotationPeriod();
}

void PublisherInstance::SetCipherKeyRotationPeriod(const uint32_t period) const
{
    m_publisher->SetCipherKeyRotationPeriod(period);
}

uint16_t PublisherInstance::GetPort() const
{
    return m_publisher->GetPort();
}

bool PublisherInstance::IsIPv6() const
{
    return m_publisher->IsIPv6();
}

void* PublisherInstance::GetUserData() const
{
    return m_userData;
}

void PublisherInstance::SetUserData(void* userData)
{
    m_userData = userData;
}

uint64_t PublisherInstance::GetTotalCommandChannelBytesSent() const
{
    return m_publisher->GetTotalCommandChannelBytesSent();
}

uint64_t PublisherInstance::GetTotalDataChannelBytesSent() const
{
    return m_publisher->GetTotalDataChannelBytesSent();
}

uint64_t PublisherInstance::GetTotalMeasurementsSent() const
{
    return m_publisher->GetTotalMeasurementsSent();
}

bool PublisherInstance::TryGetSubscriberConnections(vector<SubscriberConnectionPtr>& subscriberConnections) const
{
    subscriberConnections.clear();

    m_publisher->IterateSubscriberConnections([&subscriberConnections](const SubscriberConnectionPtr& connection, void* userData)
    {
        subscriberConnections.push_back(connection);
    },
    nullptr);

    return !subscriberConnections.empty();
}

void PublisherInstance::DisconnectSubscriber(const SubscriberConnectionPtr& connection) const
{
    m_publisher->DisconnectSubscriber(connection);
}

void PublisherInstance::DisconnectSubscriber(const sttp::Guid& instanceID) const
{
    m_publisher->DisconnectSubscriber(instanceID);
}
