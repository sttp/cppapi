//******************************************************************************************************
//  Convert.cpp - Gbtc
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
//  04/06/2012 - Stephen C. Wills
//       Generated original version of source code.
//
//******************************************************************************************************

// ReSharper disable CppClangTidyClangDiagnosticExitTimeDestructors

#include "Convert.h"
#include <iomanip>
#include <sstream>
#include <codecvt>
#include <regex>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/algorithm/string.hpp>
#include "utf8.h"

using namespace std;
using namespace std::chrono;
using namespace boost::uuids;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace sttp;

const datetime_t DateTimeEpoch(date(1400, 1, 1), TimeSpan(0, 0, 0));
const auto DateTimeTicksPerSecond = TimeSpan::ticks_per_second();

inline int GetRadix(const string& value)
{
    return StartsWith(value, "0x") ? 16 : 10;
}

string PreparseTimestamp(const string& timestamp, TimeSpan& utcOffset)
{
    // 2018-03-14T19:23:11.665-04:00
    vector<string> dateTimeParts = Split(Replace(timestamp, "T", " "), " ", false);

    // Failed to understand timestamp format, just return input
    if (dateTimeParts.empty() || dateTimeParts.size() > 2)
        return timestamp;

    string updatedTimestamp{};
    string& datePart = dateTimeParts[0];
    string part{};
    vector<string> dateParts = Split(Replace(datePart, "/", "-", false), "-", false);

    if (dateParts.size() != 3)
        return timestamp;

    string year, month, day;

    for (int32_t i = 0; i < 3; i++)
    {
        part = dateParts[i];

        if (part.size() == 1)
            part.insert(0, "0");

        if (part.size() == 4)
            year = part;
        else if (month.empty())
            month = part;
        else
            day = part;
    }

    updatedTimestamp.append(year);
    updatedTimestamp.append("-");
    updatedTimestamp.append(month);
    updatedTimestamp.append("-");
    updatedTimestamp.append(day);

    if (dateTimeParts.size() == 1)
    {
        updatedTimestamp.append(" 00:00:00");
        return updatedTimestamp;
    }

    string& timePart = dateTimeParts[1];

    // Remove any time zone offset and hold on to it for later
    const bool containsMinus = Contains(timePart, "-", false);
    vector<string> timeParts = Split(timePart, containsMinus ? "-" : "+", false);
    string timeZoneOffset{};

    if (timeParts.size() == 2)
    {
        timePart = timeParts[0];

        // Swap timezone sign for conversion to UTC
        timeZoneOffset.append(containsMinus ? "+" : "-");
        timeZoneOffset.append(Replace(timeParts[1], ":", "", false));
    }

    timeParts = Split(timePart, ":", false);

    if (timeParts.size() == 2)
        timeParts.emplace_back("00");

    if (timeParts.size() != 3)
        return timestamp;

    updatedTimestamp.append(" ");

    for (int32_t i = 0; i < 3; i++)
    {
        string fractionalSeconds{};
        part = timeParts[i];

        if (i == 2 && Contains(part, ".", false))
        {
            vector<string> secondParts = Split(part, ".", false);

            if (secondParts.size() == 2)
            {
                part = secondParts[0];
                fractionalSeconds.append(".");
                fractionalSeconds.append(secondParts[1]);
            }
        }

        if (i > 0)
            updatedTimestamp.append(":");

        if (part.size() == 1)
            updatedTimestamp.append("0");

        updatedTimestamp.append(part);

        if (!fractionalSeconds.empty())
            updatedTimestamp.append(fractionalSeconds);
    }

    if (timeZoneOffset.size() == 5)
    {
        int32_t hour, minute;

        if (TryParseInt32(timeZoneOffset.substr(0, 3), hour) && TryParseInt32(timeZoneOffset.substr(3), minute))
            utcOffset = TimeSpan(hour, minute, 0);
    }

    return updatedTimestamp;
}

