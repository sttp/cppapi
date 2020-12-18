//******************************************************************************************************
//  DataRow.h - Gbtc
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
//  11/03/2018 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "../CommonTypes.h"
#include "../Nullable.h"
#include "DataColumn.h"

namespace sttp {
namespace filterexpressions
{
    class ValueExpression;
    typedef sttp::SharedPtr<ValueExpression> ValueExpressionPtr;

    class ExpressionTree;
    typedef sttp::SharedPtr<ExpressionTree> ExpressionTreePtr;
}}

namespace sttp {
namespace data
{
    enum class DataType;

    class DataTable;
    typedef sttp::SharedPtr<DataTable> DataTablePtr;

    class DataRow;
    typedef sttp::SharedPtr<DataRow> DataRowPtr;

    class DataRow : public sttp::EnableSharedThisPtr<DataRow> // NOLINT
    {
    private:
        DataTablePtr m_parent;
        std::vector<void*> m_values;

        int32_t GetColumnIndex(const std::string& columnName) const;
        DataColumnPtr ValidateColumnType(int32_t columnIndex, DataType targetType, bool read = false) const;

        sttp::filterexpressions::ExpressionTreePtr GetExpressionTree(const DataColumnPtr& column);
        sttp::Object GetComputedValue(const DataColumnPtr& column, DataType targetType);

        template<class T>
        sttp::Nullable<T> GetValue(int32_t columnIndex, DataType targetType);

        template<class T>
        void SetValue(int32_t columnIndex, const sttp::Nullable<T>& value, DataType targetType);
    public:
        DataRow(DataTablePtr parent);
        ~DataRow();

        const DataTablePtr& Parent() const;

        bool IsNull(int32_t columnIndex);
        bool IsNull(const std::string& columnName);
        void SetNullValue(int32_t columnIndex);
        void SetNullValue(const std::string& columnName);

        sttp::Nullable<std::string> ValueAsString(int32_t columnIndex);
        sttp::Nullable<std::string> ValueAsString(const std::string& columnName);
        void SetStringValue(int32_t columnIndex, const sttp::Nullable<std::string>& value);
        void SetStringValue(const std::string& columnName, const sttp::Nullable<std::string>& value);

        sttp::Nullable<bool> ValueAsBoolean(int32_t columnIndex);
        sttp::Nullable<bool> ValueAsBoolean(const std::string& columnName);
        void SetBooleanValue(int32_t columnIndex, const sttp::Nullable<bool>& value);
        void SetBooleanValue(const std::string& columnName, const sttp::Nullable<bool>& value);

        sttp::Nullable<sttp::datetime_t> ValueAsDateTime(int32_t columnIndex);
        sttp::Nullable<sttp::datetime_t> ValueAsDateTime(const std::string& columnName);
        void SetDateTimeValue(int32_t columnIndex, const sttp::Nullable<sttp::datetime_t>& value);
        void SetDateTimeValue(const std::string& columnName, const sttp::Nullable<sttp::datetime_t>& value);

        sttp::Nullable<sttp::float32_t> ValueAsSingle(int32_t columnIndex);
        sttp::Nullable<sttp::float32_t> ValueAsSingle(const std::string& columnName);
        void SetSingleValue(int32_t columnIndex, const sttp::Nullable<sttp::float32_t>& value);
        void SetSingleValue(const std::string& columnName, const sttp::Nullable<sttp::float32_t>& value);

        sttp::Nullable<sttp::float64_t> ValueAsDouble(int32_t columnIndex);
        sttp::Nullable<sttp::float64_t> ValueAsDouble(const std::string& columnName);
        void SetDoubleValue(int32_t columnIndex, const sttp::Nullable<sttp::float64_t>& value);
        void SetDoubleValue(const std::string& columnName, const sttp::Nullable<sttp::float64_t>& value);

        sttp::Nullable<sttp::decimal_t> ValueAsDecimal(int32_t columnIndex);
        sttp::Nullable<sttp::decimal_t> ValueAsDecimal(const std::string& columnName);
        void SetDecimalValue(int32_t columnIndex, const sttp::Nullable<sttp::decimal_t>& value);
        void SetDecimalValue(const std::string& columnName, const sttp::Nullable<sttp::decimal_t>& value);

