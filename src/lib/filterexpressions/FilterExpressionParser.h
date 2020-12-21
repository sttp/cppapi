//******************************************************************************************************
//  FilterExpressionParser.h - Gbtc
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
//  11/01/2018 - J. Ritchie Carroll
//       Generated original version of source code.
//
//******************************************************************************************************

#pragma once

// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#include "FilterExpressionSyntaxBaseListener.h"
#include "FilterExpressionSyntaxLexer.h"

// For some reason this symbol gets undefined? So, we re-define it...
#ifndef EOF
#define EOF (-1)
#endif

#include "../../CommonTypes.h"
#include "../../data/DataSet.h"
#include "ExpressionTree.h"

#ifdef _DEBUG
#define SUPPRESS_CONSOLE_ERROR_OUTPUT false
#else
#define SUPPRESS_CONSOLE_ERROR_OUTPUT true
#endif

namespace sttp {
namespace filterexpressions
{
    // Simple exception type thrown by the filter expression parser
    class FilterExpressionParserException : public Exception
    {
    private:
        std::string m_message;

    public:
        FilterExpressionParserException(std::string message) noexcept;
        const char* what() const noexcept;
    };

    struct TableIDFields
    {
        std::string SignalIDFieldName;
        std::string MeasurementKeyFieldName;
        std::string PointTagFieldName;
    };

    typedef sttp::SharedPtr<TableIDFields> TableIDFieldsPtr;

    class FilterExpressionParser;
    typedef sttp::SharedPtr<FilterExpressionParser> FilterExpressionParserPtr;

    typedef std::function<void(FilterExpressionParserPtr, const std::string&)> ParsingExceptionCallback;

    class FilterExpressionParser : // NOLINT
        public FilterExpressionSyntaxBaseListener,
        public sttp::EnableSharedThisPtr<FilterExpressionParser>
    {
    private:
        class CallbackErrorListener : public antlr4::BaseErrorListener
        {
        private:
            const FilterExpressionParserPtr m_filterExpressionParser;
            const ParsingExceptionCallback m_parsingExceptionCallback;

        public:
            CallbackErrorListener(FilterExpressionParserPtr filterExpressionParser, ParsingExceptionCallback parsingExceptionCallback);
            void syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string& msg, std::exception_ptr e) override;
        };

        antlr4::ANTLRInputStream m_inputStream;
        FilterExpressionSyntaxLexer* m_lexer;
        antlr4::CommonTokenStream* m_tokens;
        FilterExpressionSyntaxParser* m_parser;
        CallbackErrorListener* m_callbackErrorListener;
        sttp::data::DataSetPtr m_dataSet;
        std::string m_primaryTableName;
        sttp::StringMap<TableIDFieldsPtr> m_tableIDFields;

        bool m_trackFilteredRows;
        std::vector<sttp::data::DataRowPtr> m_filteredRows;
        std::unordered_set<sttp::data::DataRowPtr> m_filteredRowSet;

        bool m_trackFilteredSignalIDs;
        std::vector<sttp::Guid> m_filteredSignalIDs;
        std::unordered_set<sttp::Guid> m_filteredSignalIDSet;

        int32_t m_filterExpressionStatementCount;

        ExpressionTreePtr m_activeExpressionTree;
        std::vector<ExpressionTreePtr> m_expressionTrees;
        std::map<const antlr4::ParserRuleContext*, ExpressionPtr> m_expressions;

        void VisitParseTreeNodes();
        void InitializeSetOperations();
        inline void AddMatchedRow(const sttp::data::DataRowPtr& row, int32_t signalIDColumnIndex);
        inline void MapMatchedFieldRow(const sttp::data::DataTablePtr& primaryTable, const std::string& columnName, const std::string& matchValue, int32_t signalIDColumnIndex);
        inline bool TryGetExpr(const antlr4::ParserRuleContext* context, ExpressionPtr& expression) const;
        inline void AddExpr(const antlr4::ParserRuleContext* context, const ExpressionPtr& expression);

        static ValueExpressionPtr ParseNumericLiteral(const std::string& literal);
    public:
        FilterExpressionParser(const std::string& filterExpression, bool suppressConsoleErrorOutput = SUPPRESS_CONSOLE_ERROR_OUTPUT);
        ~FilterExpressionParser() override;

