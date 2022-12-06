//******************************************************************************************************
//  Constants.h - Gbtc
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
//  03/29/2012 - Stephen C. Wills
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "../CommonTypes.h"

namespace sttp::transport
{
    // Measurement state flags.
    enum class MeasurementStateFlags : unsigned int
    {
        // Defines normal state.
        Normal = 0x0,
        // Defines bad data state.
        BadData = 0x1,
        // Defines suspect data state.
        SuspectData = 0x2,
        // Defines over range error, i.e., unreasonable high value.
        OverRangeError = 0x4,
        // Defines under range error, i.e., unreasonable low value.
        UnderRangeError = 0x8,
        // Defines alarm for high value.
        AlarmHigh = 0x10,
        // Defines alarm for low value.
        AlarmLow = 0x20,
        // Defines warning for high value.
        WarningHigh = 0x40,
        // Defines warning for low value.
        WarningLow = 0x80,
        // Defines alarm for flat-lined value, i.e., latched value test alarm.
        FlatlineAlarm = 0x100,
        // Defines comparison alarm, i.e., outside threshold of comparison with a real-time value.
        ComparisonAlarm = 0x200,
        // Defines rate-of-change alarm.
        ROCAlarm = 0x400,
        // Defines bad value received.
        ReceivedAsBad = 0x800,
        // Defines calculated value state.
        CalculatedValue = 0x1000,
        // Defines calculation error with the value.
        CalculationError = 0x2000,
        // Defines calculation warning with the value.
        CalculationWarning = 0x4000,
        // Defines reserved quality flag.
        ReservedQualityFlag = 0x8000,
        // Defines bad time state.
        BadTime = 0x10000,
        // Defines suspect time state.
        SuspectTime = 0x20000,
        // Defines late time alarm.
        LateTimeAlarm = 0x40000,
        // Defines future time alarm.
        FutureTimeAlarm = 0x80000,
        // Defines up-sampled state.
        UpSampled = 0x100000,
        // Defines down-sampled state.
        DownSampled = 0x200000,
        // Defines discarded value state.
        DiscardedValue = 0x400000,
        // Defines reserved time flag.
        ReservedTimeFlag = 0x800000,
        // Defines user defined flag 1.
        UserDefinedFlag1 = 0x1000000,
        // Defines user defined flag 2.
        UserDefinedFlag2 = 0x2000000,
        // Defines user defined flag 3.
        UserDefinedFlag3 = 0x4000000,
        // Defines user defined flag 4.
        UserDefinedFlag4 = 0x8000000,
        // Defines user defined flag 5.
        UserDefinedFlag5 = 0x10000000,
        // Defines system error state.
        SystemError = 0x20000000,
        // Defines system warning state.
        SystemWarning = 0x40000000,
        // Defines measurement error flag.
        MeasurementError = 0x80000000
    };

    MeasurementStateFlags operator &(MeasurementStateFlags lhs, MeasurementStateFlags rhs);
    MeasurementStateFlags operator ^(MeasurementStateFlags lhs, MeasurementStateFlags rhs);
    MeasurementStateFlags operator ~(MeasurementStateFlags rhs);
    MeasurementStateFlags& operator |=(MeasurementStateFlags &lhs, MeasurementStateFlags rhs);
    MeasurementStateFlags& operator &=(MeasurementStateFlags &lhs, MeasurementStateFlags rhs);
    MeasurementStateFlags& operator ^=(MeasurementStateFlags &lhs, MeasurementStateFlags rhs);

    // Common constants.
    struct Common
    {
        static constexpr size_t MaxPacketSize = 32768U;
        static constexpr uint32_t PayloadHeaderSize = 4U;
        static constexpr uint32_t ResponseHeaderSize = 6U;
    };

