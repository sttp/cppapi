//******************************************************************************************************
//  FilterExpressionParser.cpp - Gbtc
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
//  11/01/2018 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

// ReSharper disable CppClangTidyClangDiagnosticExitTimeDestructors
// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "FilterExpressionParser.h"
#include "tree/ParseTreeWalker.h"
#include "../Nullable.h"

using namespace std;
using namespace sttp;
using namespace sttp::data;
using namespace sttp::filterexpressions;
using namespace antlr4;
using namespace antlr4::tree;
using namespace boost;

static string ParseStringLiteral(string stringLiteral)
{
    // Remove any surrounding quotes from string, ANTLR grammar already
    // ensures strings starting with quote also ends with one
    if (stringLiteral.front() == '\'')
    {
        stringLiteral.erase(0, 1);
        stringLiteral.erase(stringLiteral.size() - 1);
    }

    return stringLiteral;
}

static sttp::Guid ParseGuidLiteral(string guidLiteral)
{
    // Remove any quotes from GUID (boost currently only handles optional braces),
    // ANTLR grammar already ensures GUID starting with quote also ends with one
    if (guidLiteral.front() == '\'')
    {
        guidLiteral.erase(0, 1);
        guidLiteral.erase(guidLiteral.size() - 1);
    }

    return ParseGuid(guidLiteral.c_str());
}

static datetime_t ParseDateTimeLiteral(string time)
{
    // Remove any surrounding '#' symbols from date/time, ANTLR grammar already
    // ensures date/time starting with '#' symbol will also end with one
    if (time.front() == '#')
    {
        time.erase(0, 1);
        time.erase(time.size() - 1);
    }

    return ParseTimestamp(time.c_str());
}

static string ParsePointTagLiteral(string pointTagLiteral)
{
    // Remove any double-quotes from point tag literal, ANTLR grammar already
    // ensures tag starting with quote also ends with one
    if (pointTagLiteral.front() == '"')
    {
        pointTagLiteral.erase(0, 1);
        pointTagLiteral.erase(pointTagLiteral.size() - 1);
    }

    return pointTagLiteral;
}

FilterExpressionParserException::FilterExpressionParserException(string message) noexcept :
    m_message(std::move(message))
{
}

const char* FilterExpressionParserException::what() const noexcept
{
    return m_message.c_str();
}

FilterExpressionParser::FilterExpressionParser(const string& filterExpression, const bool suppressConsoleErrorOutput) :
    m_inputStream(filterExpression),
    m_callbackErrorListener(nullptr),
    m_dataSet(nullptr),
    m_trackFilteredRows(true),
    m_trackFilteredSignalIDs(false),
    m_filterExpressionStatementCount(0)
{
    m_lexer = new FilterExpressionSyntaxLexer(&m_inputStream);
    m_tokens = new CommonTokenStream(m_lexer);
    m_parser = new FilterExpressionSyntaxParser(m_tokens);
    
    if (suppressConsoleErrorOutput)
        m_parser->removeErrorListeners();
}

FilterExpressionParser::~FilterExpressionParser()
{
    delete m_lexer;
    delete m_tokens;
    delete m_parser;
    delete m_callbackErrorListener;
}

FilterExpressionParser::CallbackErrorListener::CallbackErrorListener(FilterExpressionParserPtr filterExpressionParser, ParsingExceptionCallback parsingExceptionCallback) :
    m_filterExpressionParser(std::move(filterExpressionParser)),
    m_parsingExceptionCallback(std::move(parsingExceptionCallback))
{        
}

void FilterExpressionParser::CallbackErrorListener::syntaxError(Recognizer* recognizer, Token* offendingSymbol, size_t line, size_t charPositionInLine, const string &msg, std::exception_ptr e)
{
    if (m_parsingExceptionCallback != nullptr)
        m_parsingExceptionCallback(m_filterExpressionParser, msg);
}

void FilterExpressionParser::VisitParseTreeNodes()
{
    // Create parse tree and visit listener methods
    const ParseTreeWalker walker;
    ParseTree* parseTree = m_parser->parse();
    walker.walk(this, parseTree);
}

void FilterExpressionParser::InitializeSetOperations()
{
    // As an optimization, set operations are not engaged until second filter expression statement
    // is encountered, only then will duplicate results be a concern. Note that only using an
    // unordered_set is not an option because results can be sorted with the "ORDER BY" clause.
    if (m_trackFilteredRows && m_filteredRowSet.empty() && !m_filteredRows.empty())
    {
        const size_t count = m_filteredRows.size();
        m_filteredRowSet.reserve(count);

        for (size_t i = 0; i < count; i++)
            m_filteredRowSet.insert(m_filteredRows[i]);
    }

    if (m_trackFilteredSignalIDs && m_filteredSignalIDSet.empty() && !m_filteredSignalIDs.empty())
    {
        const size_t count = m_filteredSignalIDs.size();
        m_filteredSignalIDSet.reserve(count);

        for (size_t i = 0; i < count; i++)
            m_filteredSignalIDSet.insert(m_filteredSignalIDs[i]);
    }
}

void FilterExpressionParser::AddMatchedRow(const sttp::data::DataRowPtr& row, const int32_t signalIDColumnIndex)
{
    if (m_filterExpressionStatementCount > 1)
    {
        // Set operations
        if (m_trackFilteredRows && m_filteredRowSet.insert(row).second)
            m_filteredRows.push_back(row);

        if (m_trackFilteredSignalIDs)
        {
            const Nullable<sttp::Guid> signalIDField = row->ValueAsGuid(signalIDColumnIndex);

            if (signalIDField.HasValue())
            {
                const sttp::Guid signalID = signalIDField.GetValueOrDefault();

                if (signalID != Empty::Guid && m_filteredSignalIDSet.insert(signalID).second)
                    m_filteredSignalIDs.push_back(signalID);
            }
        }
    }
    else
    {
        // Vector only operations
        if (m_trackFilteredRows)
            m_filteredRows.push_back(row);

        if (m_trackFilteredSignalIDs)
        {
            const Nullable<sttp::Guid> signalIDField = row->ValueAsGuid(signalIDColumnIndex);

            if (signalIDField.HasValue())
            {
                const sttp::Guid signalID = signalIDField.GetValueOrDefault();

                if (signalID != Empty::Guid)
                    m_filteredSignalIDs.push_back(signalID);
            }
        }
    }
}

