//******************************************************************************************************
//  ExpressionTree.h - Gbtc
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
//  12/02/2018 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

#include "../../CommonTypes.h"
#include "../../data/DataSet.h"

namespace sttp {
namespace filterexpressions
{
    // Simple exception type thrown by the expression tree
    class ExpressionTreeException : public Exception
    {
    private:
        std::string m_message;

    public:
        ExpressionTreeException(std::string message) noexcept;
        const char* what() const noexcept;
    };

    enum class ExpressionType
    {
        Value,
        Unary,
        Column,
        InList,
        Function,
        Operator
    };

    // Base class for all expression types
    class Expression // NOLINT
    {
    public:
        explicit Expression(ExpressionType type);
        virtual ~Expression();

        const ExpressionType Type;
    };

    typedef sttp::SharedPtr<Expression> ExpressionPtr;
    typedef std::vector<ExpressionPtr> ExpressionCollection;
    typedef sttp::SharedPtr<ExpressionCollection> ExpressionCollectionPtr;

    // These expression value data types are reduced to a reasonable set of possible types
    // that can be represented in a filter expression. All data table column values will be
    // mapped to these types. The behavior of these types has been modeled to operate similar
    // to parsing of literal expressions in .NET DataSet operations, see:
    // https://docs.microsoft.com/en-us/dotnet/api/system.data.datacolumn.expression#parsing-literal-expressions
    enum class ExpressionValueType
    {
        Boolean,    // bool
        Int32,      // int32_t
        Int64,      // int64_t
        Decimal,    // decimal_t
        Double,     // float64_t
        String,     // string
        Guid,       // Guid
        DateTime,   // DateTime
        Undefined   // nullptr (make sure value is always last in enum)
    };

    const extern int32_t ExpressionValueTypeLength;
    const extern char* ExpressionValueTypeAcronym[];
    const char* EnumName(ExpressionValueType valueType);

    bool IsIntegerType(ExpressionValueType valueType);
    bool IsNumericType(ExpressionValueType valueType);

    class ValueExpression : public Expression
    {
    private:
        void ValidateValueType(ExpressionValueType valueType) const;

    public:
        ValueExpression(ExpressionValueType valueType, sttp::Object value, bool valueIsNullable = false);

        const sttp::Object Value;
        const ExpressionValueType ValueType;
        const bool ValueIsNullable;

        bool IsNull() const;
        std::string ToString() const;

        // The following functions are value type validated
        bool ValueAsBoolean() const;
        Nullable<bool> ValueAsNullableBoolean() const;

        int32_t ValueAsInt32() const;
        Nullable<int32_t> ValueAsNullableInt32() const;

        int64_t ValueAsInt64() const;
        Nullable<int64_t> ValueAsNullableInt64() const;

        sttp::decimal_t ValueAsDecimal() const;
        Nullable<sttp::decimal_t> ValueAsNullableDecimal() const;

        sttp::float64_t ValueAsDouble() const;
        Nullable<sttp::float64_t> ValueAsNullableDouble() const;

        std::string ValueAsString() const;
        Nullable<std::string> ValueAsNullableString() const;

        sttp::Guid ValueAsGuid() const;
        Nullable<sttp::Guid> ValueAsNullableGuid() const;

        sttp::datetime_t ValueAsDateTime() const;
        Nullable<sttp::datetime_t> ValueAsNullableDateTime() const;
    };

    typedef sttp::SharedPtr<ValueExpression> ValueExpressionPtr;

    enum class ExpressionUnaryType
    {
        Plus,
        Minus,
        Not
    };

    const extern char* ExpressionUnaryTypeAcronym[];
    const char* EnumName(ExpressionUnaryType unaryType);

    class UnaryExpression : public Expression
    {
    public:
        UnaryExpression(ExpressionUnaryType unaryType, ExpressionPtr value);

        const ExpressionUnaryType UnaryType;
        const ExpressionPtr Value;
    };

    typedef sttp::SharedPtr<UnaryExpression> UnaryExpressionPtr;

    class ColumnExpression : public Expression
    {
    public:
        ColumnExpression(sttp::data::DataColumnPtr dataColumn);

        const sttp::data::DataColumnPtr DataColumn;
    };

    typedef sttp::SharedPtr<ColumnExpression> ColumnExpressionPtr;

    class InListExpression : public Expression
    {
    public:
        InListExpression(ExpressionPtr value, ExpressionCollectionPtr arguments, bool hasNotKeyword, bool exactMatch);

        const ExpressionPtr Value;
        const ExpressionCollectionPtr Arguments;
        const bool HasNotKeyword;
        const bool ExactMatch;
    };

    typedef sttp::SharedPtr<InListExpression> InListExpressionPtr;