    // DataPublisher data packet flags.
    struct DataPacketFlags
    {
        // Obsolete: Determines if data packet is synchronized. Bit set = synchronized, bit clear = unsynchronized.
        static constexpr uint8_t Synchronized = 0x01;
        // Determines if serialized measurement is compact. Bit set = compact, bit clear = full fidelity.
        static constexpr uint8_t Compact = 0x02;
        // Determines which cipher index to use when encrypting data packet. Bit set = use odd cipher index (i.e., 1), bit clear = use even cipher index (i.e., 0).
        static constexpr uint8_t CipherIndex = 0x04;
        // Determines if data packet payload is compressed. Bit set = payload compressed, bit clear = payload normal.
        static constexpr uint8_t Compressed = 0x08;
        // Determines which signal index cache to use when decoding a data packet. Used by STTP version 2 or greater.
        // Bit set = use odd cache index (i.e., 1), bit clear = use even cache index (i.e., 0).
        static constexpr uint8_t CacheIndex = 0x10;
        // Defines state where there are no flags set.
        static constexpr uint8_t NoFlags = 0x00;
    };

    // Enumeration of the possible server commands received by a DataPublisher and sent by a DataSubscriber during an STTP session.
    struct ServerCommand
    {
        // Solicited server commands will receive a ServerResponse.Succeeded or ServerResponse.Failed response
        // code along with an associated success or failure message. Message type for successful responses will
        // be based on server command - for example, server response for a successful MetaDataRefresh command
        // will return a serialized DataSet of the available server metadata. Message type for failed responses
        // will always be a string of text representing the error message.

        // Defines a command code handling connect operations.
	    // Only used as part of connection refused response -- value not sent on the wire.
        static constexpr uint8_t Connect = 0x00;
        // Defines a command code for requesting an updated set of metadata.
        static constexpr uint8_t MetadataRefresh = 0x01;
        // Defines a command code for requesting a subscription of streaming data from server based on connection string that follows.
        static constexpr uint8_t Subscribe = 0x02;
        // Defines a command code for requesting that server stop sending streaming data to the client and cancel the current subscription.
        static constexpr uint8_t Unsubscribe = 0x03;
        // Defines a command code for manually requesting that server send a new set of cipher keys for data packet encryption (UDP only).
        static constexpr uint8_t RotateCipherKeys = 0x04;
        // Defines a command code for manually requesting that server to update the processing interval with the following specified value.
        static constexpr uint8_t UpdateProcessingInterval = 0x05;
        // Defines a command code for establishing operational modes.
	    // As soon as connection is established, requests that server set operational modes that affect how the subscriber and publisher will communicate.
        static constexpr uint8_t DefineOperationalModes = 0x06;
        // Defines a command code for receipt of a notification.
	    // This message is sent in response to `ServerResponse.Notify` allowing publisher to clear notification retransmit status.
        static constexpr uint8_t ConfirmNotification = 0x07;
        // Defines a command code for receipt of a buffer block measurement.
	    // This message is sent in response to `ServerResponse.BufferBlock` allowing publisher to clear buffer block retransmit status.
        static constexpr uint8_t ConfirmBufferBlock = 0x08;
        // Defines a command code for receipt of a base time update.
	    // This message is sent in response to `ServerResponse.UpdateBaseTimes` allowing publisher to safely transition to next base time offsets.
        static constexpr uint8_t ConfirmUpdateBaseTimes = 0x09;
        // Defines a service command for confirming the receipt of a signal index cache.
	    // This message is sent in response to `ServerResponse.UpdateSignalIndexCache` allowing publisher to safely transition to next signal index cache.
        static constexpr uint8_t ConfirmUpdateSignalIndexCache = 0x0A;
        // Defines a command code for receipt of a cipher key update.
	    // This message is sent in response to `ServerResponse.UpdateCipherKeys` allowing publisher to safely transition to next cipher key set.
        static constexpr uint8_t ConfirmUpdateCipherKeys = 0x0B;
        // Defines a command code for requesting the primary metadata schema.
        static constexpr uint8_t GetPrimaryMetadataSchema = 0x0C;
        // Defines a command code for requesting the signal selection schema.
        static constexpr uint8_t GetSignalSelectionSchema = 0x0D;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand00 = 0xD0;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand01 = 0xD1;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand02 = 0xD2;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand03 = 0xD3;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand04 = 0xD4;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand05 = 0xD5;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand06 = 0xD6;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand07 = 0xD7;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand08 = 0xD8;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand09 = 0xD9;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand10 = 0xDA;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand11 = 0xDB;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand12 = 0xDC;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand13 = 0xDD;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand14 = 0xDE;
        // Defines a command code handling user-defined commands.
        static constexpr uint8_t UserCommand15 = 0xDF;