        const sttp::data::DataSetPtr& GetDataSet() const;
        void SetDataSet(const sttp::data::DataSetPtr& dataSet);

        TableIDFieldsPtr GetTableIDFields(const std::string& tableName) const;
        void SetTableIDFields(const std::string& tableName, const TableIDFieldsPtr& tableIDFields);

        const std::string& GetPrimaryTableName() const;
        void SetPrimaryTableName(const std::string& tableName);

        // ParsingExceptionCallback function is defined with the following signature:
        //   void HandleParsingException(FilterExpressionParserPtr, const string& message)
        void RegisterParsingExceptionCallback(const ParsingExceptionCallback& parsingExceptionCallback);

        void Evaluate();

        bool GetTrackFilteredRows() const;
        void SetTrackFilteredRows(bool trackFilteredRows);
        const std::vector<sttp::data::DataRowPtr>& FilteredRows() const;
        const std::unordered_set<sttp::data::DataRowPtr>& FilteredRowSet();

        bool GetTrackFilteredSignalIDs() const;
        void SetTrackFilteredSignalIDs(bool trackFilteredSignalIDs);
        const std::vector<sttp::Guid>& FilteredSignalIDs() const;
        const std::unordered_set<sttp::Guid>& FilteredSignalIDSet();

        const std::vector<ExpressionTreePtr>& GetExpressionTrees();

        void enterFilterExpressionStatement(FilterExpressionSyntaxParser::FilterExpressionStatementContext*) override;
        void enterFilterStatement(FilterExpressionSyntaxParser::FilterStatementContext*) override;
        void exitIdentifierStatement(FilterExpressionSyntaxParser::IdentifierStatementContext*) override;
        void enterExpression(FilterExpressionSyntaxParser::ExpressionContext*) override;
        void exitExpression(FilterExpressionSyntaxParser::ExpressionContext*) override;
        void exitPredicateExpression(FilterExpressionSyntaxParser::PredicateExpressionContext*) override;
        void exitValueExpression(FilterExpressionSyntaxParser::ValueExpressionContext*) override;
        void exitLiteralValue(FilterExpressionSyntaxParser::LiteralValueContext*) override;
        void exitColumnName(FilterExpressionSyntaxParser::ColumnNameContext*) override;
        void exitFunctionExpression(FilterExpressionSyntaxParser::FunctionExpressionContext*) override;

        static std::vector<ExpressionTreePtr> GenerateExpressionTrees(const sttp::data::DataSetPtr& dataSet, const std::string& primaryTableName, const std::string& filterExpression, bool suppressConsoleErrorOutput = SUPPRESS_CONSOLE_ERROR_OUTPUT);
        static std::vector<ExpressionTreePtr> GenerateExpressionTrees(const sttp::data::DataTablePtr& dataTable, const std::string& filterExpression, bool suppressConsoleErrorOutput = SUPPRESS_CONSOLE_ERROR_OUTPUT);
        static ExpressionTreePtr GenerateExpressionTree(const sttp::data::DataTablePtr& dataTable, const std::string& filterExpression, bool suppressConsoleErrorOutput = SUPPRESS_CONSOLE_ERROR_OUTPUT);
    
        static ValueExpressionPtr Evaluate(const sttp::data::DataRowPtr& dataRow, const std::string& filterExpression, bool suppressConsoleErrorOutput = SUPPRESS_CONSOLE_ERROR_OUTPUT);
    
        static std::vector<sttp::data::DataRowPtr> Select(const sttp::data::DataSetPtr& dataSet, const std::string& filterExpression, const std::string& primaryTableName = {}, const TableIDFieldsPtr& tableIDFields = nullptr, bool suppressConsoleErrorOutput = SUPPRESS_CONSOLE_ERROR_OUTPUT);
        static std::vector<sttp::data::DataRowPtr> Select(const sttp::data::DataTablePtr& dataTable, const std::string& filterExpression, const TableIDFieldsPtr& tableIDFields = nullptr, bool suppressConsoleErrorOutput = SUPPRESS_CONSOLE_ERROR_OUTPUT);
        static std::vector<sttp::data::DataRowPtr> Select(const ExpressionTreePtr& expressionTree);

        static const TableIDFieldsPtr DefaultTableIDFields;
    };

    typedef sttp::SharedPtr<FilterExpressionParser> FilterExpressionParserPtr;
}}