void FilterExpressionParser::MapMatchedFieldRow(const DataTablePtr& primaryTable, const string& columnName, const string& matchValue, const int32_t signalIDColumnIndex)
{
    const DataColumnPtr& column = primaryTable->Column(columnName);

    if (column == nullptr)
        return;

    const int32_t columnIndex = column->Index();

    for (int32_t i = 0; i < primaryTable->RowCount(); i++)
    {
        const DataRowPtr& row = primaryTable->Row(i);

        if (row)
        {
            const Nullable<string> field = row->ValueAsString(columnIndex);

            if (field.HasValue() && IsEqual(matchValue, field.GetValueOrDefault()))
            {
                AddMatchedRow(row, signalIDColumnIndex);
                return;
            }
        }
    }
}

bool FilterExpressionParser::TryGetExpr(const ParserRuleContext* context, ExpressionPtr& expression) const
{
    return TryGetValue<const ParserRuleContext*, ExpressionPtr>(m_expressions, context, expression, nullptr);
}

void FilterExpressionParser::AddExpr(const ParserRuleContext* context, const ExpressionPtr& expression)
{
    // Track expression in parser rule context map
    m_expressions.insert(pair<const ParserRuleContext*, ExpressionPtr>(context, expression));

    // Update active expression tree root
    m_activeExpressionTree->Root = expression;
}

const DataSetPtr& FilterExpressionParser::GetDataSet() const
{
    return m_dataSet;
}

void FilterExpressionParser::SetDataSet(const DataSetPtr& dataSet)
{
    m_dataSet = dataSet;
}

TableIDFieldsPtr FilterExpressionParser::GetTableIDFields(const string& tableName) const
{
    TableIDFieldsPtr tableIDFields;
    
    TryGetValue<TableIDFieldsPtr>(m_tableIDFields, tableName, tableIDFields, nullptr);

    return tableIDFields;
}

void FilterExpressionParser::SetTableIDFields(const string& tableName, const TableIDFieldsPtr& tableIDFields)
{
    m_tableIDFields[tableName] = tableIDFields;
}

const string& FilterExpressionParser::GetPrimaryTableName() const
{
    return m_primaryTableName;
}

void FilterExpressionParser::SetPrimaryTableName(const string& tableName)
{
    m_primaryTableName = tableName;
}

void FilterExpressionParser::RegisterParsingExceptionCallback(const ParsingExceptionCallback& parsingExceptionCallback)
{
    if (m_callbackErrorListener)
    {
        m_parser->removeErrorListener(m_callbackErrorListener);
        delete m_callbackErrorListener;
    }

    if (parsingExceptionCallback == nullptr)
        return;

    m_callbackErrorListener = new CallbackErrorListener(shared_from_this(), parsingExceptionCallback);
    m_parser->addErrorListener(m_callbackErrorListener);
}

void FilterExpressionParser::Evaluate()
{
    if (m_dataSet == nullptr)
        throw FilterExpressionParserException("Cannot evaluate filter expression, no dataset has been defined");

    if (!m_trackFilteredRows && !m_trackFilteredSignalIDs)
        throw FilterExpressionParserException("No use in evaluating filter expression, neither filtered rows nor signal IDs have been set for tracking");

    m_filterExpressionStatementCount = 0;
    m_filteredRows.clear();
    m_filteredRowSet.clear();
    m_filteredSignalIDs.clear();
    m_filteredSignalIDSet.clear();
    m_expressionTrees.clear();
    m_expressions.clear();

    VisitParseTreeNodes();

    // Each filter expression statement will have its own expression tree, evaluate each
    for (auto& expressionTree : m_expressionTrees)
    {
        const vector<DataRowPtr> matchedRows = Select(expressionTree);
        int32_t signalIDColumnIndex = -1;

        if (m_trackFilteredSignalIDs)
        {
            const DataTablePtr& table = expressionTree->Table();
            const TableIDFieldsPtr& primaryTableIDFields = GetTableIDFields(table->Name());

            if (primaryTableIDFields == nullptr)
                throw FilterExpressionParserException("Failed to find ID fields record for table \"" + table->Name() + "\"");

            const DataColumnPtr& signalIDColumn = table->Column(primaryTableIDFields->SignalIDFieldName);

            if (signalIDColumn == nullptr)
                throw FilterExpressionParserException("Failed to find signal ID field \"" + primaryTableIDFields->SignalIDFieldName + "\" for table \"" + table->Name() + "\"");

            signalIDColumnIndex = signalIDColumn->Index();
        }

        for (const auto& matchedRow : matchedRows)
            AddMatchedRow(matchedRow, signalIDColumnIndex);
    }
}

bool FilterExpressionParser::GetTrackFilteredRows() const
{
    return m_trackFilteredRows;
}

void FilterExpressionParser::SetTrackFilteredRows(const bool trackFilteredRows)
{
    m_trackFilteredRows = trackFilteredRows;
}

const vector<DataRowPtr>& FilterExpressionParser::FilteredRows() const
{
    return m_filteredRows;
}

const unordered_set<DataRowPtr>& FilterExpressionParser::FilteredRowSet()
{
    InitializeSetOperations();
    return m_filteredRowSet;
}

bool FilterExpressionParser::GetTrackFilteredSignalIDs() const
{
    return m_trackFilteredSignalIDs;
}

void FilterExpressionParser::SetTrackFilteredSignalIDs(const bool trackFilteredSignalIDs)
{
    m_trackFilteredSignalIDs = trackFilteredSignalIDs;
}

const vector<sttp::Guid>& FilterExpressionParser::FilteredSignalIDs() const
{
    return m_filteredSignalIDs;
}

const unordered_set<sttp::Guid>& FilterExpressionParser::FilteredSignalIDSet()
{
    InitializeSetOperations();
    return m_filteredSignalIDSet;
}

const vector<ExpressionTreePtr>& FilterExpressionParser::GetExpressionTrees()
{
    if (m_expressionTrees.empty())
        VisitParseTreeNodes();

    return m_expressionTrees;
}

/*
    filterExpressionStatement
     : identifierStatement
     | filterStatement
     | expression
     ;
 */
void FilterExpressionParser::enterFilterExpressionStatement(FilterExpressionSyntaxParser::FilterExpressionStatementContext*)
{
    // One filter expression can contain multiple filter statements separated by semi-colon,
    // so we track each as an independent expression tree
    m_expressions.clear();
    m_activeExpressionTree = nullptr;
    m_filterExpressionStatementCount++;

    // Encountering second filter expression statement necessitates the use of set operations
    // to prevent possible result duplications
    if (m_filterExpressionStatementCount == 2)
        InitializeSetOperations();
}

/*
    filterStatement
     : K_FILTER ( K_TOP topLimit )? tableName K_WHERE expression ( K_ORDER K_BY orderingTerm ( ',' orderingTerm )* )?
     ;

    topLimit
     : ( '-' | '+' )? INTEGER_LITERAL
     ;

    orderingTerm
     : exactMatchModifier? columnName ( K_ASC | K_DESC )?
     ;
 */