void sttp::ToUnixTime(const int64_t ticks, time_t& unixSOC, uint16_t& milliseconds)
{
    // Unix dates are measured as the number of seconds since 1/1/1970
    unixSOC = (ticks - Ticks::UnixBaseOffset) / Ticks::PerSecond;

    if (unixSOC < 0)
        unixSOC = 0;

    milliseconds = static_cast<uint16_t>(ticks / 10000 % 1000);
}

datetime_t sttp::FromUnixTime(const time_t unixSOC, const uint16_t milliseconds)
{
    return from_time_t(unixSOC) + Milliseconds(milliseconds);
}

datetime_t sttp::FromTicks(const int64_t ticks)
{
    static float64_t ticksPerSecondF = static_cast<float64_t>(Ticks::PerSecond);
    const datetime_t time = from_time_t((ticks - Ticks::UnixBaseOffset) / Ticks::PerSecond);
    const int64_t pticks = static_cast<int64_t>(ticks % Ticks::PerSecond / ticksPerSecondF * DateTimeTicksPerSecond); // NOLINT
    return time + TimeSpan(0, 0, 0, pticks % DateTimeTicksPerSecond);
}

int64_t sttp::ToTicks(const datetime_t& time)
{
    static int64_t tickInterval = static_cast<int64_t>(pow(10LL, TimeSpan::num_fractional_digits()));
    const TimeSpan offset = time - DateTimeEpoch;
    return Ticks::PTimeBaseOffset + offset.total_seconds() * Ticks::PerSecond +
        static_cast<int64_t>(offset.fractional_seconds() / tickInterval * Ticks::PerSecond);
}

bool sttp::TimestampIsReasonable(const int64_t value, const float64_t lagTime, const float64_t leadTime, const bool utc)
{
    static const float64_t ticksPerSecondF = static_cast<float64_t>(Ticks::PerSecond);

    if (lagTime <= 0)
        throw runtime_error("lagTime must be greater than zero, but it can be less than one");

    if (leadTime <= 0)
        throw runtime_error("leadTime must be greater than zero, but it can be less than one");

    // Calculate timestamp distance from local system time in seconds
    const float64_t distance = (ToTicks(utc ? UtcNow() : Now()) - value) / ticksPerSecondF; // NOLINT
    return distance >= -leadTime && distance <= lagTime;
}

bool sttp::TimestampIsReasonable(const datetime_t& value, const float64_t lagTime, const float64_t leadTime, const bool utc)
{
    return TimestampIsReasonable(ToTicks(value), lagTime, leadTime);
}

int64_t sttp::RoundToSubsecondDistribution(const int64_t ticks, const int32_t samplesPerSecond)
{
    // Baseline timestamp to the top of the second
    const int64_t baseTicks = ticks - ticks % Ticks::PerSecond;

    // Remove the whole seconds from ticks
    const float64_t ticksBeyondSecond = static_cast<float64_t>(ticks - baseTicks);

    // Calculate a frame index between 0 and m_framesPerSecond - 1,
    // corresponding to ticks rounded to the nearest frame
    const int64_t frameIndex = static_cast<int64_t>(round(ticksBeyondSecond / (Ticks::PerSecond / static_cast<float64_t>(samplesPerSecond))));

    // Calculate the timestamp of the nearest frame
    int64_t destinationTicks = frameIndex * Ticks::PerSecond / samplesPerSecond;

    // Recover the seconds that were removed
    destinationTicks += baseTicks;

    return destinationTicks;
}

