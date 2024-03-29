//******************************************************************************************************
//  TransportTypes.cpp - Gbtc
//
//  Copyright � 2019, Grid Protection Alliance.  All Rights Reserved.
//
//  Licensed to the Grid Protection Alliance (GPA) under one or more contributor license agreements. See
//  the NOTICE file distributed with this work for additional information regarding copyright ownership.
//  The GPA licenses this file to you under the MIT License (MIT), the "License"; you may
//  not use this file except in compliance with the License. You may obtain a copy of the License at:
//
//      http://opensource.org/licenses/MIT
//
//  Unless agreed to in writing, the subject software distributed under the License is distributed on an
//  "AS-IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. Refer to the
//  License for the specific language governing permissions and limitations.
//
//  Code Modification History:
//  ----------------------------------------------------------------------------------------------------
//  03/23/2018 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "TransportTypes.h"
#include "../Convert.h"

using namespace std;
using namespace sttp;
using namespace sttp::transport;

SubscriberException::SubscriberException(string message) noexcept :
    m_message(move(message))
{
}

const char* SubscriberException::what() const noexcept
{
    return m_message.data();
}

PublisherException::PublisherException(string message) noexcept :
    m_message(move(message))
{
}

const char* PublisherException::what() const noexcept
{
    return m_message.data();
}

Measurement::Measurement() :
    SignalID(Empty::Guid),
    Value(NaN),
    Adder(0.0),
    Multiplier(1.0)
{
}

Measurement::Measurement(const SimpleMeasurement& source) :
    SignalID(source.SignalID),
    Value(source.Value),
    Adder(0.0),
    Multiplier(1.0),
    Timestamp(source.Timestamp),
    Flags(source.Flags)
{
}

float64_t Measurement::AdjustedValue() const
{
    return Value * Multiplier + Adder;
}

datetime_t Measurement::GetDateTime() const
{
    return FromTicks(Timestamp);
}

void Measurement::GetUnixTime(time_t& unixSOC, uint16_t& milliseconds) const
{
    ToUnixTime(Timestamp, unixSOC, milliseconds);
}

void Measurement::GetSimpleMeasurement(SimpleMeasurement& measurement) const
{
    measurement.SignalID = SignalID;
    measurement.Value = AdjustedValue();
    measurement.Timestamp = Timestamp;
    measurement.Flags = Flags;
}

MeasurementPtr sttp::transport::ToPtr(const Measurement& source)
{
    MeasurementPtr destination = NewSharedPtr<Measurement>();

    destination->SignalID = source.SignalID;
    destination->Value = source.Value;
    destination->Adder = source.Adder;
    destination->Multiplier = source.Multiplier;
    destination->Timestamp = source.Timestamp;
    destination->Flags = source.Flags;
    destination->ID = source.ID;
    destination->Source = source.Source;
    destination->Tag = source.Tag;

    return destination;
}

MeasurementPtr sttp::transport::ToPtr(const SimpleMeasurement& source)
{
    MeasurementPtr destination = NewSharedPtr<Measurement>();

    destination->SignalID = source.SignalID;
    destination->Value = source.Value;
    destination->Adder = 0.0;
    destination->Multiplier = 1.0;
    destination->Timestamp = source.Timestamp;
    destination->Flags = source.Flags;

    return destination;
}

SignalReference::SignalReference() :
    SignalID(Empty::Guid)
{    
}

SignalReference::SignalReference(const string& signal) : SignalID(Guid())
{
    // Signal reference may contain multiple dashes, we're interested in the last one
    const auto splitIndex = signal.find_last_of('-');

    // Assign default values to fields
    Index = 0;

    if (splitIndex == string::npos)
    {
        // This represents an error - best we can do is assume entire string is the acronym
        Acronym = ToUpper(Trim(signal));
        Kind = SignalKind::Unknown;
    }
    else
    {
        const string signalType = ToUpper(Trim(signal.substr(splitIndex + 1)));
        Acronym = ToUpper(Trim(signal.substr(0, splitIndex)));

        // If the length of the signal type acronym is greater than 2, then this
        // is an indexed signal type (e.g., CORDOVA-PA2)
        if (signalType.length() > 2)
        {
            Kind = ParseSignalKind(signalType.substr(0, 2));

            if (Kind != SignalKind::Unknown)
                TryParseUInt16(signalType.substr(2), Index);
        }
        else
        {
            Kind = ParseSignalKind(signalType);
        }
    }
}

