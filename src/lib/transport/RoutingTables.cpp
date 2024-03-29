//******************************************************************************************************
//  RoutingTables.cpp - Gbtc
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
//  03/28/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include "RoutingTables.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;

RoutingTables::RoutingTables() :
    m_activeRoutes(NewSharedPtr<RoutingTable>()),
    m_enabled(true)
{
    m_routingTablesThread = Thread([this]()
    {
        while (true)
        {
            m_routingTableOperations.WaitForData();

            if (!m_enabled)
                break;

            const auto [handler, routes] = m_routingTableOperations.Dequeue();

            if (handler != nullptr)
                handler(*this, routes);
        }
    });
}

RoutingTables::~RoutingTables() noexcept
{
    try
    {
        m_enabled = false;
        m_routingTableOperations.Release();
        m_routingTablesThread.join();
    }
    catch (...)
    {
        // ReSharper disable once CppRedundantControlFlowJump
        return;
    }
}

RoutingTables::RoutingTablePtr RoutingTables::CloneActiveRoutes()
{
    ReaderLock readLock(m_activeRoutesLock);
    RoutingTablePtr clonedRoutes = NewSharedPtr<RoutingTable>(*m_activeRoutes);
    return clonedRoutes;
}

void RoutingTables::SetActiveRoutes(RoutingTablePtr activeRoutes)
{
    WriterLock writeLock(m_activeRoutesLock);
    m_activeRoutes = std::move(activeRoutes);
}

void RoutingTables::UpdateRoutesOperation(RoutingTables& routingTables, const DestinationRoutes& destinationRoutes)
{
    const RoutingTablePtr activeRoutesPtr = routingTables.CloneActiveRoutes();
    RoutingTable& activeRoutes = *activeRoutesPtr;
    const SubscriberConnectionPtr& destination = destinationRoutes.first;
    const unordered_set<sttp::Guid>& routes = destinationRoutes.second;

    // Remove subscriber connection from undesired measurement route destinations
    for (auto& [signalID, destinations] : activeRoutes)
    {
        if (routes.contains(signalID))
            destinations->erase(destination);
    }

    // Add subscriber connection to desired measurement route destinations
    for (const auto& signalID : routes)
    {
        DestinationsPtr destinations;

        if (!TryGetValue(activeRoutes, signalID, destinations, destinations))
        {
            destinations = NewSharedPtr<Destinations>();
            activeRoutes.emplace(signalID, destinations);
        }

        destinations->insert(destination);
    }

    routingTables.SetActiveRoutes(activeRoutesPtr);
}

void RoutingTables::RemoveRoutesOperation(RoutingTables& routingTables, const DestinationRoutes& destinationRoutes)
{
    const RoutingTablePtr activeRoutesPtr = routingTables.CloneActiveRoutes();
    const RoutingTable& activeRoutes = *activeRoutesPtr;
    const SubscriberConnectionPtr& destination = destinationRoutes.first;

    // Remove subscriber connection from existing measurement route destinations
    for (const auto& [_, destinations] : activeRoutes)
        destinations->erase(destination);

    routingTables.SetActiveRoutes(activeRoutesPtr);
}

void RoutingTables::UpdateRoutes(const SubscriberConnectionPtr& destination, const unordered_set<Guid>& routes)
{
    // Queue update routes operation
    m_routingTableOperations.Enqueue(RoutingTableOperation(&UpdateRoutesOperation, DestinationRoutes(destination, routes)));
}

void RoutingTables::RemoveRoutes(const SubscriberConnectionPtr& destination)
{
    // Queue remove routes operation
    m_routingTableOperations.Enqueue(RoutingTableOperation(&RemoveRoutesOperation, DestinationRoutes(destination, unordered_set<Guid>())));
}

void RoutingTables::PublishMeasurements(const vector<MeasurementPtr>& measurements)
{
    typedef vector<MeasurementPtr> Measurements;
    typedef SharedPtr<Measurements> MeasurementsPtr;
    unordered_map<SubscriberConnectionPtr, MeasurementsPtr> routedMeasurementMap;
    const size_t size = measurements.size();

    // Constrain read lock to this block
    {
        ReaderLock readLock(m_activeRoutesLock);
        const RoutingTable& activeRoutes = *m_activeRoutes;

        for (const auto& measurement : measurements)
        {
            DestinationsPtr destinationsPtr;

            if (TryGetValue(activeRoutes, measurement->SignalID, destinationsPtr, destinationsPtr))
            {
                const Destinations& destinations = *destinationsPtr;

                for (const auto& destination : destinations)
                {
                    if (destination == nullptr)
                        continue;

                    MeasurementsPtr routedMeasurements;

                    if (!TryGetValue(routedMeasurementMap, destination, routedMeasurements, routedMeasurements))
                    {
                        routedMeasurements = NewSharedPtr<Measurements>();
                        routedMeasurements->reserve(size);
                        routedMeasurementMap.emplace(destination, routedMeasurements);
                    }

                    routedMeasurements->push_back(measurement);
                }
            }
        }
    }

    // Publish routed measurements
    for (auto& [destinationPtr, subscriberMeasurements] : routedMeasurementMap)
    {
        auto& destination = *destinationPtr;

        if (destination.GetIsSubscribed() && !destination.GetIsTemporalSubscription())
            destination.PublishMeasurements(*subscriberMeasurements);
    }
}

void RoutingTables::Clear()
{
    m_routingTableOperations.Clear();
    WriterLock writeLock(m_activeRoutesLock);
    m_activeRoutes->clear();
}