uint32_t sttp::TicksToString(char* ptr, const uint32_t maxsize, string format, const int64_t ticks)
{
    time_t fromSeconds;
    uint16_t milliseconds;

    ToUnixTime(ticks, fromSeconds, milliseconds);

    stringstream formatStream;
    uint32_t formatIndex = 0;

    while (formatIndex < format.size())
    {
        char c = format[formatIndex];
        ++formatIndex;

        if (c != '%')
        {
            // Not a format specifier
            formatStream << c;
            continue;
        }

        // Check for %f and %t format specifiers and handle them
        // accordingly. All other specifiers get forwarded to strftime
        c = format[formatIndex];
        ++formatIndex;

        switch (c)
        {
            case 'f':
            {
                stringstream temp;
                temp << setw(3) << setfill('0') << static_cast<int>(milliseconds);
                formatStream << temp.str();
                break;
            }

            case 't':
                formatStream << ticks;
                break;

            default:
                formatStream << '%' << c;
                break;
        }
    }

    struct tm timeinfo{};

#ifdef _WIN32
    gmtime_s(&timeinfo, &fromSeconds);
#else
    gmtime_r(&fromSeconds, &timeinfo);
#endif

    return ConvertUInt32(strftime(ptr, maxsize, formatStream.str().data(), &timeinfo));
}

datetime_t sttp::LocalFromUtc(const datetime_t& timestamp)
{
    return boost::date_time::c_local_adjustor<datetime_t>::utc_to_local(timestamp);
}

string sttp::ToString(const Guid& value)
{
    return boost::uuids::to_string(value);
}

string sttp::ToString(const datetime_t& value, const char* format)
{
    stringstream stream;
    time_facet* facet = new time_facet(format);
    
    auto _ = stream.imbue(locale(stream.getloc(), facet));
    stream << value;

    return stream.str();
}

string sttp::ToString(const TimeSpan& value)
{
    // TODO: Consider improving elapsed time string with hours, minutes, etc.
    const float64_t seconds = static_cast<float64_t>(value.total_milliseconds()) / 1000.0;
    return ToString(seconds) + " seconds";
}

string sttp::ToString(const decimal_t& value)
{
    return value.str();
}

wstring sttp::ToUTF16(const string& value)
{
    // wstring_convert is deprecated in C++17.
    //wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    //return converter.from_bytes(value);

    wstring wide;
    utf8::utf8to32(value.begin(), value.end(), std::back_inserter(wide));
    return wide;
}

string sttp::ToUTF8(const wstring& value)
{
    // wstring_convert is deprecated in C++17.
    //wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    //return converter.to_bytes(value);

    string narrow;
    utf8::utf32to8(value.begin(), value.end(), std::back_inserter(narrow));
    return narrow;
}

bool sttp::ParseBoolean(const string& value)
{
    bool result;
    TryParseBoolean(value, result);
    return result;
}

bool sttp::TryParseBoolean(const string& value, bool& result, const bool defaultValue)
{
    if (value.empty())
    {
        result = defaultValue;
        return false;
    }

    if (IsEqual(value, "true"))
    {
        result = true;
        return true;
    }

    if (IsEqual(value, "false"))
    {
        result = false;
        return true;
    }

    int32_t i32Val;

    if (TryParseInt32(value, i32Val))
    {
        result = i32Val != 0;
        return true;
    }

    const char first = static_cast<char>(toupper(value[0]));
    return first == 'T' || first == 'Y';
}

bool sttp::IsInteger(const string& value)
{
    int64_t i64Val;
    uint64_t ui64Val;

    return TryParseInt64(value, i64Val) || TryParseUInt64(value, ui64Val);
}

bool sttp::IsNumeric(const string& value)
{
    float64_t f64Val;

    return TryParseDouble(value, f64Val);
}

bool sttp::TryParseUInt16(const string& value, uint16_t& result, const uint16_t defaultValue)
{
    try
    {
        const auto conversion = stoul(value, nullptr, GetRadix(value));

        if (conversion > UInt16::MaxValue)
        {
            result = defaultValue;
            return false;
        }

        result = static_cast<uint16_t>(conversion);
        return true;
    }
    catch (...)
    {
        result = defaultValue;
        return false;
    }
}

bool sttp::TryParseInt32(const string& value, int32_t& result, const int32_t defaultValue)
{
    try
    {
        result = stoi(value, nullptr, GetRadix(value));
        return true;
    }
    catch (...)
    {
        result = defaultValue;
        return false;
    }
}