const char* sttp::transport::SignalKindDescription[] =
{
    "Angle",
    "Magnitude",
    "Frequency",
    "DfDt",
    "Status",
    "Digital",
    "Analog",
    "Calculation",
    "Statistic",
    "Alarm",
    "Quality",
    "Unknown"
};

const char* sttp::transport::SignalKindAcronym[] =
{
    "PA",
    "PM",
    "FQ",
    "DF",
    "SF",
    "DV",
    "AV",
    "CV",
    "ST",
    "AL",
    "QF",
    "??"
};

// SignalReference.ToString()
ostream& sttp::transport::operator << (ostream& stream, const SignalReference& reference)
{
    if (reference.Index > 0)
        return stream << reference.Acronym << "-" << SignalKindAcronym[static_cast<int32_t>(reference.Kind)] << reference.Index;

    return stream << reference.Acronym << "-" << SignalKindAcronym[static_cast<int32_t>(reference.Kind)];
}

string sttp::transport::GetSignalTypeAcronym(const SignalKind kind, const char phasorType)
{
    switch (kind)
    {
        case SignalKind::Angle:
            return toupper(phasorType) == 'V' ? "VPHA" : "IPHA";
        case SignalKind::Magnitude:
            return toupper(phasorType) == 'V' ? "VPHM" : "IPHM";
        case SignalKind::Frequency:
            return "FREQ";
        case SignalKind::DfDt:
            return "DFDT";
        case SignalKind::Status:
            return "FLAG";
        case SignalKind::Digital:
            return "DIGI";
        case SignalKind::Analog:
            return "ALOG";
        case SignalKind::Calculation:
            return "CALC";
        case SignalKind::Statistic:
            return "STAT";
        case SignalKind::Alarm:
            return "ALRM";
        case SignalKind::Quality:
            return "QUAL";
        case SignalKind::Unknown:
        default:
            return "NULL";
    }
}

std::string sttp::transport::GetEngineeringUnits(const std::string& signalType)
{
    if (IsEqual(signalType, "IPHM"))
        return "Amps";

    if (IsEqual(signalType, "VPHM"))
        return "Volts";

    if (IsEqual(signalType, "FREQ"))
        return "Hz";

    if (EndsWith(signalType, "PHA"))
        return "Degrees";

    return Empty::String;
}

std::string sttp::transport::GetProtocolType(const std::string& protocolName)
{
    if (IsEqual(protocolName, "Streaming Telemetry Transport Protocol") ||
        IsEqual(protocolName, "STTP") ||
        StartsWith(protocolName, "Gateway") ||
        StartsWith(protocolName, "Modbus") ||
        StartsWith(protocolName, "DNP"))
            return "Measurement";

    return "Frame";
}

void sttp::transport::ParseMeasurementKey(const std::string& key, std::string& source, uint64_t& id)
{
    const vector<string> parts = Split(key, ":");
    const size_t length = parts.size();

    if (length == 2)
    {
        source =  parts[0];
        TryParseUInt64(parts[1], id);
    }
    else if (length > 2)
    {
        stringstream sourceParts;

        for (size_t i = 0; i < length - 1; i++)
        {
            if (i > 0)
                sourceParts << ':';

            sourceParts << parts[i];
        }

        source =  sourceParts.str();
        TryParseUInt64(parts[length - 1], id);
    }
    else
    {
        if (TryParseUInt64(key, id))
        {
            source = Empty::String;
        }
        else
        {
            source = key;
            id = UInt64::MaxValue;
        }
    }
}

// Gets the "SignalKind" enum for the specified "acronym".
//  params:
//       acronym: Acronym of the desired "SignalKind"
//  returns: The "SignalKind" for the specified "acronym".
SignalKind sttp::transport::ParseSignalKind(const string& acronym)
{
    if (acronym == "PA") // Phase Angle
        return SignalKind::Angle;

    if (acronym == "PM") // Phase Magnitude
        return SignalKind::Magnitude;

    if (acronym == "FQ") // Frequency
        return SignalKind::Frequency;

    if (acronym == "DF") // dF/dt
        return SignalKind::DfDt;

    if (acronym == "SF") // Status Flags
        return SignalKind::Status;

    if (acronym == "DV") // Digital Value
        return SignalKind::Digital;

    if (acronym == "AV") // Analog Value
        return SignalKind::Analog;

    if (acronym == "CV") // Calculated Value
        return SignalKind::Calculation;

    if (acronym == "ST") // Statistical Value
        return SignalKind::Statistic;

    if (acronym == "AL") // Alarm Value
        return SignalKind::Alarm;

    if (acronym == "QF") // Quality Flags
        return SignalKind::Quality;

    return SignalKind::Unknown;
}