void FilterExpressionParser::enterFilterStatement(FilterExpressionSyntaxParser::FilterStatementContext* context)
{
    const string& tableName = context->tableName()->getText();
    const DataTablePtr& table = m_dataSet->Table(tableName);

    if (table == nullptr)
        throw FilterExpressionParserException("Failed to find table \"" + tableName + "\"");

    m_activeExpressionTree = NewSharedPtr<ExpressionTree>(table);
    m_expressionTrees.push_back(m_activeExpressionTree);

    if (context->K_TOP() != nullptr)
        TryParseInt32(context->topLimit()->getText(), m_activeExpressionTree->TopLimit, -1);

    if (context->K_ORDER() != nullptr && context->K_BY() != nullptr)
    {
        for (size_t i = 0; i < context->orderingTerm().size(); i++)
        {
            auto* orderingTermContext = context->orderingTerm(i);
            const string& orderByColumnName = orderingTermContext->orderByColumnName()->getText();
            const DataColumnPtr& orderByColumn = table->Column(orderByColumnName);

            if (orderByColumn == nullptr)
                throw FilterExpressionParserException("Failed to find order by field \"" + orderByColumnName + "\" for table \"" + table->Name() + "\"");

            m_activeExpressionTree->OrderByTerms.emplace_back(
                orderByColumn,
                orderingTermContext->K_DESC() == nullptr,
                orderingTermContext->exactMatchModifier() == nullptr
            );
        }
    }
}

/*
    identifierStatement
     : GUID_LITERAL
     | MEASUREMENT_KEY_LITERAL
     | POINT_TAG_LITERAL
     ;
 */
void FilterExpressionParser::exitIdentifierStatement(FilterExpressionSyntaxParser::IdentifierStatementContext* context)
{
    sttp::Guid signalID = Empty::Guid;

    if (context->GUID_LITERAL())
    {
        signalID = ParseGuidLiteral(context->GUID_LITERAL()->getText());

        if (!m_trackFilteredRows && !m_trackFilteredSignalIDs)
        {
            // Handle edge case of encountering standalone Guid when not tracking rows or table identifiers.
            // In this scenario the filter expression parser would only be used to generate expression trees
            // for general expression parsing, e.g., for a DataColumn expression, so here the Guid should be
            // treated as a literal expression value instead of an identifier to track:
            enterExpression(nullptr);
            m_activeExpressionTree->Root = CastSharedPtr<Expression>(NewSharedPtr<ValueExpression>(ExpressionValueType::Guid, signalID));
            return;
        }
        
        if (m_trackFilteredSignalIDs && signalID != Empty::Guid)
        {
            if (m_filterExpressionStatementCount > 1)
            {
                if (m_filteredSignalIDSet.insert(signalID).second)
                    m_filteredSignalIDs.push_back(signalID);
            }
            else
            {
                m_filteredSignalIDs.push_back(signalID);
            }
        }

        if (!m_trackFilteredRows)
            return;
    }

    const DataTablePtr& primaryTable = m_dataSet->Table(m_primaryTableName);

    if (primaryTable == nullptr)
        return;

    const TableIDFieldsPtr& primaryTableIDFields = GetTableIDFields(m_primaryTableName);

    if (primaryTableIDFields == nullptr)
        return;

    const DataColumnPtr& signalIDColumn = primaryTable->Column(primaryTableIDFields->SignalIDFieldName);

    if (signalIDColumn == nullptr)
        return;

    const int32_t signalIDColumnIndex = signalIDColumn->Index();

    if (m_trackFilteredRows && signalID != Empty::Guid)
    {
        // Map matching row for manually specified Guid
        for (int32_t i = 0; i < primaryTable->RowCount(); i++)
        {
            const DataRowPtr& row = primaryTable->Row(i);

            if (row)
            {
                const Nullable<sttp::Guid> signalIDField = row->ValueAsGuid(signalIDColumnIndex);

                if (signalIDField.HasValue() && signalIDField.GetValueOrDefault() == signalID)
                {
                    if (m_filterExpressionStatementCount > 1)
                    {
                        if (m_filteredRowSet.insert(row).second)
                            m_filteredRows.push_back(row);
                    }
                    else
                    {
                        m_filteredRows.push_back(row);
                    }

                    return;
                }
            }
        }

        return;
    }

    if (context->MEASUREMENT_KEY_LITERAL())
    {
        MapMatchedFieldRow(primaryTable, primaryTableIDFields->MeasurementKeyFieldName, context->MEASUREMENT_KEY_LITERAL()->getText(), signalIDColumnIndex);
        return;
    }

    if (context->POINT_TAG_LITERAL())
        MapMatchedFieldRow(primaryTable, primaryTableIDFields->PointTagFieldName, ParsePointTagLiteral(context->POINT_TAG_LITERAL()->getText()), signalIDColumnIndex);
}

/*
    expression
     : notOperator expression
     | expression logicalOperator expression
     | predicateExpression
     ;
 */
void FilterExpressionParser::enterExpression(FilterExpressionSyntaxParser::ExpressionContext*)
{
    // Handle case of encountering a standalone expression, i.e., an expression not within a filter statement context
    if (m_activeExpressionTree == nullptr)
    {
        const DataTablePtr& table = m_dataSet->Table(m_primaryTableName);

        if (table == nullptr)
            throw FilterExpressionParserException("Failed to find table \"" + m_primaryTableName + "\"");

        m_activeExpressionTree = NewSharedPtr<ExpressionTree>(table);
        m_expressionTrees.push_back(m_activeExpressionTree);
    }
}

/*
    expression
     : notOperator expression
     | expression logicalOperator expression
     | predicateExpression
     ;
 */