bool sttp::TryParseUInt32(const string& value, uint32_t& result, const uint32_t defaultValue)
{
    try
    {
        result = stoul(value, nullptr, GetRadix(value));
        return true;
    }
    catch (...)
    {
        result = defaultValue;
        return false;
    }
}

bool sttp::TryParseInt64(const string& value, int64_t& result, const int64_t defaultValue)
{
    try
    {
        result = stoll(value, nullptr, GetRadix(value));
        return true;
    }
    catch (...)
    {
        result = defaultValue;
        return false;
    }
}

bool sttp::TryParseUInt64(const string& value, uint64_t& result, const uint64_t defaultValue)
{
    try
    {
        result = stoull(value);
        return true;
    }
    catch (...)
    {
        result = defaultValue;
        return false;
    }
}

bool sttp::TryParseDouble(const string& value, float64_t& result, const float64_t defaultValue)
{
    try
    {
        result = stod(value);
        return true;
    }
    catch (...)
    {
        result = defaultValue;
        return false;
    }
}

bool sttp::TryParseDecimal(const string& value, decimal_t& result, const decimal_t defaultValue)
{
    try
    {
        result = decimal_t(value);
        return true;
    }
    catch (...)
    {
        result = defaultValue;
        return false;
    }
}

string sttp::RegExEncode(const char value)
{
    stringstream stream;
    stream << std::hex << static_cast<int>(value);
    return "\\u" + PadLeft(stream.str(), 4, '0');
}

bool sttp::IsGuid(const string& value)
{
    Guid guidVal;

    return TryParseGuid(value, guidVal);
}

Guid sttp::ParseGuid(const uint8_t* data, const bool swapEndianness)
{
    Guid id;
    uint8_t swappedBytes[16];
    uint8_t* encodedBytes;

    if (swapEndianness)
    {
        uint8_t copy[8];

        for (uint32_t i = 0; i < 16; i++)
        {
            swappedBytes[i] = data[i];

            if (i < 8)
                copy[i] = swappedBytes[i];
        }

        // Convert Microsoft encoding to RFC
        swappedBytes[3] = copy[0];
        swappedBytes[2] = copy[1];
        swappedBytes[1] = copy[2];
        swappedBytes[0] = copy[3];

        swappedBytes[4] = copy[5];
        swappedBytes[5] = copy[4];

        swappedBytes[6] = copy[7];
        swappedBytes[7] = copy[6];

        encodedBytes = swappedBytes;
    }
    else
    {
        encodedBytes = const_cast<uint8_t*>(data);
    }

    for (Guid::iterator iter = id.begin(); iter != id.end(); ++iter, ++encodedBytes)
        *iter = static_cast<Guid::value_type>(*encodedBytes);

    return id;
}

Guid sttp::ParseGuid(const char* data)
{
    const string_generator generator;
    return generator(data);
}

bool sttp::TryParseGuid(const string& value, Guid& result, const Guid defaultValue)
{
    try
    {
        result = ParseGuid(value.c_str());
        return true;
    }
    catch (...)
    {
        result = defaultValue;
        return false;
    }
}

void sttp::SwapGuidEndianness(Guid& value)
{
    uint8_t* data = value.data;
    uint8_t copy[8];

    for (uint32_t i = 0; i < 8; i++)
        copy[i] = data[i];

    // The following uint32 and two uint16 values are little-endian encoded in Microsoft implementations,
    // boost follows RFC encoding rules and encodes the bytes as big-endian. For proper Guid interpretation
    // by .NET applications the following bytes must be swapped before wire-serialization:
    data[3] = copy[0];
    data[2] = copy[1];
    data[1] = copy[2];
    data[0] = copy[3];

    data[4] = copy[5];
    data[5] = copy[4];

    data[6] = copy[7];
    data[7] = copy[6];
}

