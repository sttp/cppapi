//******************************************************************************************************
//  CommonTypes.cpp - Gbtc
//
//  Copyright © 2019, Grid Protection Alliance.  All Rights Reserved.
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

// ReSharper disable CppClangTidyClangDiagnosticExitTimeDestructors
// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum
// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "CommonTypes.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#define BOOST_ALLOW_DEPRECATED_HEADERS  // NOLINT(clang-diagnostic-unused-macros)
#include <boost/uuid/uuid_generators.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace sttp;

boost::uuids::random_generator RandomGuidGen;
boost::uuids::nil_generator NilGuidGen;

const decimal_t Decimal::MaxValue = numeric_limits<decimal_t>::max();

const decimal_t Decimal::MinValue = numeric_limits<decimal_t>::min();

const decimal_t Decimal::DotNetMaxValue = decimal_t("79228162514264337593543950335");

const decimal_t Decimal::DotNetMinValue = decimal_t("-79228162514264337593543950335");

const datetime_t DateTime::MaxValue(max_date_time);

const datetime_t DateTime::MinValue(min_date_time);

const string Empty::String {};

const datetime_t Empty::DateTime {};

const Guid Empty::Guid = NilGuidGen();

const Object Empty::Object(nullptr);

const IPAddress Empty::IPAddress {};

size_t StringHash::operator()(const string& value) const
{
    size_t seed = 0;
    const locale locale;

    for (auto it : value)
        hash_combine(seed, toupper(it, locale));

    return seed;
}

bool StringEqual::operator()(const string& left, const string& right) const
{
    return IsEqual(left, right);
}

bool StringComparer::operator()(const std::string& left, const std::string& right) const
{
    return Compare(left, right) < 0;
}

uint32_t sttp::WriteBytes(vector<uint8_t>& buffer, const uint8_t* source, const uint32_t offset, const uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
        buffer.push_back(source[offset + i]);

    return length;
}

uint32_t sttp::WriteBytes(vector<uint8_t>& buffer, const vector<uint8_t>& source)
{
    const uint32_t length = ConvertUInt32(source.size());

    for (uint32_t i = 0; i < length; i++)
        buffer.push_back(source[i]);

    return length;
}

uint32_t sttp::WriteBytes(vector<uint8_t>& buffer, const Guid& value)
{
    const uint8_t* bytes = value.data;

    for (uint32_t i = 0; i < 16; i++)
        buffer.push_back(bytes[i]);

    return 16;
}

Guid sttp::NewGuid()
{
    return RandomGuidGen();
}

bool sttp::IsEmptyOrWhiteSpace(const string& value)
{
    if (value.empty())
        return true;

    return ranges::all_of(value, [](const char c)
    {
        return isspace(c);
    });
}

bool sttp::IsEqual(const string& left, const string& right, const bool ignoreCase)
{
    return ignoreCase ? 
        iequals(left, right) :
        equals(left, right);
}

bool sttp::StartsWith(const string& value, const string& findValue, const bool ignoreCase)
{
    return ignoreCase ? 
        istarts_with(value, findValue) :
        starts_with(value, findValue);
}

bool sttp::EndsWith(const string& value, const string& findValue, const bool ignoreCase)
{
    return ignoreCase ? 
        iends_with(value, findValue) :
        ends_with(value, findValue);
}

bool sttp::Contains(const string& value, const string& findValue, const bool ignoreCase)
{
    return ignoreCase ? 
        icontains(value, findValue) :
        contains(value, findValue);
}

int32_t sttp::Count(const string& value, const string& findValue, const bool ignoreCase)
{
    find_iterator<string::const_iterator> it = ignoreCase ? 
        make_find_iterator(value, first_finder(findValue, is_iequal())) :
        make_find_iterator(value, first_finder(findValue, is_equal()));

    const find_iterator<string::const_iterator> end {};
    int32_t count = 0;

    for (; it != end; ++it, ++count) //-V127
    {
    }

    return count;
}

int32_t sttp::Compare(const string& leftValue, const string& rightValue, const bool ignoreCase)
{
    if (ignoreCase)
    {
        if (ilexicographical_compare(leftValue, rightValue))
            return -1;

        if (ilexicographical_compare(rightValue, leftValue))
            return 1;

        return 0;
    }

    if (lexicographical_compare(leftValue, rightValue))
        return -1;

    if (lexicographical_compare(rightValue, leftValue))
        return 1;

    return 0;
}

int32_t sttp::IndexOf(const string& value, const string& findValue, const bool ignoreCase)
{
    const iterator_range<string::const_iterator> it = ignoreCase ?
        ifind_first(value, findValue) :
        find_first(value, findValue);

    if (it.empty())
        return -1;

    return ConvertInt32(std::distance(value.begin(), it.begin()));
}

int32_t sttp::IndexOf(const string& value, const string& findValue, int32_t index, const bool ignoreCase)
{
    const iterator_range<string::const_iterator> it = ignoreCase ? 
        ifind_nth(value, findValue, index) :
        find_nth(value, findValue, index);

    if (it.empty())
        return -1;

    return ConvertInt32(std::distance(value.begin(), it.begin()));
}

int32_t sttp::LastIndexOf(const string& value, const string& findValue, const bool ignoreCase)
{
    const iterator_range<string::const_iterator> it = ignoreCase ? 
        ifind_last(value, findValue) :
        find_last(value, findValue);

    if (it.empty())
        return -1;

    return ConvertInt32(std::distance(value.begin(), it.begin()));
}