void FilterExpressionParser::exitExpression(FilterExpressionSyntaxParser::ExpressionContext* context)
{
    ExpressionPtr value;

    // Check for predicate expressions (see explicit visit function)
    auto* predicateExpressionContext = context->predicateExpression();

    if (predicateExpressionContext != nullptr)
    {
        if (TryGetExpr(predicateExpressionContext, value))
        {
            AddExpr(context, value);
            return;
        }

        throw FilterExpressionParserException("Failed to find predicate expression \"" + predicateExpressionContext->getText() + "\"");
    }

    // Check for not operator expressions
    const auto* notOperatorContext = context->notOperator();

    if (notOperatorContext != nullptr)
    {
        if (context->expression().size() != 1)
            throw FilterExpressionParserException("Not operator expression is malformed: \"" + context->getText() + "\"");

        if (!TryGetExpr(context->expression(0), value))
            throw FilterExpressionParserException("Failed to find not operator expression \"" + context->getText() + "\"");
        
        AddExpr(context, NewSharedPtr<UnaryExpression>(ExpressionUnaryType::Not, value));
        return;
    }

    // Check for logical operator expressions
    auto* logicalOperatorContext = context->logicalOperator();

    if (logicalOperatorContext != nullptr)
    {
        ExpressionPtr leftValue, rightValue;
        ExpressionOperatorType operatorType;

        if (context->expression().size() != 2)
            throw FilterExpressionParserException("Operator expression, in logical operator expression context, is malformed: \"" + context->getText() + "\"");

        if (!TryGetExpr(context->expression(0), leftValue))
            throw FilterExpressionParserException("Failed to find left operator expression \"" + context->expression(0)->getText() + "\"");

        if (!TryGetExpr(context->expression(1), rightValue))
            throw FilterExpressionParserException("Failed to find right operator expression \"" + context->expression(1)->getText() + "\"");

        const string& operatorSymbol = logicalOperatorContext->getText();

        // Check for boolean operations
        if (logicalOperatorContext->K_AND() != nullptr || IsEqual(operatorSymbol, "&&", false))
            operatorType = ExpressionOperatorType::And;
        else if (logicalOperatorContext->K_OR() != nullptr  || IsEqual(operatorSymbol, "||", false))
            operatorType = ExpressionOperatorType::Or;
        else
            throw FilterExpressionParserException("Unexpected logical operator \"" + operatorSymbol + "\"");

        AddExpr(context, NewSharedPtr<OperatorExpression>(operatorType, leftValue, rightValue));
        return;
    }

    throw FilterExpressionParserException("Unexpected expression \"" + context->getText() + "\"");
}

/*
    predicateExpression
     : predicateExpression notOperator? K_IN exactMatchModifier? '(' expressionList ')'
     | predicateExpression K_IS notOperator? K_NULL
     | predicateExpression comparisonOperator predicateExpression
     | predicateExpression notOperator? K_LIKE exactMatchModifier? predicateExpression
     | valueExpression
     ;
 */
void FilterExpressionParser::exitPredicateExpression(FilterExpressionSyntaxParser::PredicateExpressionContext* context)
{
    ExpressionPtr value;

    // Check for value expressions (see explicit visit function)
    auto* valueExpressionContext = context->valueExpression();

    if (valueExpressionContext != nullptr)
    {
        if (TryGetExpr(valueExpressionContext, value))
        {
            AddExpr(context, value);
            return;
        }

        throw FilterExpressionParserException("Failed to find value expression \"" + valueExpressionContext->getText() + "\"");
    }

    // Check for IN expressions
    const auto* inKeywordContext = context->K_IN();
    const auto* notOperatorContext = context->notOperator();
    const auto* exactMatchModifierContext = context->exactMatchModifier();

    if (inKeywordContext != nullptr)
    {
        // IN expression expects one predicate
        if (context->predicateExpression().size() != 1)
            throw FilterExpressionParserException("\"IN\" expression is malformed: \"" + context->getText() + "\"");

        if (!TryGetExpr(context->predicateExpression(0), value))
            throw FilterExpressionParserException("Failed to find \"IN\" predicate expression \"" + context->predicateExpression(0)->getText() + "\"");

        const ExpressionCollectionPtr arguments = NewSharedPtr<ExpressionCollection>();
        auto* expressionList = context->expressionList();
        const size_t argumentCount = expressionList->expression().size();

        if (argumentCount < 1)
            throw FilterExpressionParserException("Not enough expressions found for \"IN\" operation");

        for (size_t i = 0; i < argumentCount; i++)
        {
            ExpressionPtr argument;

            if (TryGetExpr(expressionList->expression(i), argument))
                arguments->push_back(argument);
            else
                throw FilterExpressionParserException("Failed to find argument expression " + ToString(i) + " \"" + expressionList->expression(i)->getText() + "\" for \"IN\" operation");
        }

        AddExpr(context, NewSharedPtr<InListExpression>(value, arguments, notOperatorContext != nullptr, exactMatchModifierContext != nullptr));
        return;
    }

    // Check for IS NULL expressions
    const auto* isKeywordContext = context->K_IS();
    const auto* nullKeywordContext = context->K_NULL();

    if (isKeywordContext != nullptr && nullKeywordContext != nullptr)
    {
        const ExpressionOperatorType operatorType = notOperatorContext == nullptr ? ExpressionOperatorType::IsNull : ExpressionOperatorType::IsNotNull;

        // IS NULL expression expects one predicate
        if (context->predicateExpression().size() != 1)
            throw FilterExpressionParserException("\"IS NULL\" expression is malformed: \"" + context->getText() + "\"");

        if (TryGetExpr(context->predicateExpression(0), value))
        {
            AddExpr(context, NewSharedPtr<OperatorExpression>(operatorType, value, nullptr));
            return;
        }

        throw FilterExpressionParserException("Failed to find \"IS NULL\" predicate expression \"" + context->predicateExpression(0)->getText() + "\"");
    }

    // Remaining operators require two predicate expressions
    if (context->predicateExpression().size() != 2)
        throw FilterExpressionParserException("Operator expression, in predicate expression context, is malformed: \"" + context->getText() + "\"");

    ExpressionPtr leftValue, rightValue;
    ExpressionOperatorType operatorType;

    if (!TryGetExpr(context->predicateExpression(0), leftValue))
        throw FilterExpressionParserException("Failed to find left operator predicate expression \"" + context->predicateExpression(0)->getText() + "\"");

    if (!TryGetExpr(context->predicateExpression(1), rightValue))
        throw FilterExpressionParserException("Failed to find right operator predicate expression \"" + context->predicateExpression(1)->getText() + "\"");

    // Check for comparison operator expressions
    auto* comparisonOperatorContext = context->comparisonOperator();

    if (comparisonOperatorContext != nullptr)
    {
        const string& operatorSymbol = comparisonOperatorContext->getText();

        // Check for comparison operations
        if (IsEqual(operatorSymbol, "<", false))
            operatorType = ExpressionOperatorType::LessThan;
        else if (IsEqual(operatorSymbol, "<=", false))
            operatorType = ExpressionOperatorType::LessThanOrEqual;
        else if (IsEqual(operatorSymbol, ">", false))
            operatorType = ExpressionOperatorType::GreaterThan;
        else if (IsEqual(operatorSymbol, ">=", false))
            operatorType = ExpressionOperatorType::GreaterThanOrEqual;
        else if (IsEqual(operatorSymbol, "=", false) || IsEqual(operatorSymbol, "==", false))
            operatorType = ExpressionOperatorType::Equal;
        else if (IsEqual(operatorSymbol, "===", false))
            operatorType = ExpressionOperatorType::EqualExactMatch;
        else if (IsEqual(operatorSymbol, "<>", false) || IsEqual(operatorSymbol, "!=", false))
            operatorType = ExpressionOperatorType::NotEqual;
        else if (IsEqual(operatorSymbol, "!==", false))
            operatorType = ExpressionOperatorType::NotEqualExactMatch;
        else
            throw FilterExpressionParserException("Unexpected comparison operator \"" + operatorSymbol + "\"");

        AddExpr(context, NewSharedPtr<OperatorExpression>(operatorType, leftValue, rightValue));
        return;
    }

    // Check for LIKE expressions
    const auto* likeKeywordContext = context->K_LIKE();

    if (likeKeywordContext != nullptr)
    {
        if (exactMatchModifierContext == nullptr)
            operatorType = notOperatorContext == nullptr ? ExpressionOperatorType::Like : ExpressionOperatorType::NotLike;
        else
            operatorType = notOperatorContext == nullptr ? ExpressionOperatorType::LikeExactMatch : ExpressionOperatorType::NotLikeExactMatch;

        AddExpr(context, NewSharedPtr<OperatorExpression>(operatorType, leftValue, rightValue));
        return;
    }

    throw FilterExpressionParserException("Unexpected predicate expression \"" + context->getText() + "\"");
}