        // Gets the ServerCommand enumeration value as a string.
        static std::string ToString(uint8_t command);
    };

    // Although the server commands and responses will be on two different paths, the response enumeration values
    // are defined as distinct from the command values to make it easier to identify codes from a wire analysis.

    // Enumeration of the possible server responses received sent by a DataPublisher and received by a DataSubscriber during an STTP session.
    struct ServerResponse
    {
        // Defines a response code for indicating a succeeded response.
	    // Informs client that its solicited server command succeeded, original command and success message follow.
        static constexpr uint8_t Succeeded = 0x80;
        // Defines a response code for indicating a failed response.
	    // Informs client that its solicited server command failed, original command and failure message follow.
        static constexpr uint8_t Failed = 0x81;
        // Defines a response code for indicating a data packet.
	    // Unsolicited response informs client that a data packet follows.
        static constexpr uint8_t DataPacket = 0x82;
        // Defines a response code for indicating a signal index cache update.
	    // Unsolicited response requests that client update its runtime signal index cache with the one that follows.
        static constexpr uint8_t UpdateSignalIndexCache = 0x83;
        // Defines a response code for indicating a runtime base-timestamp offsets have been updated.
	    // Unsolicited response requests that client update its runtime base-timestamp offsets with those that follow.
        static constexpr uint8_t UpdateBaseTimes = 0x84;
        // Defines a response code for indicating a runtime cipher keys have been updated.
	    // Response, solicited or unsolicited, requests that client update its runtime data cipher keys with those that follow.
        static constexpr uint8_t UpdateCipherKeys = 0x85;
        // Defines a response code for indicating the start time of data being published.
	    // Unsolicited response provides the start time of data being processed from the first measurement.
        static constexpr uint8_t DataStartTime = 0x86;
        // Defines a response code for indicating that processing has completed.
	    // Unsolicited response provides notification that input processing has completed, typically via temporal constraint.
        static constexpr uint8_t ProcessingComplete = 0x87;
        // Defines a response code for indicating a buffer block.
	    // Unsolicited response informs client that a raw buffer block follows.
        static constexpr uint8_t BufferBlock = 0x88;
        // Defines a response code for indicating a notification.
	    // Unsolicited response provides a notification message to the client.
        static constexpr uint8_t Notify = 0x89;
        // Defines a response code for indicating a that the publisher configuration metadata has changed.
	    // Unsolicited response provides a notification that the publisher's source configuration has changed and that client
	    // may want to request a meta-data refresh.
        static constexpr uint8_t ConfigurationChanged = 0x8A;
        // Codes for handling user-defined responses.
        static constexpr uint8_t UserResponse00 = 0xE0;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse01 = 0xE1;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse02 = 0xE2;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse03 = 0xE3;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse04 = 0xE4;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse05 = 0xE5;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse06 = 0xE6;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse07 = 0xE7;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse08 = 0xE8;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse09 = 0xE9;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse10 = 0xEA;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse11 = 0xEB;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse12 = 0xEC;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse13 = 0xED;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse14 = 0xEE;
        // Defines a response code handling user-defined responses.
        static constexpr uint8_t UserResponse15 = 0xEF;
        // Defines a response code for indicating a nil-operation keep-alive ping.
	    // The command channel can remain quiet for some time, this command allows a period test of client connectivity.
        static constexpr uint8_t NoOP = 0xFF;

        // Gets the ServerResponse enumeration value as a string.
        static std::string ToString(uint8_t response);
    };

    // Operational modes are sent from a subscriber to a publisher to request operational behaviors for the
    // connection, as a result the operation modes must be sent before any other command. The publisher may
    // silently refuse some requests (e.g., compression) based on its configuration. Operational modes only
    // apply to fundamental protocol control.