        sttp::Nullable<sttp::Guid> ValueAsGuid(int32_t columnIndex);
        sttp::Nullable<sttp::Guid> ValueAsGuid(const std::string& columnName);
        void SetGuidValue(int32_t columnIndex, const sttp::Nullable<sttp::Guid>& value);
        void SetGuidValue(const std::string& columnName, const sttp::Nullable<sttp::Guid>& value);

        sttp::Nullable<int8_t> ValueAsInt8(int32_t columnIndex);
        sttp::Nullable<int8_t> ValueAsInt8(const std::string& columnName);
        void SetInt8Value(int32_t columnIndex, const sttp::Nullable<int8_t>& value);
        void SetInt8Value(const std::string& columnName, const sttp::Nullable<int8_t>& value);

        sttp::Nullable<int16_t> ValueAsInt16(int32_t columnIndex);
        sttp::Nullable<int16_t> ValueAsInt16(const std::string& columnName);
        void SetInt16Value(int32_t columnIndex, const sttp::Nullable<int16_t>& value);
        void SetInt16Value(const std::string& columnName, const sttp::Nullable<int16_t>& value);

        sttp::Nullable<int32_t> ValueAsInt32(int32_t columnIndex);
        sttp::Nullable<int32_t> ValueAsInt32(const std::string& columnName);
        void SetInt32Value(int32_t columnIndex, const sttp::Nullable<int32_t>& value);
        void SetInt32Value(const std::string& columnName, const sttp::Nullable<int32_t>& value);

        sttp::Nullable<int64_t> ValueAsInt64(int32_t columnIndex);
        sttp::Nullable<int64_t> ValueAsInt64(const std::string& columnName);
        void SetInt64Value(int32_t columnIndex, const sttp::Nullable<int64_t>& value);
        void SetInt64Value(const std::string& columnName, const sttp::Nullable<int64_t>& value);

        sttp::Nullable<uint8_t> ValueAsUInt8(int32_t columnIndex);
        sttp::Nullable<uint8_t> ValueAsUInt8(const std::string& columnName);
        void SetUInt8Value(int32_t columnIndex, const sttp::Nullable<uint8_t>& value);
        void SetUInt8Value(const std::string& columnName, const sttp::Nullable<uint8_t>& value);

        sttp::Nullable<uint16_t> ValueAsUInt16(int32_t columnIndex);
        sttp::Nullable<uint16_t> ValueAsUInt16(const std::string& columnName);
        void SetUInt16Value(int32_t columnIndex, const sttp::Nullable<uint16_t>& value);
        void SetUInt16Value(const std::string& columnName, const sttp::Nullable<uint16_t>& value);

        sttp::Nullable<uint32_t> ValueAsUInt32(int32_t columnIndex);
        sttp::Nullable<uint32_t> ValueAsUInt32(const std::string& columnName);
        void SetUInt32Value(int32_t columnIndex, const sttp::Nullable<uint32_t>& value);
        void SetUInt32Value(const std::string& columnName, const sttp::Nullable<uint32_t>& value);

        sttp::Nullable<uint64_t> ValueAsUInt64(int32_t columnIndex);
        sttp::Nullable<uint64_t> ValueAsUInt64(const std::string& columnName);
        void SetUInt64Value(int32_t columnIndex, const sttp::Nullable<uint64_t>& value);
        void SetUInt64Value(const std::string& columnName, const sttp::Nullable<uint64_t>& value);

        static const DataRowPtr NullPtr;
    };
}}

// Setup standard hash code for DataRowPtr
namespace std  // NOLINT
{
    template<>
    struct hash<sttp::data::DataRowPtr>
    {
        size_t operator () (const sttp::data::DataRowPtr& dataRow) const noexcept
        {
            return boost::hash<sttp::data::DataRowPtr>()(dataRow);
        }
    };
}