    enum class ExpressionFunctionType
    {
        Abs,
        Ceiling,
        Coalesce,
        Convert,
        Contains,
        DateAdd,
        DateDiff,
        DatePart,
        EndsWith,
        Floor,
        IIf,
        IndexOf,
        IsDate,
        IsInteger,
        IsGuid,
        IsNull,
        IsNumeric,
        LastIndexOf,
        Len,
        Lower,
        MaxOf,
        MinOf,
        Now,
        NthIndexOf,
        Power,
        RegExMatch,
        RegExVal,
        Replace,
        Reverse,
        Round,
        Split,
        Sqrt,
        StartsWith,
        StrCount,
        StrCmp,
        SubStr,
        Trim,
        TrimLeft,
        TrimRight,
        Upper,
        UtcNow
    };

    class FunctionExpression : public Expression
    {
    public:
        FunctionExpression(ExpressionFunctionType functionType, ExpressionCollectionPtr arguments);

        const ExpressionFunctionType FunctionType;
        const ExpressionCollectionPtr Arguments;
    };

    typedef sttp::SharedPtr<FunctionExpression> FunctionExpressionPtr;

    enum class ExpressionOperatorType
    {
        Multiply,
        Divide,
        Modulus,
        Add,
        Subtract,
        BitShiftLeft,
        BitShiftRight,
        BitwiseAnd,
        BitwiseOr,
        BitwiseXor,
        LessThan,
        LessThanOrEqual,
        GreaterThan,
        GreaterThanOrEqual,
        Equal,
        EqualExactMatch,
        NotEqual,
        NotEqualExactMatch,
        IsNull,
        IsNotNull,
        Like,
        LikeExactMatch,
        NotLike,
        NotLikeExactMatch,
        And,
        Or
    };

    const extern char* ExpressionOperatorTypeAcronym[];
    const char* EnumName(ExpressionOperatorType operatorType);

    class OperatorExpression : public Expression
    {
    public:
        OperatorExpression(ExpressionOperatorType operatorType, ExpressionPtr leftValue, ExpressionPtr rightValue);

        const ExpressionOperatorType OperatorType;
        const ExpressionPtr LeftValue;
        const ExpressionPtr RightValue;
    };

    typedef sttp::SharedPtr<OperatorExpression> OperatorExpressionPtr;

    class ExpressionTree
    {
    private:
        sttp::data::DataRowPtr m_currentRow;
        sttp::data::DataTablePtr m_table;

        ValueExpressionPtr Evaluate(const ExpressionPtr& expression, ExpressionValueType targetValueType = ExpressionValueType::Boolean) const;
        ValueExpressionPtr EvaluateUnary(const ExpressionPtr& expression) const;
        ValueExpressionPtr EvaluateColumn(const ExpressionPtr& expression) const;
        ValueExpressionPtr EvaluateInList(const ExpressionPtr& expression) const;
        ValueExpressionPtr EvaluateFunction(const ExpressionPtr& expression) const;
        ValueExpressionPtr EvaluateOperator(const ExpressionPtr& expression) const;

        // Operation Value Type Selectors
        ExpressionValueType DeriveOperationValueType(ExpressionOperatorType operationType, ExpressionValueType leftValueType, ExpressionValueType rightValueType) const;
        ExpressionValueType DeriveArithmeticOperationValueType(ExpressionOperatorType operationType, ExpressionValueType leftValueType, ExpressionValueType rightValueType) const;
        ExpressionValueType DeriveIntegerOperationValueType(ExpressionOperatorType operationType, ExpressionValueType leftValueType, ExpressionValueType rightValueType) const;
        ExpressionValueType DeriveComparisonOperationValueType(ExpressionOperatorType operationType, ExpressionValueType leftValueType, ExpressionValueType rightValueType) const;
        ExpressionValueType DeriveBooleanOperationValueType(ExpressionOperatorType operationType, ExpressionValueType leftValueType, ExpressionValueType rightValueType) const;

