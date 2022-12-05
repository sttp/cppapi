//******************************************************************************************************
//  Constants.cpp - Gbtc
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
//  02/20/2019 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#include "Constants.h"
#include "../Convert.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;

MeasurementStateFlags sttp::transport::operator &(MeasurementStateFlags lhs, MeasurementStateFlags rhs)
{
    return static_cast<MeasurementStateFlags> (
        static_cast<underlying_type_t<MeasurementStateFlags>>(lhs) &
        static_cast<underlying_type_t<MeasurementStateFlags>>(rhs)
        );
}

MeasurementStateFlags sttp::transport::operator ^(MeasurementStateFlags lhs, MeasurementStateFlags rhs)
{
    return static_cast<MeasurementStateFlags> (
        static_cast<underlying_type_t<MeasurementStateFlags>>(lhs) ^
        static_cast<underlying_type_t<MeasurementStateFlags>>(rhs)
        );
}

MeasurementStateFlags sttp::transport::operator ~(MeasurementStateFlags rhs)
{
    return static_cast<MeasurementStateFlags> (
        ~static_cast<underlying_type_t<MeasurementStateFlags>>(rhs)
        );
}

MeasurementStateFlags& sttp::transport::operator |=(MeasurementStateFlags& lhs, MeasurementStateFlags rhs)
{
    lhs = static_cast<MeasurementStateFlags> (
        static_cast<underlying_type_t<MeasurementStateFlags>>(lhs) |
        static_cast<underlying_type_t<MeasurementStateFlags>>(rhs)
        );

    return lhs;
}

MeasurementStateFlags& sttp::transport::operator &=(MeasurementStateFlags& lhs, MeasurementStateFlags rhs)
{
    lhs = static_cast<MeasurementStateFlags> (
        static_cast<underlying_type_t<MeasurementStateFlags>>(lhs) &
        static_cast<underlying_type_t<MeasurementStateFlags>>(rhs)
        );

    return lhs;
}

MeasurementStateFlags& sttp::transport::operator ^=(MeasurementStateFlags& lhs, MeasurementStateFlags rhs)
{
    lhs = static_cast<MeasurementStateFlags> (
        static_cast<underlying_type_t<MeasurementStateFlags>>(lhs) ^
        static_cast<underlying_type_t<MeasurementStateFlags>>(rhs)
        );

    return lhs;
}

string ServerCommand::ToString(const uint8_t command)
{
    switch (command)
    {
    case ServerCommand::Connect:
        return "Connect";
    case ServerCommand::MetadataRefresh:
        return "MetadataRefresh";
    case ServerCommand::Subscribe:
        return "Subscribe";
    case ServerCommand::Unsubscribe:
        return "Unsubscribe";
    case ServerCommand::RotateCipherKeys:
        return "RotateCipherKeys";
    case ServerCommand::UpdateProcessingInterval:
        return "UpdateProcessingInterval";
    case ServerCommand::DefineOperationalModes:
        return "DefineOperationalModes";
    case ServerCommand::ConfirmNotification:
        return "ConfirmNotification";
    case ServerCommand::ConfirmBufferBlock:
        return "ConfirmBufferBlock";
    case ServerCommand::ConfirmUpdateSignalIndexCache:
        return "ConfirmUpdateSignalIndexCache";
    case ServerCommand::ConfirmUpdateCipherKeys:
        return "ConfirmUpdateCipherKeys";
    case ServerCommand::GetPrimaryMetadataSchema:
        return "GetPrimaryMetadataSchema";
    case ServerCommand::GetSignalSelectionSchema:
        return "GetSignalSelectionSchema";
    case ServerCommand::UserCommand00:
        return "UserCommand00";
    case ServerCommand::UserCommand01:
        return "UserCommand01";
    case ServerCommand::UserCommand02:
        return "UserCommand02";
    case ServerCommand::UserCommand03:
        return "UserCommand03";
    case ServerCommand::UserCommand04:
        return "UserCommand04";
    case ServerCommand::UserCommand05:
        return "UserCommand05";
    case ServerCommand::UserCommand06:
        return "UserCommand06";
    case ServerCommand::UserCommand07:
        return "UserCommand07";
    case ServerCommand::UserCommand08:
        return "UserCommand08";
    case ServerCommand::UserCommand09:
        return "UserCommand09";
    case ServerCommand::UserCommand10:
        return "UserCommand10";
    case ServerCommand::UserCommand11:
        return "UserCommand11";
    case ServerCommand::UserCommand12:
        return "UserCommand12";
    case ServerCommand::UserCommand13:
        return "UserCommand13";
    case ServerCommand::UserCommand14:
        return "UserCommand14";
    case ServerCommand::UserCommand15:
        return "UserCommand15";
    default:
        return ToHex(command);
    }
}