vector<string> sttp::Split(const string& value, const string& delimiterValue, const bool ignoreCase)
{
    split_iterator<string::const_iterator> it = ignoreCase ?
        make_split_iterator(value, first_finder(delimiterValue, is_iequal())) :
        make_split_iterator(value, first_finder(delimiterValue, is_equal()));

    const split_iterator<string::const_iterator> end {};
    vector<string> values;

    for (; it != end; ++it)
    {
        values.push_back(copy_range<string>(*it));
    }

    return values;
}

string sttp::Split(const string& value, const string& delimiterValue, int32_t index, const bool ignoreCase)
{
    split_iterator<string::const_iterator> it = ignoreCase ?
        make_split_iterator(value, first_finder(delimiterValue, is_iequal())) :
        make_split_iterator(value, first_finder(delimiterValue, is_equal()));

    const split_iterator<string::const_iterator> end {};
    int32_t count = 0;

    for (; it != end; ++it, ++count) //-V127
    {
        if (count == index)
            return copy_range<string>(*it);
    }

    return string {};
}

string sttp::Replace(const string& value, const string& findValue, const string& replaceValue, const bool ignoreCase)
{
    return ignoreCase ? 
        ireplace_all_copy(value, findValue, replaceValue) :
        replace_all_copy(value, findValue, replaceValue);
}

string sttp::ToUpper(const string& value)
{
    return to_upper_copy(value);
}

string sttp::ToLower(const string& value)
{
    return to_lower_copy(value);
}

string sttp::Trim(const string& value)
{
    return trim_copy(value);
}

string sttp::Trim(const string& value, const string& trimValues)
{
    return trim_copy_if(value, is_any_of(trimValues));
}

string sttp::TrimRight(const string& value)
{
    return trim_right_copy(value);
}

string sttp::TrimRight(const string& value, const string& trimValues)
{
    return trim_right_copy_if(value, is_any_of(trimValues));
}

string sttp::TrimLeft(const string& value)
{
    return trim_left_copy(value);
}

string sttp::TrimLeft(const string& value, const string& trimValues)
{
    return trim_left_copy_if(value, is_any_of(trimValues));
}

string sttp::PadLeft(const string& value, const uint32_t count, const char padChar)
{
    if (value.size() < count)
        return string(count - value.size(), padChar) + value;

    return value;
}

string sttp::PadRight(const string& value, const uint32_t count, const char padChar)
{
    if (value.size() < count)
        return value + string(count - value.size(), padChar);

    return value;
}

datetime_t sttp::DateAdd(const datetime_t& value, const int32_t addValue, const TimeInterval interval)
{
    switch (interval)
    {
        case TimeInterval::Year:
            return value + years(addValue);
        case TimeInterval::Month:
            return value + months(addValue);
        case TimeInterval::DayOfYear:
        case TimeInterval::Day:
        case TimeInterval::WeekDay:
            return value + days(addValue);
        case TimeInterval::Week:
            return value + weeks(addValue);
        case TimeInterval::Hour:
            return value + hours(addValue);
        case TimeInterval::Minute:
            return value + minutes(addValue);
        case TimeInterval::Second:
            return value + seconds(addValue);
        case TimeInterval::Millisecond:
            return value + milliseconds(addValue);
        default:
            throw runtime_error("Unexpected time interval encountered");
    }
}

int32_t sttp::DateDiff(const datetime_t& startTime, const datetime_t& endTime, const TimeInterval interval)
{
    if (interval < TimeInterval::Hour)
    {
        switch (interval)
        {
            case TimeInterval::Year:
                return endTime.date().year() - startTime.date().year();
            case TimeInterval::Month:
                return DateDiff(startTime, endTime, TimeInterval::Year) * 12 + (endTime.date().month() - startTime.date().month());
            case TimeInterval::DayOfYear:
            case TimeInterval::Day:
            case TimeInterval::WeekDay:
                return (endTime.date() - startTime.date()).days();
            case TimeInterval::Week:
                return (endTime.date() - startTime.date()).days() / 7;
            default:
                throw runtime_error("Unexpected time interval encountered");
        }
    }

    const TimeSpan duration = endTime - startTime;

    switch (interval)
    {
        case TimeInterval::Hour:
            return static_cast<int32_t>(duration.hours());
        case TimeInterval::Minute:
            return static_cast<int32_t>(duration.total_seconds() / 60);
        case TimeInterval::Second:
            return static_cast<int32_t>(duration.total_seconds());
        case TimeInterval::Millisecond:
            return static_cast<int32_t>(duration.total_milliseconds());
        default:
            throw runtime_error("Unexpected time interval encountered");
    }
}

int32_t sttp::DatePart(const datetime_t& value, const TimeInterval interval)
{
    static const int64_t tickInterval = static_cast<int64_t>(pow(10LL, TimeSpan::num_fractional_digits()));

    switch (interval)
    {
        case TimeInterval::Year:
            return value.date().year();
        case TimeInterval::Month:
            return value.date().month();
        case TimeInterval::DayOfYear:
            return value.date().day_of_year();
        case TimeInterval::Day:
            return value.date().day();
        case TimeInterval::Week:
            return value.date().week_number();
        case TimeInterval::WeekDay:
            return value.date().day_of_week() + 1;
        case TimeInterval::Hour:
            return static_cast<int32_t>(value.time_of_day().hours());
        case TimeInterval::Minute:
            return static_cast<int32_t>(value.time_of_day().minutes());
        case TimeInterval::Second:
            return static_cast<int32_t>(value.time_of_day().seconds());
        case TimeInterval::Millisecond:
            return static_cast<int32_t>(value.time_of_day().fractional_seconds() * 1000LL / tickInterval);
        default:
            throw runtime_error("Unexpected time interval encountered");
    }
}

datetime_t sttp::Now()
{
    return datetime_t { microsec_clock::local_time() };
}

datetime_t sttp::UtcNow()
{
    return datetime_t { microsec_clock::universal_time() };
}
