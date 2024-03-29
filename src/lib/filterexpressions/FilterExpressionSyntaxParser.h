
// Generated from FilterExpressionSyntax.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"




class  FilterExpressionSyntaxParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, K_ABS = 29, K_AND = 30, K_ASC = 31, K_BINARY = 32, 
    K_BY = 33, K_CEILING = 34, K_COALESCE = 35, K_CONVERT = 36, K_CONTAINS = 37, 
    K_DATEADD = 38, K_DATEDIFF = 39, K_DATEPART = 40, K_DESC = 41, K_ENDSWITH = 42, 
    K_FILTER = 43, K_FLOOR = 44, K_IIF = 45, K_IN = 46, K_INDEXOF = 47, 
    K_IS = 48, K_ISDATE = 49, K_ISINTEGER = 50, K_ISGUID = 51, K_ISNULL = 52, 
    K_ISNUMERIC = 53, K_LASTINDEXOF = 54, K_LEN = 55, K_LIKE = 56, K_LOWER = 57, 
    K_MAXOF = 58, K_MINOF = 59, K_NOT = 60, K_NOW = 61, K_NTHINDEXOF = 62, 
    K_NULL = 63, K_OR = 64, K_ORDER = 65, K_POWER = 66, K_REGEXMATCH = 67, 
    K_REGEXVAL = 68, K_REPLACE = 69, K_REVERSE = 70, K_ROUND = 71, K_SQRT = 72, 
    K_SPLIT = 73, K_STARTSWITH = 74, K_STRCOUNT = 75, K_STRCMP = 76, K_SUBSTR = 77, 
    K_TOP = 78, K_TRIM = 79, K_TRIMLEFT = 80, K_TRIMRIGHT = 81, K_UPPER = 82, 
    K_UTCNOW = 83, K_WHERE = 84, K_XOR = 85, BOOLEAN_LITERAL = 86, IDENTIFIER = 87, 
    INTEGER_LITERAL = 88, NUMERIC_LITERAL = 89, GUID_LITERAL = 90, MEASUREMENT_KEY_LITERAL = 91, 
    POINT_TAG_LITERAL = 92, STRING_LITERAL = 93, DATETIME_LITERAL = 94, 
    SINGLE_LINE_COMMENT = 95, MULTILINE_COMMENT = 96, SPACES = 97, UNEXPECTED_CHAR = 98
  };

  enum {
    RuleParse = 0, RuleErr = 1, RuleFilterExpressionStatementList = 2, RuleFilterExpressionStatement = 3, 
    RuleIdentifierStatement = 4, RuleFilterStatement = 5, RuleTopLimit = 6, 
    RuleOrderingTerm = 7, RuleExpressionList = 8, RuleExpression = 9, RulePredicateExpression = 10, 
    RuleValueExpression = 11, RuleNotOperator = 12, RuleUnaryOperator = 13, 
    RuleExactMatchModifier = 14, RuleComparisonOperator = 15, RuleLogicalOperator = 16, 
    RuleBitwiseOperator = 17, RuleMathOperator = 18, RuleFunctionName = 19, 
    RuleFunctionExpression = 20, RuleLiteralValue = 21, RuleTableName = 22, 
    RuleColumnName = 23, RuleOrderByColumnName = 24
  };

  explicit FilterExpressionSyntaxParser(antlr4::TokenStream *input);

  FilterExpressionSyntaxParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~FilterExpressionSyntaxParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ParseContext;
  class ErrContext;
  class FilterExpressionStatementListContext;
  class FilterExpressionStatementContext;
  class IdentifierStatementContext;
  class FilterStatementContext;
  class TopLimitContext;
  class OrderingTermContext;
  class ExpressionListContext;
  class ExpressionContext;
  class PredicateExpressionContext;
  class ValueExpressionContext;
  class NotOperatorContext;
  class UnaryOperatorContext;
  class ExactMatchModifierContext;
  class ComparisonOperatorContext;
  class LogicalOperatorContext;
  class BitwiseOperatorContext;
  class MathOperatorContext;
  class FunctionNameContext;
  class FunctionExpressionContext;
  class LiteralValueContext;
  class TableNameContext;
  class ColumnNameContext;
  class OrderByColumnNameContext; 

  class  ParseContext : public antlr4::ParserRuleContext {
  public:
    ParseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    FilterExpressionStatementListContext *filterExpressionStatementList();
    ErrContext *err();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParseContext* parse();

  class  ErrContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *unexpected_charToken = nullptr;
    ErrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UNEXPECTED_CHAR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ErrContext* err();

  class  FilterExpressionStatementListContext : public antlr4::ParserRuleContext {
  public:
    FilterExpressionStatementListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<FilterExpressionStatementContext *> filterExpressionStatement();
    FilterExpressionStatementContext* filterExpressionStatement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FilterExpressionStatementListContext* filterExpressionStatementList();

  class  FilterExpressionStatementContext : public antlr4::ParserRuleContext {
  public:
    FilterExpressionStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierStatementContext *identifierStatement();
    FilterStatementContext *filterStatement();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FilterExpressionStatementContext* filterExpressionStatement();

  class  IdentifierStatementContext : public antlr4::ParserRuleContext {
  public:
    IdentifierStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GUID_LITERAL();
    antlr4::tree::TerminalNode *MEASUREMENT_KEY_LITERAL();
    antlr4::tree::TerminalNode *POINT_TAG_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  IdentifierStatementContext* identifierStatement();

  class  FilterStatementContext : public antlr4::ParserRuleContext {
  public:
    FilterStatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *K_FILTER();
    TableNameContext *tableName();
    antlr4::tree::TerminalNode *K_WHERE();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *K_TOP();
    TopLimitContext *topLimit();
    antlr4::tree::TerminalNode *K_ORDER();
    antlr4::tree::TerminalNode *K_BY();
    std::vector<OrderingTermContext *> orderingTerm();
    OrderingTermContext* orderingTerm(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FilterStatementContext* filterStatement();

  class  TopLimitContext : public antlr4::ParserRuleContext {
  public:
    TopLimitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTEGER_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TopLimitContext* topLimit();

  class  OrderingTermContext : public antlr4::ParserRuleContext {
  public:
    OrderingTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    OrderByColumnNameContext *orderByColumnName();
    ExactMatchModifierContext *exactMatchModifier();
    antlr4::tree::TerminalNode *K_ASC();
    antlr4::tree::TerminalNode *K_DESC();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OrderingTermContext* orderingTerm();

  class  ExpressionListContext : public antlr4::ParserRuleContext {
  public:
    ExpressionListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExpressionListContext* expressionList();

  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NotOperatorContext *notOperator();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    PredicateExpressionContext *predicateExpression();
    LogicalOperatorContext *logicalOperator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExpressionContext* expression();
  ExpressionContext* expression(int precedence);
  class  PredicateExpressionContext : public antlr4::ParserRuleContext {
  public:
    PredicateExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ValueExpressionContext *valueExpression();
    std::vector<PredicateExpressionContext *> predicateExpression();
    PredicateExpressionContext* predicateExpression(size_t i);
    ComparisonOperatorContext *comparisonOperator();
    antlr4::tree::TerminalNode *K_LIKE();
    NotOperatorContext *notOperator();
    ExactMatchModifierContext *exactMatchModifier();
    antlr4::tree::TerminalNode *K_IN();
    ExpressionListContext *expressionList();
    antlr4::tree::TerminalNode *K_IS();
    antlr4::tree::TerminalNode *K_NULL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PredicateExpressionContext* predicateExpression();
  PredicateExpressionContext* predicateExpression(int precedence);
  class  ValueExpressionContext : public antlr4::ParserRuleContext {
  public:
    ValueExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LiteralValueContext *literalValue();
    ColumnNameContext *columnName();
    FunctionExpressionContext *functionExpression();
    UnaryOperatorContext *unaryOperator();
    std::vector<ValueExpressionContext *> valueExpression();
    ValueExpressionContext* valueExpression(size_t i);
    ExpressionContext *expression();
    MathOperatorContext *mathOperator();
    BitwiseOperatorContext *bitwiseOperator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ValueExpressionContext* valueExpression();
  ValueExpressionContext* valueExpression(int precedence);
  class  NotOperatorContext : public antlr4::ParserRuleContext {
  public:
    NotOperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *K_NOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  NotOperatorContext* notOperator();

  class  UnaryOperatorContext : public antlr4::ParserRuleContext {
  public:
    UnaryOperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *K_NOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  UnaryOperatorContext* unaryOperator();

  class  ExactMatchModifierContext : public antlr4::ParserRuleContext {
  public:
    ExactMatchModifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *K_BINARY();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExactMatchModifierContext* exactMatchModifier();

  class  ComparisonOperatorContext : public antlr4::ParserRuleContext {
  public:
    ComparisonOperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ComparisonOperatorContext* comparisonOperator();

  class  LogicalOperatorContext : public antlr4::ParserRuleContext {
  public:
    LogicalOperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *K_AND();
    antlr4::tree::TerminalNode *K_OR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  LogicalOperatorContext* logicalOperator();

  class  BitwiseOperatorContext : public antlr4::ParserRuleContext {
  public:
    BitwiseOperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *K_XOR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BitwiseOperatorContext* bitwiseOperator();

  class  MathOperatorContext : public antlr4::ParserRuleContext {
  public:
    MathOperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MathOperatorContext* mathOperator();

  class  FunctionNameContext : public antlr4::ParserRuleContext {
  public:
    FunctionNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *K_ABS();
    antlr4::tree::TerminalNode *K_CEILING();
    antlr4::tree::TerminalNode *K_COALESCE();
    antlr4::tree::TerminalNode *K_CONVERT();
    antlr4::tree::TerminalNode *K_CONTAINS();
    antlr4::tree::TerminalNode *K_DATEADD();
    antlr4::tree::TerminalNode *K_DATEDIFF();
    antlr4::tree::TerminalNode *K_DATEPART();
    antlr4::tree::TerminalNode *K_ENDSWITH();
    antlr4::tree::TerminalNode *K_FLOOR();
    antlr4::tree::TerminalNode *K_IIF();
    antlr4::tree::TerminalNode *K_INDEXOF();
    antlr4::tree::TerminalNode *K_ISDATE();
    antlr4::tree::TerminalNode *K_ISINTEGER();
    antlr4::tree::TerminalNode *K_ISGUID();
    antlr4::tree::TerminalNode *K_ISNULL();
    antlr4::tree::TerminalNode *K_ISNUMERIC();
    antlr4::tree::TerminalNode *K_LASTINDEXOF();
    antlr4::tree::TerminalNode *K_LEN();
    antlr4::tree::TerminalNode *K_LOWER();
    antlr4::tree::TerminalNode *K_MAXOF();
    antlr4::tree::TerminalNode *K_MINOF();
    antlr4::tree::TerminalNode *K_NOW();
    antlr4::tree::TerminalNode *K_NTHINDEXOF();
    antlr4::tree::TerminalNode *K_POWER();
    antlr4::tree::TerminalNode *K_REGEXMATCH();
    antlr4::tree::TerminalNode *K_REGEXVAL();
    antlr4::tree::TerminalNode *K_REPLACE();
    antlr4::tree::TerminalNode *K_REVERSE();
    antlr4::tree::TerminalNode *K_ROUND();
    antlr4::tree::TerminalNode *K_SPLIT();
    antlr4::tree::TerminalNode *K_SQRT();
    antlr4::tree::TerminalNode *K_STARTSWITH();
    antlr4::tree::TerminalNode *K_STRCOUNT();
    antlr4::tree::TerminalNode *K_STRCMP();
    antlr4::tree::TerminalNode *K_SUBSTR();
    antlr4::tree::TerminalNode *K_TRIM();
    antlr4::tree::TerminalNode *K_TRIMLEFT();
    antlr4::tree::TerminalNode *K_TRIMRIGHT();
    antlr4::tree::TerminalNode *K_UPPER();
    antlr4::tree::TerminalNode *K_UTCNOW();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FunctionNameContext* functionName();

  class  FunctionExpressionContext : public antlr4::ParserRuleContext {
  public:
    FunctionExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FunctionNameContext *functionName();
    ExpressionListContext *expressionList();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FunctionExpressionContext* functionExpression();

  class  LiteralValueContext : public antlr4::ParserRuleContext {
  public:
    LiteralValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTEGER_LITERAL();
    antlr4::tree::TerminalNode *NUMERIC_LITERAL();
    antlr4::tree::TerminalNode *STRING_LITERAL();
    antlr4::tree::TerminalNode *DATETIME_LITERAL();
    antlr4::tree::TerminalNode *GUID_LITERAL();
    antlr4::tree::TerminalNode *BOOLEAN_LITERAL();
    antlr4::tree::TerminalNode *K_NULL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  LiteralValueContext* literalValue();

  class  TableNameContext : public antlr4::ParserRuleContext {
  public:
    TableNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TableNameContext* tableName();

  class  ColumnNameContext : public antlr4::ParserRuleContext {
  public:
    ColumnNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ColumnNameContext* columnName();

  class  OrderByColumnNameContext : public antlr4::ParserRuleContext {
  public:
    OrderByColumnNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OrderByColumnNameContext* orderByColumnName();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool expressionSempred(ExpressionContext *_localctx, size_t predicateIndex);
  bool predicateExpressionSempred(PredicateExpressionContext *_localctx, size_t predicateIndex);
  bool valueExpressionSempred(ValueExpressionContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