string ServerResponse::ToString(const uint8_t response)
{
    switch (response)
    {
    case ServerResponse::Succeeded:
        return "Succeeded";
    case ServerResponse::Failed:
        return "Failed";
    case ServerResponse::DataPacket:
        return "DataPacket";
    case ServerResponse::UpdateSignalIndexCache:
        return "UpdateSignalIndexCache";
    case ServerResponse::UpdateBaseTimes:
        return "UpdateBaseTimes";
    case ServerResponse::UpdateCipherKeys:
        return "UpdateCipherKeys";
    case ServerResponse::DataStartTime:
        return "DataStartTime";
    case ServerResponse::ProcessingComplete:
        return "ProcessingComplete";
    case ServerResponse::BufferBlock:
        return "BufferBlock";
    case ServerResponse::Notify:
        return "Notify";
    case ServerResponse::ConfigurationChanged:
        return "ConfigurationChanged";
    case ServerResponse::UserResponse00:
        return "UserResponse00";
    case ServerResponse::UserResponse01:
        return "UserResponse01";
    case ServerResponse::UserResponse02:
        return "UserResponse02";
    case ServerResponse::UserResponse03:
        return "UserResponse03";
    case ServerResponse::UserResponse04:
        return "UserResponse04";
    case ServerResponse::UserResponse05:
        return "UserResponse05";
    case ServerResponse::UserResponse06:
        return "UserResponse06";
    case ServerResponse::UserResponse07:
        return "UserResponse07";
    case ServerResponse::UserResponse08:
        return "UserResponse08";
    case ServerResponse::UserResponse09:
        return "UserResponse09";
    case ServerResponse::UserResponse10:
        return "UserResponse10";
    case ServerResponse::UserResponse11:
        return "UserResponse11";
    case ServerResponse::UserResponse12:
        return "UserResponse12";
    case ServerResponse::UserResponse13:
        return "UserResponse13";
    case ServerResponse::UserResponse14:
        return "UserResponse14";
    case ServerResponse::UserResponse15:
        return "UserResponse15";
    case ServerResponse::NoOP:
        return "NoOP";
    default:
        return ToHex(response);
    }
}