const char* sttp::Coalesce(const char* data, const char* nonEmptyValue)
{
    if (data == nullptr)
        return nonEmptyValue;

    if (data[0] == '\0')
        return nonEmptyValue;

    return data;
}

// Attempt to parse a timestamp string, e.g.: 2018-03-14T19:23:11.665-04:00
bool sttp::TryParseTimestamp(const char* time, datetime_t& timestamp, const datetime_t& defaultValue, const bool parseAsUTC)
{
    static const locale formats[] = {
        locale(locale::classic(), new time_input_facet("%Y-%m-%d %H:%M:%S%F")),
        locale(locale::classic(), new time_input_facet("%Y%m%dT%H%M%S%F"))
    };

    TimeSpan utcOffset{};
    const string cleanTimestamp = PreparseTimestamp(time, utcOffset);

    for (const locale& format : formats)
    {
        istringstream stream(cleanTimestamp);

        auto _ = stream.imbue(format);
        stream >> timestamp;

        if (static_cast<bool>(stream))
        {
            if (parseAsUTC)
                timestamp += utcOffset;

            return true;
        }
    }

    timestamp = defaultValue;
    return false;
}

datetime_t sttp::ParseTimestamp(const char* time, const bool parseAsUTC)
{
    datetime_t timestamp;

    if (TryParseTimestamp(time, timestamp, DateTime::MinValue, parseAsUTC))
        return timestamp;

    throw runtime_error("Failed to parse timestamp \"" + string(time) + "\"");
}

datetime_t sttp::ParseRelativeTimestamp(const char* time, const datetime_t& defaultValue)
{
    static const regex expression(R"(\*\s*([+-]?\d+)\s*(\w+))");
    datetime_t timestamp;

    if (TryParseTimestamp(time, timestamp, defaultValue, true))
        return timestamp;

    const datetime_t now = UtcNow();
    const string timetag = Trim(time);
    smatch match;

    if (IsEqual(timetag, "*", false))
        return now;

    if (regex_search(timetag, match, expression) && match.size() == 3)
    {
        int32_t offset;
        TryParseInt32(match.str(1), offset);

        const char unit = ToLower(Trim(match.str(2)))[0];

        switch (unit)
        {
            case 's':
                timestamp = DateAdd(now, offset, TimeInterval::Second);
                break;
            case 'm':
                timestamp = DateAdd(now, offset, TimeInterval::Minute);
                break;
            case 'h':
                timestamp = DateAdd(now, offset, TimeInterval::Hour);
                break;
            case 'd':
                timestamp = DateAdd(now, offset, TimeInterval::Day);
                break;
            default:
                timestamp = defaultValue;
                break;
        }
    }
    else
    {
        timestamp = defaultValue;
    }

    return timestamp;
}