/*
    valueExpression
     : literalValue
     | columnName
     | functionExpression
     | unaryOperator valueExpression
     | '(' expression ')'
     | valueExpression mathOperator valueExpression
     | valueExpression bitwiseOperator valueExpression
; */
void FilterExpressionParser::exitValueExpression(FilterExpressionSyntaxParser::ValueExpressionContext* context)
{
    ExpressionPtr value;

    // Check for literal values (see explicit visit function)
    auto* literalValueContext = context->literalValue();

    if (literalValueContext != nullptr)
    {
        if (TryGetExpr(literalValueContext, value))
        {
            AddExpr(context, value);
            return;
        }

        throw FilterExpressionParserException("Failed to find literal value \"" + literalValueContext->getText() + "\"");
    }

    // Check for column names (see explicit visit function)
    auto* columnNameContext = context->columnName();

    if (columnNameContext != nullptr)
    {
        if (TryGetExpr(columnNameContext, value))
        {
            AddExpr(context, value);
            return;
        }

        throw FilterExpressionParserException("Failed to find column name \"" + columnNameContext->getText() + "\"");
    }

    // Check for function expressions (see explicit visit function)
    auto* functionExpressionContext = context->functionExpression();

    if (functionExpressionContext != nullptr)
    {
        if (TryGetExpr(functionExpressionContext, value))
        {
            AddExpr(context, value);
            return;
        }

        throw FilterExpressionParserException("Failed to find function expression \"" + functionExpressionContext->getText() + "\"");
    }

    // Check for unary operators
    auto* unaryOperatorContext = context->unaryOperator();

    if (unaryOperatorContext != nullptr)
    {
        if (context->valueExpression().size() != 1)
            throw FilterExpressionParserException("Unary operator value expression is undefined");

        if (TryGetExpr(context->valueExpression(0), value))
        {
            ExpressionUnaryType unaryType;
            const string unaryOperator = unaryOperatorContext->getText();

            if (IsEqual(unaryOperator, "+", false))
                unaryType = ExpressionUnaryType::Plus;
            else if (IsEqual(unaryOperator, "-", false))
                unaryType = ExpressionUnaryType::Minus;
            else if (IsEqual(unaryOperator, "~", false) || IsEqual(unaryOperator, "!", false) || unaryOperatorContext->K_NOT() != nullptr)
                unaryType = ExpressionUnaryType::Not;
            else
                throw FilterExpressionParserException("Unexpected unary operator type \"" + unaryOperator + "\"");

            AddExpr(context, NewSharedPtr<UnaryExpression>(unaryType, value));
            return;
        }

        throw FilterExpressionParserException("Failed to find unary operator value expression \"" + context->getText() + "\"");
    }

    // Check for sub-expressions, i.e., "(" expression ")"
    auto* expressionContext = context->expression();

    if (expressionContext != nullptr)
    {
        if (TryGetExpr(expressionContext, value))
        {
            AddExpr(context, value);
            return;
        }

        throw FilterExpressionParserException("Failed to find sub-expression \"" + expressionContext->getText() + "\"");
    }

    // Remaining operators require two value expressions
    if (context->valueExpression().size() != 2)
        throw FilterExpressionParserException("Operator expression, in value expression context, is malformed: \"" + context->getText() + "\"");

    ExpressionPtr leftValue, rightValue;
    ExpressionOperatorType operatorType;

    if (!TryGetExpr(context->valueExpression(0), leftValue))
        throw FilterExpressionParserException("Failed to find left operator value expression \"" + context->valueExpression(0)->getText() + "\"");

    if (!TryGetExpr(context->valueExpression(1), rightValue))
        throw FilterExpressionParserException("Failed to find right operator value expression \"" + context->valueExpression(1)->getText() + "\"");

    // Check for math operator expressions
    auto* mathOperatorContext = context->mathOperator();

    if (mathOperatorContext != nullptr)
    {
        const string& operatorSymbol = mathOperatorContext->getText();

        // Check for arithmetic operations
        if (IsEqual(operatorSymbol, "*", false))
            operatorType = ExpressionOperatorType::Multiply;
        else if (IsEqual(operatorSymbol, "/", false))
            operatorType = ExpressionOperatorType::Divide;
        else if (IsEqual(operatorSymbol, "%", false))
            operatorType = ExpressionOperatorType::Modulus;
        else if (IsEqual(operatorSymbol, "+", false))
            operatorType = ExpressionOperatorType::Add;
        else if (IsEqual(operatorSymbol, "-", false))
            operatorType = ExpressionOperatorType::Subtract;
        else
            throw FilterExpressionParserException("Unexpected math operator \"" + operatorSymbol + "\"");

        AddExpr(context, NewSharedPtr<OperatorExpression>(operatorType, leftValue, rightValue));
        return;
    }

    // Check for bitwise operator expressions
    auto* bitwiseOperatorContext = context->bitwiseOperator();

    if (bitwiseOperatorContext != nullptr)
    {
        const string& operatorSymbol = bitwiseOperatorContext->getText();

        // Check for bitwise operations
        if (IsEqual(operatorSymbol, "<<", false))
            operatorType = ExpressionOperatorType::BitShiftLeft;
        else if (IsEqual(operatorSymbol, ">>", false))
            operatorType = ExpressionOperatorType::BitShiftRight;
        else if (IsEqual(operatorSymbol, "&", false))
            operatorType = ExpressionOperatorType::BitwiseAnd;
        else if (IsEqual(operatorSymbol, "|", false))
            operatorType = ExpressionOperatorType::BitwiseOr;
        else if (IsEqual(operatorSymbol, "^", false) || bitwiseOperatorContext->K_XOR() != nullptr)
            operatorType = ExpressionOperatorType::BitwiseXor;
        else
            throw FilterExpressionParserException("Unexpected bitwise operator \"" + operatorSymbol + "\"");

        AddExpr(context, NewSharedPtr<OperatorExpression>(operatorType, leftValue, rightValue));
        return;
    }

    throw FilterExpressionParserException("Unexpected value expression \"" + context->getText() + "\"");
}