// Define "instances" of all static constants so values can be passed by reference and found by linker
// JRC: This appears to be no longer needed, leaving for now in case a particular environment needs it
/*
const size_t Common::MaxPacketSize;
const uint32_t Common::PayloadHeaderSize;
const uint32_t Common::ResponseHeaderSize;

const uint8_t DataPacketFlags::Synchronized;
const uint8_t DataPacketFlags::Compact;
const uint8_t DataPacketFlags::CipherIndex;
const uint8_t DataPacketFlags::Compressed;
const uint8_t DataPacketFlags::CacheIndex;
const uint8_t DataPacketFlags::NoFlags;

const uint8_t ServerCommand::Connect;
const uint8_t ServerCommand::MetadataRefresh;
const uint8_t ServerCommand::Subscribe;
const uint8_t ServerCommand::Unsubscribe;
const uint8_t ServerCommand::RotateCipherKeys;
const uint8_t ServerCommand::UpdateProcessingInterval;
const uint8_t ServerCommand::DefineOperationalModes;
const uint8_t ServerCommand::ConfirmNotification;
const uint8_t ServerCommand::ConfirmBufferBlock;
const uint8_t ServerCommand::ConfirmUpdateBaseTimes;
const uint8_t ServerCommand::ConfirmUpdateSignalIndexCache;
const uint8_t ServerCommand::ConfirmUpdateCipherKeys;
const uint8_t ServerCommand::GetPrimaryMetadataSchema;
const uint8_t ServerCommand::GetSignalSelectionSchema;
const uint8_t ServerCommand::UserCommand00;
const uint8_t ServerCommand::UserCommand01;
const uint8_t ServerCommand::UserCommand02;
const uint8_t ServerCommand::UserCommand03;
const uint8_t ServerCommand::UserCommand04;
const uint8_t ServerCommand::UserCommand05;
const uint8_t ServerCommand::UserCommand06;
const uint8_t ServerCommand::UserCommand07;
const uint8_t ServerCommand::UserCommand08;
const uint8_t ServerCommand::UserCommand09;
const uint8_t ServerCommand::UserCommand10;
const uint8_t ServerCommand::UserCommand11;
const uint8_t ServerCommand::UserCommand12;
const uint8_t ServerCommand::UserCommand13;
const uint8_t ServerCommand::UserCommand14;
const uint8_t ServerCommand::UserCommand15;

const uint8_t ServerResponse::Succeeded;
const uint8_t ServerResponse::Failed;
const uint8_t ServerResponse::DataPacket;
const uint8_t ServerResponse::UpdateSignalIndexCache;
const uint8_t ServerResponse::UpdateBaseTimes;
const uint8_t ServerResponse::UpdateCipherKeys;
const uint8_t ServerResponse::DataStartTime;
const uint8_t ServerResponse::ProcessingComplete;
const uint8_t ServerResponse::BufferBlock;
const uint8_t ServerResponse::Notify;
const uint8_t ServerResponse::ConfigurationChanged;
const uint8_t ServerResponse::UserResponse00;
const uint8_t ServerResponse::UserResponse01;
const uint8_t ServerResponse::UserResponse02;
const uint8_t ServerResponse::UserResponse03;
const uint8_t ServerResponse::UserResponse04;
const uint8_t ServerResponse::UserResponse05;
const uint8_t ServerResponse::UserResponse06;
const uint8_t ServerResponse::UserResponse07;
const uint8_t ServerResponse::UserResponse08;
const uint8_t ServerResponse::UserResponse09;
const uint8_t ServerResponse::UserResponse10;
const uint8_t ServerResponse::UserResponse11;
const uint8_t ServerResponse::UserResponse12;
const uint8_t ServerResponse::UserResponse13;
const uint8_t ServerResponse::UserResponse14;
const uint8_t ServerResponse::UserResponse15;
const uint8_t ServerResponse::NoOP;

const uint32_t OperationalModes::VersionMask;
const uint32_t OperationalModes::CompressionModeMask;
const uint32_t OperationalModes::EncodingMask;
const uint32_t OperationalModes::ImplementationSpecificExtensionMask;
const uint32_t OperationalModes::ReceiveExternalMetadata;
const uint32_t OperationalModes::ReceiveInternalMetadata;
const uint32_t OperationalModes::CompressPayloadData;
const uint32_t OperationalModes::CompressSignalIndexCache;
const uint32_t OperationalModes::CompressMetadata;
const uint32_t OperationalModes::NoFlags;

const uint32_t OperationalEncoding::UTF16LE;
const uint32_t OperationalEncoding::UTF16BE;
const uint32_t OperationalEncoding::UTF8;

const uint32_t CompressionModes::GZip;
const uint32_t CompressionModes::TSSC;
const uint32_t CompressionModes::None;
*/