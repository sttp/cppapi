//******************************************************************************************************
//  RoutingTables.h - Gbtc
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
//  03/28/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "../CommonTypes.h"
#include "../ThreadSafeQueue.h"
#include "SubscriberConnection.h"

namespace sttp {
namespace transport
{
    class RoutingTables // NOLINT
    {
    private:
        typedef std::unordered_set<SubscriberConnectionPtr> Destinations;
        typedef sttp::SharedPtr<Destinations> DestinationsPtr;
        typedef std::unordered_map<sttp::Guid, DestinationsPtr> RoutingTable;
        typedef sttp::SharedPtr<RoutingTable> RoutingTablePtr;
        typedef std::pair<SubscriberConnectionPtr, std::unordered_set<sttp::Guid>> DestinationRoutes;
        typedef std::function<void(RoutingTables&, const DestinationRoutes&)> RoutingTableOperationHandler;
        typedef std::pair<RoutingTableOperationHandler, DestinationRoutes> RoutingTableOperation;

        sttp::Thread m_routingTablesThread;
        sttp::ThreadSafeQueue<RoutingTableOperation> m_routingTableOperations;
        RoutingTablePtr m_activeRoutes;
        sttp::SharedMutex m_activeRoutesLock;
        std::atomic_bool m_enabled;

        RoutingTablePtr CloneActiveRoutes();
        void SetActiveRoutes(RoutingTablePtr activeRoutes);
        static void UpdateRoutesOperation(RoutingTables& routingTables, const DestinationRoutes& destinationRoutes);
        static void RemoveRoutesOperation(RoutingTables& routingTables, const DestinationRoutes& destinationRoutes);

    public:
        RoutingTables();
        ~RoutingTables();

        void UpdateRoutes(const SubscriberConnectionPtr& destination, const std::unordered_set<sttp::Guid>& routes);
        void RemoveRoutes(const SubscriberConnectionPtr& destination);
        void PublishMeasurements(const std::vector<MeasurementPtr>& measurements);

        void Clear();
    };
}}