/*
    literalValue
     : INTEGER_LITERAL
     | NUMERIC_LITERAL
     | STRING_LITERAL
     | DATETIME_LITERAL
     | GUID_LITERAL
     | BOOLEAN_LITERAL
     | K_NULL
     ;
 */
void FilterExpressionParser::exitLiteralValue(FilterExpressionSyntaxParser::LiteralValueContext* context)
{
    ValueExpressionPtr result = nullptr;

    if (context->INTEGER_LITERAL())
    {
        const string& literal = context->INTEGER_LITERAL()->getText();
        int64_t i64Val;
        uint64_t ui64Val;

        if (TryParseInt64(literal, i64Val))
        {
            if (i64Val < Int32::MinValue || i64Val > Int32::MaxValue)
                result = NewSharedPtr<ValueExpression>(ExpressionValueType::Int64, i64Val);
            else
                result = NewSharedPtr<ValueExpression>(ExpressionValueType::Int32, static_cast<int32_t>(i64Val));
        }
        else if (TryParseUInt64(literal, ui64Val))
        {
            if (ui64Val > Int64::MaxValue)
                result = ParseNumericLiteral(literal);
            else if (ui64Val > Int32::MaxValue)
                result = NewSharedPtr<ValueExpression>(ExpressionValueType::Int64, static_cast<int64_t>(ui64Val));
            else
                result = NewSharedPtr<ValueExpression>(ExpressionValueType::Int32, static_cast<int32_t>(ui64Val));
        }
        else
        {
            result = NewSharedPtr<ValueExpression>(ExpressionValueType::Decimal, decimal_t(literal));
        }
    }
    else if (context->NUMERIC_LITERAL())
    {
        const string& literal = context->NUMERIC_LITERAL()->getText();
        float64_t dblVal;
        
        // Real literals using scientific notation are parsed as double
        if (Contains(literal, "E") && TryParseDouble(literal, dblVal))
            result = NewSharedPtr<ValueExpression>(ExpressionValueType::Double, dblVal);
        else
            result = ParseNumericLiteral(literal);
    }
    else if (context->STRING_LITERAL())
    {
        result = NewSharedPtr<ValueExpression>(ExpressionValueType::String, ParseStringLiteral(context->STRING_LITERAL()->getText()));
    }
    else if (context->DATETIME_LITERAL())
    {
        result = NewSharedPtr<ValueExpression>(ExpressionValueType::DateTime, ParseDateTimeLiteral(context->DATETIME_LITERAL()->getText()));
    }
    else if (context->GUID_LITERAL())
    {
        result = NewSharedPtr<ValueExpression>(ExpressionValueType::Guid, ParseGuidLiteral(context->GUID_LITERAL()->getText()));
    }
    else if (context->BOOLEAN_LITERAL())
    {
        result = IsEqual(context->BOOLEAN_LITERAL()->getText(), "true") ? ExpressionTree::True : ExpressionTree::False;
    }
    else if (context->K_NULL())
    {
        result = ExpressionTree::NullValue(ExpressionValueType::Undefined);
    }

    if (result)
        AddExpr(context, CastSharedPtr<Expression>(result));
}

ValueExpressionPtr FilterExpressionParser::ParseNumericLiteral(const string& literal)
{
    decimal_t decVal;

    if (TryParseDecimal(literal, decVal))
        return NewSharedPtr<ValueExpression>(ExpressionValueType::Decimal, decVal);

    float64_t dblVal;

    if (TryParseDouble(literal, dblVal))
        return NewSharedPtr<ValueExpression>(ExpressionValueType::Double, dblVal);

    return NewSharedPtr<ValueExpression>(ExpressionValueType::String, literal);
}

/*
    columnName
     : IDENTIFIER
     ;
 */
void FilterExpressionParser::exitColumnName(FilterExpressionSyntaxParser::ColumnNameContext* context)
{
    const string& columnName = context->IDENTIFIER()->getText();
    const DataColumnPtr& dataColumn = m_activeExpressionTree->Table()->Column(columnName);

    if (dataColumn == nullptr)
        throw FilterExpressionParserException("Failed to find column \"" + columnName + "\" in table \"" + m_activeExpressionTree->Table()->Name() + "\"");

    AddExpr(context, NewSharedPtr<ColumnExpression>(dataColumn));
}

/*
    functionExpression
     : functionName '(' expressionList? ')'
     ;
 */