StringMap<string> sttp::ParseKeyValuePairs(const string& value, const char parameterDelimiter, const char keyValueDelimiter, const char startValueDelimiter, const char endValueDelimiter)
{
    if (parameterDelimiter == keyValueDelimiter ||
        parameterDelimiter == startValueDelimiter ||
        parameterDelimiter == endValueDelimiter ||
        keyValueDelimiter == startValueDelimiter ||
        keyValueDelimiter == endValueDelimiter ||
        startValueDelimiter == endValueDelimiter)
        throw invalid_argument("All delimiters must be unique");

    const string& escapedParameterDelimiter = RegExEncode(parameterDelimiter);
    const string& escapedKeyValueDelimiter = RegExEncode(keyValueDelimiter);
    const string& escapedStartValueDelimiter = RegExEncode(startValueDelimiter);
    const string& escapedEndValueDelimiter = RegExEncode(endValueDelimiter);
    const string& escapedBackslashDelimiter = RegExEncode('\\');
    const string& parameterDelimiterStr = string(1, parameterDelimiter);
    const string& keyValueDelimiterStr = string(1, keyValueDelimiter);
    const string& startValueDelimiterStr = string(1, startValueDelimiter);
    const string& endValueDelimiterStr = string(1, endValueDelimiter);
    const string& backslashDelimiterStr = "\\";

    StringMap<string> keyValuePairs;
    vector<string> escapedValue;
    bool valueEscaped = false;
    uint32_t delimiterDepth = 0;

    // Escape any parameter or key/value delimiters within tagged value sequences
    //      For example, the following string:
    //          "normalKVP=-1; nestedKVP={p1=true; p2=false}")
    //      would be encoded as:
    //          "normalKVP=-1; nestedKVP=p1\\u003dtrue\\u003b p2\\u003dfalse")
    for (const char character : value)
    {
        if (character == startValueDelimiter)
        {
            if (!valueEscaped)
            {
                valueEscaped = true;
                continue;   // Don't add tag start delimiter to final value
            }

            // Handle nested delimiters
            delimiterDepth++;
        }

        if (character == endValueDelimiter)
        {
            if (valueEscaped)
            {
                if (delimiterDepth > 0)
                {
                    // Handle nested delimiters
                    delimiterDepth--;
                }
                else
                {
                    valueEscaped = false;
                    continue;   // Don't add tag stop delimiter to final value
                }
            }
            else
            {
                throw runtime_error("Failed to parse key/value pairs: invalid delimiter mismatch. Encountered end value delimiter '" + endValueDelimiterStr + "' before start value delimiter '" + startValueDelimiterStr + "'.");  // NOLINT
            }
        }

        if (valueEscaped)
        {
            // Escape any delimiter characters inside nested key/value pair
            if (character == parameterDelimiter)
                escapedValue.push_back(escapedParameterDelimiter);
            else if (character == keyValueDelimiter)
                escapedValue.push_back(escapedKeyValueDelimiter);
            else if (character == startValueDelimiter)
                escapedValue.push_back(escapedStartValueDelimiter);
            else if (character == endValueDelimiter)
                escapedValue.push_back(escapedEndValueDelimiter);
            else if (character == '\\')
                escapedValue.push_back(escapedBackslashDelimiter);
            else
                escapedValue.emplace_back(1, character);
        }
        else
        {
            if (character == '\\')
                escapedValue.push_back(escapedBackslashDelimiter);
            else
                escapedValue.emplace_back(1, character);
        }
    }

    if (delimiterDepth != 0 || valueEscaped)
    {
        // If value is still escaped, tagged expression was not terminated
        if (valueEscaped)
            delimiterDepth = 1;

        const bool moreStartDelimiters = delimiterDepth > 0;

        throw runtime_error(
            "Failed to parse key/value pairs: invalid delimiter mismatch. Encountered more " +
            (moreStartDelimiters ? "start value delimiters '" + startValueDelimiterStr + "'" : "end value delimiters '" + endValueDelimiterStr + "'") + " than " +
            (moreStartDelimiters ? "end value delimiters '" + endValueDelimiterStr + "'" : "start value delimiters '" + startValueDelimiterStr + "'") + ".");
    }

    // Parse key/value pairs from escaped value
    vector<string> pairs = Split(boost::algorithm::join(escapedValue, ""), parameterDelimiterStr, false);

    for (const string& pair : pairs)
    {
        // Separate key from value
        vector<string> elements = Split(pair, keyValueDelimiterStr, false);

        if (elements.size() == 2)
        {
            // Get key
            const string key = Trim(elements[0]);

            // Get unescaped value
            const string unescapedValue = Replace(Replace(Replace(Replace(Replace(Trim(elements[1]),
                escapedParameterDelimiter, parameterDelimiterStr, false),
                escapedKeyValueDelimiter, keyValueDelimiterStr, false),
                escapedStartValueDelimiter, startValueDelimiterStr, false),
                escapedEndValueDelimiter, endValueDelimiterStr, false),                    
                escapedBackslashDelimiter, backslashDelimiterStr, false);

            // Add or replace key elements with unescaped value
            keyValuePairs[key] = unescapedValue;
        }
    }

    return keyValuePairs;
}