        // Filter Expression Function Implementations
        ValueExpressionPtr Abs(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr Ceiling(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr Coalesce(const ExpressionCollectionPtr& arguments) const;
        ValueExpressionPtr Convert(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& targetType) const;
        ValueExpressionPtr Contains(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& testValue, const ValueExpressionPtr& ignoreCase) const;
        ValueExpressionPtr DateAdd(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& addValue, const ValueExpressionPtr& intervalType) const;
        ValueExpressionPtr DateDiff(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, const ValueExpressionPtr& intervalType) const;
        ValueExpressionPtr DatePart(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& intervalType) const;
        ValueExpressionPtr EndsWith(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& testValue, const ValueExpressionPtr& ignoreCase) const;
        ValueExpressionPtr Floor(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr IIf(const ValueExpressionPtr& testValue, const ExpressionPtr& leftResultValue, const ExpressionPtr& rightResultValue) const;
        ValueExpressionPtr IndexOf(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& testValue, const ValueExpressionPtr& ignoreCase) const;
        ValueExpressionPtr IsDate(const ValueExpressionPtr& testValue) const;
        ValueExpressionPtr IsInteger(const ValueExpressionPtr& testValue) const;
        ValueExpressionPtr IsGuid(const ValueExpressionPtr& testValue) const;
        ValueExpressionPtr IsNull(const ValueExpressionPtr& testValue, const ValueExpressionPtr& defaultValue) const;
        ValueExpressionPtr IsNumeric(const ValueExpressionPtr& testValue) const;
        ValueExpressionPtr LastIndexOf(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& testValue, const ValueExpressionPtr& ignoreCase) const;
        ValueExpressionPtr Len(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr Lower(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr MaxOf(const ExpressionCollectionPtr& arguments) const;
        ValueExpressionPtr MinOf(const ExpressionCollectionPtr& arguments) const;
        ValueExpressionPtr Now() const;
        ValueExpressionPtr NthIndexOf(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& testValue, const ValueExpressionPtr& indexValue, const ValueExpressionPtr& ignoreCase) const;
        ValueExpressionPtr Power(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& exponentValue) const;
        ValueExpressionPtr RegExMatch(const ValueExpressionPtr& regexValue, const ValueExpressionPtr& testValue) const;
        ValueExpressionPtr RegExVal(const ValueExpressionPtr& regexValue, const ValueExpressionPtr& testValue) const;
        ValueExpressionPtr Replace(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& testValue, const ValueExpressionPtr& replaceValue, const ValueExpressionPtr& ignoreCase) const;
        ValueExpressionPtr Reverse(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr Round(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr Split(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& delimiterValue, const ValueExpressionPtr& indexValue, const ValueExpressionPtr& ignoreCase) const;
        ValueExpressionPtr Sqrt(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr StartsWith(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& testValue, const ValueExpressionPtr& ignoreCase) const;
        ValueExpressionPtr StrCount(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& testValue, const ValueExpressionPtr& ignoreCase) const;
        ValueExpressionPtr StrCmp(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, const ValueExpressionPtr& ignoreCase) const;
        ValueExpressionPtr SubStr(const ValueExpressionPtr& sourceValue, const ValueExpressionPtr& indexValue, const ValueExpressionPtr& lengthValue) const;
        ValueExpressionPtr Trim(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr TrimLeft(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr TrimRight(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr Upper(const ValueExpressionPtr& sourceValue) const;
        ValueExpressionPtr UtcNow() const;

        // Filter Expression Operator Implementations
        ValueExpressionPtr Multiply(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr Divide(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr Modulus(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr Add(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr Subtract(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr BitShiftLeft(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue) const;
        ValueExpressionPtr BitShiftRight(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue) const;
        ValueExpressionPtr BitwiseAnd(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr BitwiseOr(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr BitwiseXor(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr LessThan(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr LessThanOrEqual(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr GreaterThan(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr GreaterThanOrEqual(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType) const;
        ValueExpressionPtr Equal(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType, bool exactMatch = false) const;
        ValueExpressionPtr NotEqual(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, ExpressionValueType valueType, bool exactMatch = false) const;
        ValueExpressionPtr IsNull(const ValueExpressionPtr& leftValue) const;
        ValueExpressionPtr IsNotNull(const ValueExpressionPtr& leftValue) const;
        ValueExpressionPtr Like(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, bool exactMatch = false) const;
        ValueExpressionPtr NotLike(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue, bool exactMatch = false) const;
        ValueExpressionPtr And(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue) const;
        ValueExpressionPtr Or(const ValueExpressionPtr& leftValue, const ValueExpressionPtr& rightValue) const;

        template<class T>
        static T Unary(const T& unaryValue, ExpressionUnaryType unaryOperation);

        template<class T>
        static T UnaryFloat(const T& unaryValue, ExpressionUnaryType unaryOperation, ExpressionValueType unaryValueType);

        static bool UnaryBool(bool unaryValue, ExpressionUnaryType unaryOperation);

        ValueExpressionPtr Convert(const ValueExpressionPtr& sourceValue, ExpressionValueType targetValueType) const;
        ValueExpressionPtr EvaluateRegEx(const std::string& functionName, const ValueExpressionPtr& regexValue, const ValueExpressionPtr& testValue, bool returnMatchedValue) const;
    public:
        ExpressionTree(sttp::data::DataTablePtr table);

        const sttp::data::DataTablePtr& Table() const;
        int32_t TopLimit;
        std::vector<std::tuple<sttp::data::DataColumnPtr, bool /* ascending */, bool /* exactMatch */>> OrderByTerms;

        ExpressionPtr Root = nullptr;

        ValueExpressionPtr Evaluate(const sttp::data::DataRowPtr& row);

        static const ValueExpressionPtr True;
        static const ValueExpressionPtr False;
        static const ValueExpressionPtr EmptyString;
        static ValueExpressionPtr NullValue(ExpressionValueType targetValueType);
    };

    typedef sttp::SharedPtr<ExpressionTree> ExpressionTreePtr;
}}