void FilterExpressionParser::exitFunctionExpression(FilterExpressionSyntaxParser::FunctionExpressionContext* context)
{
    ExpressionFunctionType functionType;
    auto* functionNameContext = context->functionName();

    if (functionNameContext->K_ABS() != nullptr)
        functionType = ExpressionFunctionType::Abs;
    else if (functionNameContext->K_CEILING() != nullptr)
        functionType = ExpressionFunctionType::Ceiling;
    else if (functionNameContext->K_COALESCE() != nullptr)
        functionType = ExpressionFunctionType::Coalesce;
    else if (functionNameContext->K_CONVERT() != nullptr)
        functionType = ExpressionFunctionType::Convert;
    else if (functionNameContext->K_CONTAINS() != nullptr)
        functionType = ExpressionFunctionType::Contains;
    else if (functionNameContext->K_DATEADD() != nullptr)
        functionType = ExpressionFunctionType::DateAdd;
    else if (functionNameContext->K_DATEDIFF() != nullptr)
        functionType = ExpressionFunctionType::DateDiff;
    else if (functionNameContext->K_DATEPART() != nullptr)
        functionType = ExpressionFunctionType::DatePart;
    else if (functionNameContext->K_ENDSWITH() != nullptr)
        functionType = ExpressionFunctionType::EndsWith;
    else if (functionNameContext->K_FLOOR() != nullptr)
        functionType = ExpressionFunctionType::Floor;
    else if (functionNameContext->K_IIF() != nullptr)
        functionType = ExpressionFunctionType::IIf;
    else if (functionNameContext->K_INDEXOF() != nullptr)
        functionType = ExpressionFunctionType::IndexOf;
    else if (functionNameContext->K_ISDATE() != nullptr)
        functionType = ExpressionFunctionType::IsDate;
    else if (functionNameContext->K_ISINTEGER() != nullptr)
        functionType = ExpressionFunctionType::IsInteger;
    else if (functionNameContext->K_ISGUID() != nullptr)
        functionType = ExpressionFunctionType::IsGuid;
    else if (functionNameContext->K_ISNULL() != nullptr)
        functionType = ExpressionFunctionType::IsNull;
    else if (functionNameContext->K_ISNUMERIC() != nullptr)
        functionType = ExpressionFunctionType::IsNumeric;
    else if (functionNameContext->K_LASTINDEXOF() != nullptr)
        functionType = ExpressionFunctionType::LastIndexOf;
    else if (functionNameContext->K_LEN() != nullptr)
        functionType = ExpressionFunctionType::Len;
    else if (functionNameContext->K_LOWER() != nullptr)
        functionType = ExpressionFunctionType::Lower;
    else if (functionNameContext->K_MAXOF() != nullptr)
        functionType = ExpressionFunctionType::MaxOf;
    else if (functionNameContext->K_MINOF() != nullptr)
        functionType = ExpressionFunctionType::MinOf;
    else if (functionNameContext->K_NOW() != nullptr)
        functionType = ExpressionFunctionType::Now;
    else if (functionNameContext->K_NTHINDEXOF() != nullptr)
        functionType = ExpressionFunctionType::NthIndexOf;
    else if (functionNameContext->K_POWER() != nullptr)
        functionType = ExpressionFunctionType::Power;
    else if (functionNameContext->K_REGEXMATCH() != nullptr)
        functionType = ExpressionFunctionType::RegExMatch;
    else if (functionNameContext->K_REGEXVAL() != nullptr)
        functionType = ExpressionFunctionType::RegExVal;
    else if (functionNameContext->K_REPLACE() != nullptr)
        functionType = ExpressionFunctionType::Replace;
    else if (functionNameContext->K_REVERSE() != nullptr)
        functionType = ExpressionFunctionType::Reverse;
    else if (functionNameContext->K_ROUND() != nullptr)
        functionType = ExpressionFunctionType::Round;
    else if (functionNameContext->K_SPLIT() != nullptr)
        functionType = ExpressionFunctionType::Split;
    else if (functionNameContext->K_SQRT() != nullptr)
        functionType = ExpressionFunctionType::Sqrt;
    else if (functionNameContext->K_STARTSWITH() != nullptr)
        functionType = ExpressionFunctionType::StartsWith;
    else if (functionNameContext->K_STRCOUNT() != nullptr)
        functionType = ExpressionFunctionType::StrCount;
    else if (functionNameContext->K_STRCMP() != nullptr)
        functionType = ExpressionFunctionType::StrCmp;
    else if (functionNameContext->K_SUBSTR() != nullptr)
        functionType = ExpressionFunctionType::SubStr;
    else if (functionNameContext->K_TRIM() != nullptr)
        functionType = ExpressionFunctionType::Trim;
    else if (functionNameContext->K_TRIMLEFT() != nullptr)
        functionType = ExpressionFunctionType::TrimLeft;
    else if (functionNameContext->K_TRIMRIGHT() != nullptr)
        functionType = ExpressionFunctionType::TrimRight;
    else if (functionNameContext->K_UPPER() != nullptr)
        functionType = ExpressionFunctionType::Upper;
    else if (functionNameContext->K_UTCNOW() != nullptr)
        functionType = ExpressionFunctionType::UtcNow;
    else
        throw FilterExpressionParserException("Unexpected function type \"" + functionNameContext->getText() + "\"");

    const ExpressionCollectionPtr arguments = NewSharedPtr<ExpressionCollection>();
    auto* expressionList = context->expressionList();

    if (expressionList != nullptr)
    {
        const size_t argumentCount = expressionList->expression().size();

        for (size_t i = 0; i < argumentCount; i++)
        {
            ExpressionPtr argument;

            if (TryGetExpr(expressionList->expression(i), argument))
                arguments->push_back(argument);
            else
                throw FilterExpressionParserException("Failed to find argument expression " + ToString(i) + " \"" + expressionList->expression(i)->getText() + "\" for function \"" + functionNameContext->getText() + "\"");
        }
    }

    AddExpr(context, NewSharedPtr<FunctionExpression>(functionType, arguments));
}

vector<ExpressionTreePtr> FilterExpressionParser::GenerateExpressionTrees(const DataSetPtr& dataSet, const string& primaryTableName, const string& filterExpression, const bool suppressConsoleErrorOutput)
{
    const FilterExpressionParserPtr parser = NewSharedPtr<FilterExpressionParser>(filterExpression, suppressConsoleErrorOutput);

    parser->SetDataSet(dataSet);
    parser->SetPrimaryTableName(primaryTableName);
    parser->SetTrackFilteredRows(false);

    return parser->GetExpressionTrees();
}

vector<ExpressionTreePtr> FilterExpressionParser::GenerateExpressionTrees(const DataTablePtr& dataTable, const string& filterExpression, const bool suppressConsoleErrorOutput)
{
    return GenerateExpressionTrees(dataTable->Parent(), dataTable->Name(), filterExpression, suppressConsoleErrorOutput);
}

ExpressionTreePtr FilterExpressionParser::GenerateExpressionTree(const DataTablePtr& dataTable, const string& filterExpression, const bool suppressConsoleErrorOutput)
{
    const vector<ExpressionTreePtr> expressionTrees = GenerateExpressionTrees(dataTable, filterExpression, suppressConsoleErrorOutput);

    if (!expressionTrees.empty())
        return expressionTrees[0];

    throw FilterExpressionParserException("No expression trees generated with filter expression \"" + filterExpression + "\" for table \"" + dataTable->Name() + "\"");
}

ValueExpressionPtr FilterExpressionParser::Evaluate(const DataRowPtr& dataRow, const string& filterExpression, const bool suppressConsoleErrorOutput)
{
    return GenerateExpressionTree(dataRow->Parent(), filterExpression, suppressConsoleErrorOutput)->Evaluate(dataRow);
}

