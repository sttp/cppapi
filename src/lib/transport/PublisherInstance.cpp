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
#include <iostream>

using namespace std;
using namespace pugi;
using namespace sttp;
using namespace sttp::data;
using namespace sttp::transport;
using namespace boost::asio::ip;

PublisherInstance::PublisherInstance() :
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

PublisherInstance::~PublisherInstance() = default;

void PublisherInstance::HandleStatusMessage(DataPublisher* source, const string& message)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->StatusMessage(message);
}

void PublisherInstance::HandleErrorMessage(DataPublisher* source, const string& message)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->ErrorMessage(message);
}

void PublisherInstance::HandleClientConnected(DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->ClientConnected(connection);
}

void PublisherInstance::HandleClientDisconnected(DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->ClientDisconnected(connection);
}

void PublisherInstance::HandleProcessingIntervalChangeRequested(DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->ProcessingIntervalChangeRequested(connection);
}

void PublisherInstance::HandleTemporalSubscriptionRequested(DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->TemporalSubscriptionRequested(connection);
}

void PublisherInstance::HandleTemporalSubscriptionCanceled(DataPublisher* source, const SubscriberConnectionPtr& connection)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->TemporalSubscriptionCanceled(connection);
}

void PublisherInstance::HandleReceivedUserCommand(DataPublisher* source, const SubscriberConnectionPtr& connection, uint32_t command, const std::vector<uint8_t>& buffer)
{
    PublisherInstance* instance = static_cast<PublisherInstance*>(source->GetUserData());

    if (instance == nullptr)
        return;

    instance->HandleUserCommand(connection, command, buffer);
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
    cout << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " connected..." << endl << endl;
}

void PublisherInstance::ClientDisconnected(const SubscriberConnectionPtr& connection)
{
    cout << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " disconnected..." << endl << endl;
}

void PublisherInstance::ProcessingIntervalChangeRequested(const SubscriberConnectionPtr& connection)
{
    cout << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " has requested to change its temporal processing interval to " << ToString(connection->GetProcessingInterval()) << "ms" << endl << endl;
}

void PublisherInstance::TemporalSubscriptionRequested(const SubscriberConnectionPtr& connection)
{
    cout << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " has requested a temporal subscription starting at " << ToString(connection->GetStartTimeConstraint()) << endl << endl;
}

void PublisherInstance::TemporalSubscriptionCanceled(const SubscriberConnectionPtr& connection)
{
    cout << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " has canceled the temporal subscription starting at " << ToString(connection->GetStartTimeConstraint()) << endl << endl;
}

void PublisherInstance::HandleUserCommand(const SubscriberConnectionPtr& connection, uint32_t command, const std::vector<uint8_t>& buffer)
{
    cout << "Client \"" << connection->GetConnectionID() << "\" with subscriber ID " << ToString(connection->GetSubscriberID()) << " sent user-defined command \"" << ToHex(command) << "\" with " << buffer.size() << " bytes of payload" << endl << endl;
}

void PublisherInstance::DefineMetadata(const vector<DeviceMetadataPtr>& deviceMetadata, const vector<MeasurementMetadataPtr>& measurementMetadata, const vector<PhasorMetadataPtr>& phasorMetadata, int32_t versionNumber) const
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

bool PublisherInstance::Start(uint16_t port, bool ipV6)
{
    return Start(TcpEndPoint(ipV6 ? tcp::v6() : tcp::v4(), port));
}

bool PublisherInstance::Start(const string& networkInterfaceIP, uint16_t port)
{
    return Start(TcpEndPoint(make_address(networkInterfaceIP), port));
}

void PublisherInstance::Stop()
{
    m_publisher->Stop();
}

bool PublisherInstance::IsStarted() const
{
    return m_publisher->IsStarted();
}

void PublisherInstance::PublishMeasurements(const SimpleMeasurement* measurements, int32_t count) const
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

void PublisherInstance::SetSecurityMode(SecurityMode securityMode) const
{
    m_publisher->SetSecurityMode(securityMode);
}

int32_t PublisherInstance::GetMaximumAllowedConnections() const
{
    return m_publisher->GetMaximumAllowedConnections();
}

void PublisherInstance::SetMaximumAllowedConnections(int32_t value) const
{
    m_publisher->SetMaximumAllowedConnections(value);
}

bool PublisherInstance::IsMetadataRefreshAllowed() const
{
    return m_publisher->GetIsMetadataRefreshAllowed();
}

void PublisherInstance::SetMetadataRefreshAllowed(bool allowed) const
{
    m_publisher->SetIsMetadataRefreshAllowed(allowed);
}

bool PublisherInstance::IsNaNValueFilterAllowed() const
{
    return m_publisher->GetIsNaNValueFilterAllowed();
}

void PublisherInstance::SetNaNValueFilterAllowed(bool allowed) const
{
    m_publisher->SetNaNValueFilterAllowed(allowed);
}

bool PublisherInstance::IsNaNValueFilterForced() const
{
    return m_publisher->GetIsNaNValueFilterForced();
}

void PublisherInstance::SetNaNValueFilterForced(bool forced) const
{
    m_publisher->SetIsNaNValueFilterForced(forced);
}

bool PublisherInstance::GetSupportsTemporalSubscriptions() const
{
    return m_publisher->GetSupportsTemporalSubscriptions();
}

void PublisherInstance::SetSupportsTemporalSubscriptions(bool value) const
{
    m_publisher->SetSupportsTemporalSubscriptions(value);
}

uint32_t PublisherInstance::GetCipherKeyRotationPeriod() const
{
    return m_publisher->GetCipherKeyRotationPeriod();
}

void PublisherInstance::SetCipherKeyRotationPeriod(uint32_t period) const
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