    // Enumeration of the possible modes that affect how DataPublisher and DataSubscriber communicate during an STTP session.
    struct OperationalModes
    {
        // Defines a bit mask used to get version number of protocol.
        static constexpr uint32_t VersionMask = 0x000000FF;
        // Defines a bit mask used to get version number of pre-IEEE standard implementations of protocol.
        static constexpr uint32_t PreStandardVersionMask = 0x0000001F;
        // Defines a bit mask used to get character encoding used when exchanging messages between publisher and subscriber.
	    // STTP currently only supports UTF-8 string encoding.
        static constexpr uint32_t EncodingMask = 0x00000300;
	    /// Bit mask used to apply an implementation-specific extension to STTP.
	    /// If the value is zero, no implementation specific extensions are applied.
	    /// If the value is non-zero, an implementation specific extension is applied, and all parties need to coordinate and agree to the extension.
	    /// If extended flags are unsupported, returned failure message text should be prefixed with UNSUPPORTED EXTENSION: as the context reference.
	    static constexpr uint32_t ImplementationSpecificExtensionMask = 0x00FF0000;
        // Defines a bit flag used to determine whether external measurements are exchanged during metadata synchronization.
    	// Bit set = external measurements are exchanged, bit clear = no external measurements are exchanged.
        static constexpr uint32_t ReceiveExternalMetadata = 0x02000000;
        // Defines a bit flag used to determine whether internal measurements are exchanged during metadata synchronization.
	    // Bit set = internal measurements are exchanged, bit clear = no internal measurements are exchanged.
        static constexpr uint32_t ReceiveInternalMetadata = 0x04000000;
        // Defines a bit flag used to determine whether payload data is compressed when exchanging between publisher and subscriber.
	    // Bit set = compress, bit clear = no compression.
        static constexpr uint32_t CompressPayloadData = 0x20000000;
        // Defines a bit flag used to determine whether the signal index cache is compressed when exchanging between publisher and subscriber.
	    // Bit set = compress, bit clear = no compression.
        static constexpr uint32_t CompressSignalIndexCache = 0x40000000;
        // Defines a bit flag used to determine whether metadata is compressed when exchanging between publisher and subscriber.
	    // Bit set = compress, bit clear = no compression.
        static constexpr uint32_t CompressMetadata = 0x80000000;
        // Defines state where there are no flags set.
        static constexpr uint32_t NoFlags = 0x00000000;
    };

    // Operational modes are sent from a subscriber to a publisher to request operational behaviors for the
    // connection, as a result the operation modes must be sent before any other command. The publisher may
    // silently refuse some requests (e.g., compression) based on its configuration. Operational modes only
    // apply to fundamental protocol control.

    // Enumeration of the possible string encoding options of an STTP session.
    struct OperationalEncoding
    {
	    // Targets little-endian 16-bit Unicode character encoding for strings.
	    // Deprecated: STTP currently only supports UTF-8 string encoding.
        static constexpr uint32_t UTF16LE = 0x00000000;
	    // Targets big-endian 16-bit Unicode character encoding for strings.
	    // Deprecated: STTP currently only supports UTF-8 string encoding.
        static constexpr uint32_t UTF16BE = 0x00000100;
        // Targets 8-bit variable-width Unicode character encoding for strings.
        static constexpr uint32_t UTF8 = 0x00000200;
    };

    // Enumeration of the possible compression modes supported by STTP.
    // Deprecated: Only used for backwards compatibility with pre-standard STTP implementations.
    // OperationalModes now supports custom compression types
    struct CompressionModes
    {
        // GZip compression
        static constexpr uint32_t GZip = 0x00000020;
        // TSSC compression
        static constexpr uint32_t TSSC = 0x00000040;
        // No compression
        static constexpr uint32_t None = 0x00000000;
    };

    // Enumeration of the possible security modes used by the DataPublisher
    // to secure data sent over the command channel in STTP.
    enum class SecurityMode
    {
        // Defines that data will be sent over the wire unencrypted.
        Off,
        // Defines that data will be sent over wire using Transport Layer Security.
        TLS
    };
}