vector<DataRowPtr> FilterExpressionParser::Select(const DataSetPtr& dataSet, const string& filterExpression, const string& primaryTableName, const TableIDFieldsPtr& tableIDFields, const bool suppressConsoleErrorOutput)
{
    const FilterExpressionParserPtr parser = NewSharedPtr<FilterExpressionParser>(filterExpression, suppressConsoleErrorOutput);

    parser->SetDataSet(dataSet);

    if (!primaryTableName.empty())
    {
        parser->SetPrimaryTableName(primaryTableName);
        parser->SetTableIDFields(primaryTableName, tableIDFields == nullptr ? DefaultTableIDFields : tableIDFields);
    }

    parser->Evaluate();

    return parser->m_filteredRows;
}

vector<DataRowPtr> FilterExpressionParser::Select(const DataTablePtr& dataTable, const string& filterExpression, const TableIDFieldsPtr& tableIDFields, const bool suppressConsoleErrorOutput)
{
    return Select(dataTable->Parent(), filterExpression, dataTable->Name(), tableIDFields, suppressConsoleErrorOutput);
}

vector<DataRowPtr> FilterExpressionParser::Select(const ExpressionTreePtr& expressionTree)
{
    const DataTablePtr& table = expressionTree->Table();
    vector<DataRowPtr> matchedRows;

    for (int32_t i = 0; i < table->RowCount(); i++)
    {
        if (expressionTree->TopLimit > -1 && static_cast<int32_t>(matchedRows.size()) >= expressionTree->TopLimit)
            break;

        const DataRowPtr& row = table->Row(i);

        if (row == nullptr)
            continue;

        const ValueExpressionPtr& resultExpression = expressionTree->Evaluate(row);

        // Final expression should have a boolean data type (it's part of a WHERE clause)
        if (resultExpression->ValueType != ExpressionValueType::Boolean)
            throw FilterExpressionParserException("Final expression tree evaluation did not result in a boolean value, result data type is \"" + string(EnumName(resultExpression->ValueType)) + "\"");

        // If final result is Null, i.e., has no value due to Null propagation, treat result as False
        if (resultExpression->ValueAsBoolean())
            matchedRows.push_back(row);
    }

    if (!matchedRows.empty() && !expressionTree->OrderByTerms.empty())
    {
        ranges::sort(matchedRows, [expressionTree](const DataRowPtr& leftMatchedRow, const DataRowPtr& rightMatchedRow)
        {
            for (auto& orderByTerm : expressionTree->OrderByTerms)
            {
                const DataColumnPtr& orderByColumn = get<0>(orderByTerm);
                const int32_t columnIndex = orderByColumn->Index();
                const bool ascending = get<1>(orderByTerm);
                const bool exactMatch = get<2>(orderByTerm);
                const DataRowPtr& leftRow = ascending ? leftMatchedRow : rightMatchedRow;
                const DataRowPtr& rightRow = ascending ? rightMatchedRow : leftMatchedRow;
                int32_t result;

                switch (orderByColumn->Type())
                {
                    case DataType::String:
                    {
                        auto leftNullable = leftRow->ValueAsString(columnIndex);
                        auto rightNullable = rightRow->ValueAsString(columnIndex);

                        if (leftNullable.HasValue() && rightNullable.HasValue())
                            result = Compare(leftNullable.GetValueOrDefault(), rightNullable.GetValueOrDefault(), !exactMatch);
                        else if (!leftNullable.HasValue() && !rightNullable.HasValue())
                            result = 0;
                        else
                            result = leftNullable.HasValue() ? 1 : -1;

                        break;
                    }
                    case DataType::Boolean:
                        result = CompareValues(leftRow->ValueAsBoolean(columnIndex), rightRow->ValueAsBoolean(columnIndex));
                        break;
                    case DataType::DateTime:
                        result = CompareValues(leftRow->ValueAsDateTime(columnIndex), rightRow->ValueAsDateTime(columnIndex));
                        break;
                    case DataType::Single:
                        result = CompareValues(leftRow->ValueAsSingle(columnIndex), rightRow->ValueAsSingle(columnIndex));
                        break;
                    case DataType::Double:
                        result = CompareValues(leftRow->ValueAsDouble(columnIndex), rightRow->ValueAsDouble(columnIndex));
                        break;
                    case DataType::Decimal:
                        result = CompareValues(leftRow->ValueAsDecimal(columnIndex), rightRow->ValueAsDecimal(columnIndex));
                        break;
                    case DataType::Guid:
                        result = CompareValues(leftRow->ValueAsGuid(columnIndex), rightRow->ValueAsGuid(columnIndex));
                        break;
                    case DataType::Int8:
                        result = CompareValues(leftRow->ValueAsInt8(columnIndex), rightRow->ValueAsInt8(columnIndex));
                        break;
                    case DataType::Int16:
                        result = CompareValues(leftRow->ValueAsInt16(columnIndex), rightRow->ValueAsInt16(columnIndex));
                        break;
                    case DataType::Int32:
                        result = CompareValues(leftRow->ValueAsInt32(columnIndex), rightRow->ValueAsInt32(columnIndex));
                        break;
                    case DataType::Int64:
                        result = CompareValues(leftRow->ValueAsInt64(columnIndex), rightRow->ValueAsInt64(columnIndex));
                        break;
                    case DataType::UInt8:
                        result = CompareValues(leftRow->ValueAsUInt8(columnIndex), rightRow->ValueAsUInt8(columnIndex));
                        break;
                    case DataType::UInt16:
                        result = CompareValues(leftRow->ValueAsUInt16(columnIndex), rightRow->ValueAsUInt16(columnIndex));
                        break;
                    case DataType::UInt32:
                        result = CompareValues(leftRow->ValueAsUInt32(columnIndex), rightRow->ValueAsUInt32(columnIndex));
                        break;
                    case DataType::UInt64:
                        result = CompareValues(leftRow->ValueAsUInt64(columnIndex), rightRow->ValueAsUInt64(columnIndex));
                        break;
                    default:
                        throw FilterExpressionParserException("Unexpected column data type encountered");
                }

                if (result < 0)
                    return true;

                if (result > 0)
                    return false;

                // Last compare result was equal, continue sort based on next order-by term
            }

            return false;
        });
    }

    return matchedRows;
}

const TableIDFieldsPtr FilterExpressionParser::DefaultTableIDFields = []
{ 
    TableIDFieldsPtr defaultTableIDFields = NewSharedPtr<TableIDFields>();

    defaultTableIDFields->SignalIDFieldName = "SignalID";
    defaultTableIDFields->MeasurementKeyFieldName = "ID";
    defaultTableIDFields->PointTagFieldName = "PointTag";

    return defaultTableIDFields;
}();