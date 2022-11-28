
// Generated from FilterExpressionSyntax.g4 by ANTLR 4.11.1


#include "FilterExpressionSyntaxListener.h"

#include "FilterExpressionSyntaxParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct FilterExpressionSyntaxParserStaticData final {
  FilterExpressionSyntaxParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  FilterExpressionSyntaxParserStaticData(const FilterExpressionSyntaxParserStaticData&) = delete;
  FilterExpressionSyntaxParserStaticData(FilterExpressionSyntaxParserStaticData&&) = delete;
  FilterExpressionSyntaxParserStaticData& operator=(const FilterExpressionSyntaxParserStaticData&) = delete;
  FilterExpressionSyntaxParserStaticData& operator=(FilterExpressionSyntaxParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag filterexpressionsyntaxParserOnceFlag;
FilterExpressionSyntaxParserStaticData *filterexpressionsyntaxParserStaticData = nullptr;

void filterexpressionsyntaxParserInitialize() {
  assert(filterexpressionsyntaxParserStaticData == nullptr);
  auto staticData = std::make_unique<FilterExpressionSyntaxParserStaticData>(
    std::vector<std::string>{
      "parse", "err", "filterExpressionStatementList", "filterExpressionStatement", 
      "identifierStatement", "filterStatement", "topLimit", "orderingTerm", 
      "expressionList", "expression", "predicateExpression", "valueExpression", 
      "notOperator", "unaryOperator", "exactMatchModifier", "comparisonOperator", 
      "logicalOperator", "bitwiseOperator", "mathOperator", "functionName", 
      "functionExpression", "literalValue", "tableName", "columnName", "orderByColumnName"
    },
    std::vector<std::string>{
      "", "';'", "','", "'-'", "'+'", "'('", "')'", "'!'", "'~'", "'==='", 
      "'<'", "'<='", "'>'", "'>='", "'='", "'=='", "'!='", "'!=='", "'<>'", 
      "'&&'", "'||'", "'<<'", "'>>'", "'&'", "'|'", "'^'", "'*'", "'/'", 
      "'%'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "K_ABS", "K_AND", 
      "K_ASC", "K_BINARY", "K_BY", "K_CEILING", "K_COALESCE", "K_CONVERT", 
      "K_CONTAINS", "K_DATEADD", "K_DATEDIFF", "K_DATEPART", "K_DESC", "K_ENDSWITH", 
      "K_FILTER", "K_FLOOR", "K_IIF", "K_IN", "K_INDEXOF", "K_IS", "K_ISDATE", 
      "K_ISINTEGER", "K_ISGUID", "K_ISNULL", "K_ISNUMERIC", "K_LASTINDEXOF", 
      "K_LEN", "K_LIKE", "K_LOWER", "K_MAXOF", "K_MINOF", "K_NOT", "K_NOW", 
      "K_NTHINDEXOF", "K_NULL", "K_OR", "K_ORDER", "K_POWER", "K_REGEXMATCH", 
      "K_REGEXVAL", "K_REPLACE", "K_REVERSE", "K_ROUND", "K_SQRT", "K_SPLIT", 
      "K_STARTSWITH", "K_STRCOUNT", "K_STRCMP", "K_SUBSTR", "K_TOP", "K_TRIM", 
      "K_TRIMLEFT", "K_TRIMRIGHT", "K_UPPER", "K_UTCNOW", "K_WHERE", "K_XOR", 
      "BOOLEAN_LITERAL", "IDENTIFIER", "INTEGER_LITERAL", "NUMERIC_LITERAL", 
      "GUID_LITERAL", "MEASUREMENT_KEY_LITERAL", "POINT_TAG_LITERAL", "STRING_LITERAL", 
      "DATETIME_LITERAL", "SINGLE_LINE_COMMENT", "MULTILINE_COMMENT", "SPACES", 
      "UNEXPECTED_CHAR"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,98,243,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,1,0,1,0,3,0,53,8,0,1,0,1,0,1,1,1,1,1,
  	1,1,2,5,2,61,8,2,10,2,12,2,64,9,2,1,2,1,2,4,2,68,8,2,11,2,12,2,69,1,2,
  	5,2,73,8,2,10,2,12,2,76,9,2,1,2,5,2,79,8,2,10,2,12,2,82,9,2,1,3,1,3,1,
  	3,3,3,87,8,3,1,4,1,4,1,5,1,5,1,5,3,5,94,8,5,1,5,1,5,1,5,1,5,1,5,1,5,1,
  	5,1,5,5,5,104,8,5,10,5,12,5,107,9,5,3,5,109,8,5,1,6,3,6,112,8,6,1,6,1,
  	6,1,7,3,7,117,8,7,1,7,1,7,3,7,121,8,7,1,8,1,8,1,8,5,8,126,8,8,10,8,12,
  	8,129,9,8,1,9,1,9,1,9,1,9,1,9,3,9,136,8,9,1,9,1,9,1,9,1,9,5,9,142,8,9,
  	10,9,12,9,145,9,9,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,3,10,156,
  	8,10,1,10,1,10,3,10,160,8,10,1,10,1,10,1,10,3,10,165,8,10,1,10,1,10,3,
  	10,169,8,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,3,10,178,8,10,1,10,5,10,
  	181,8,10,10,10,12,10,184,9,10,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,
  	1,11,1,11,1,11,3,11,197,8,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,
  	5,11,207,8,11,10,11,12,11,210,9,11,1,12,1,12,1,13,1,13,1,14,1,14,1,15,
  	1,15,1,16,1,16,1,17,1,17,1,18,1,18,1,19,1,19,1,20,1,20,1,20,3,20,231,
  	8,20,1,20,1,20,1,21,1,21,1,22,1,22,1,23,1,23,1,24,1,24,1,24,0,3,18,20,
  	22,25,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,
  	46,48,0,12,1,0,90,92,1,0,3,4,2,0,31,31,41,41,2,0,7,7,60,60,3,0,3,4,7,
  	8,60,60,2,0,9,9,32,32,1,0,9,18,3,0,19,20,30,30,64,64,2,0,21,25,85,85,
  	2,0,3,4,26,28,10,0,29,29,34,40,42,42,44,45,47,47,49,55,57,59,61,62,66,
  	77,79,83,4,0,63,63,86,86,88,90,93,94,249,0,52,1,0,0,0,2,56,1,0,0,0,4,
  	62,1,0,0,0,6,86,1,0,0,0,8,88,1,0,0,0,10,90,1,0,0,0,12,111,1,0,0,0,14,
  	116,1,0,0,0,16,122,1,0,0,0,18,135,1,0,0,0,20,146,1,0,0,0,22,196,1,0,0,
  	0,24,211,1,0,0,0,26,213,1,0,0,0,28,215,1,0,0,0,30,217,1,0,0,0,32,219,
  	1,0,0,0,34,221,1,0,0,0,36,223,1,0,0,0,38,225,1,0,0,0,40,227,1,0,0,0,42,
  	234,1,0,0,0,44,236,1,0,0,0,46,238,1,0,0,0,48,240,1,0,0,0,50,53,3,4,2,
  	0,51,53,3,2,1,0,52,50,1,0,0,0,52,51,1,0,0,0,53,54,1,0,0,0,54,55,5,0,0,
  	1,55,1,1,0,0,0,56,57,5,98,0,0,57,58,6,1,-1,0,58,3,1,0,0,0,59,61,5,1,0,
  	0,60,59,1,0,0,0,61,64,1,0,0,0,62,60,1,0,0,0,62,63,1,0,0,0,63,65,1,0,0,
  	0,64,62,1,0,0,0,65,74,3,6,3,0,66,68,5,1,0,0,67,66,1,0,0,0,68,69,1,0,0,
  	0,69,67,1,0,0,0,69,70,1,0,0,0,70,71,1,0,0,0,71,73,3,6,3,0,72,67,1,0,0,
  	0,73,76,1,0,0,0,74,72,1,0,0,0,74,75,1,0,0,0,75,80,1,0,0,0,76,74,1,0,0,
  	0,77,79,5,1,0,0,78,77,1,0,0,0,79,82,1,0,0,0,80,78,1,0,0,0,80,81,1,0,0,
  	0,81,5,1,0,0,0,82,80,1,0,0,0,83,87,3,8,4,0,84,87,3,10,5,0,85,87,3,18,
  	9,0,86,83,1,0,0,0,86,84,1,0,0,0,86,85,1,0,0,0,87,7,1,0,0,0,88,89,7,0,
  	0,0,89,9,1,0,0,0,90,93,5,43,0,0,91,92,5,78,0,0,92,94,3,12,6,0,93,91,1,
  	0,0,0,93,94,1,0,0,0,94,95,1,0,0,0,95,96,3,44,22,0,96,97,5,84,0,0,97,108,
  	3,18,9,0,98,99,5,65,0,0,99,100,5,33,0,0,100,105,3,14,7,0,101,102,5,2,
  	0,0,102,104,3,14,7,0,103,101,1,0,0,0,104,107,1,0,0,0,105,103,1,0,0,0,
  	105,106,1,0,0,0,106,109,1,0,0,0,107,105,1,0,0,0,108,98,1,0,0,0,108,109,
  	1,0,0,0,109,11,1,0,0,0,110,112,7,1,0,0,111,110,1,0,0,0,111,112,1,0,0,
  	0,112,113,1,0,0,0,113,114,5,88,0,0,114,13,1,0,0,0,115,117,3,28,14,0,116,
  	115,1,0,0,0,116,117,1,0,0,0,117,118,1,0,0,0,118,120,3,48,24,0,119,121,
  	7,2,0,0,120,119,1,0,0,0,120,121,1,0,0,0,121,15,1,0,0,0,122,127,3,18,9,
  	0,123,124,5,2,0,0,124,126,3,18,9,0,125,123,1,0,0,0,126,129,1,0,0,0,127,
  	125,1,0,0,0,127,128,1,0,0,0,128,17,1,0,0,0,129,127,1,0,0,0,130,131,6,
  	9,-1,0,131,132,3,24,12,0,132,133,3,18,9,3,133,136,1,0,0,0,134,136,3,20,
  	10,0,135,130,1,0,0,0,135,134,1,0,0,0,136,143,1,0,0,0,137,138,10,2,0,0,
  	138,139,3,32,16,0,139,140,3,18,9,3,140,142,1,0,0,0,141,137,1,0,0,0,142,
  	145,1,0,0,0,143,141,1,0,0,0,143,144,1,0,0,0,144,19,1,0,0,0,145,143,1,
  	0,0,0,146,147,6,10,-1,0,147,148,3,22,11,0,148,182,1,0,0,0,149,150,10,
  	3,0,0,150,151,3,30,15,0,151,152,3,20,10,4,152,181,1,0,0,0,153,155,10,
  	2,0,0,154,156,3,24,12,0,155,154,1,0,0,0,155,156,1,0,0,0,156,157,1,0,0,
  	0,157,159,5,56,0,0,158,160,3,28,14,0,159,158,1,0,0,0,159,160,1,0,0,0,
  	160,161,1,0,0,0,161,181,3,20,10,3,162,164,10,5,0,0,163,165,3,24,12,0,
  	164,163,1,0,0,0,164,165,1,0,0,0,165,166,1,0,0,0,166,168,5,46,0,0,167,
  	169,3,28,14,0,168,167,1,0,0,0,168,169,1,0,0,0,169,170,1,0,0,0,170,171,
  	5,5,0,0,171,172,3,16,8,0,172,173,5,6,0,0,173,181,1,0,0,0,174,175,10,4,
  	0,0,175,177,5,48,0,0,176,178,3,24,12,0,177,176,1,0,0,0,177,178,1,0,0,
  	0,178,179,1,0,0,0,179,181,5,63,0,0,180,149,1,0,0,0,180,153,1,0,0,0,180,
  	162,1,0,0,0,180,174,1,0,0,0,181,184,1,0,0,0,182,180,1,0,0,0,182,183,1,
  	0,0,0,183,21,1,0,0,0,184,182,1,0,0,0,185,186,6,11,-1,0,186,197,3,42,21,
  	0,187,197,3,46,23,0,188,197,3,40,20,0,189,190,3,26,13,0,190,191,3,22,
  	11,4,191,197,1,0,0,0,192,193,5,5,0,0,193,194,3,18,9,0,194,195,5,6,0,0,
  	195,197,1,0,0,0,196,185,1,0,0,0,196,187,1,0,0,0,196,188,1,0,0,0,196,189,
  	1,0,0,0,196,192,1,0,0,0,197,208,1,0,0,0,198,199,10,2,0,0,199,200,3,36,
  	18,0,200,201,3,22,11,3,201,207,1,0,0,0,202,203,10,1,0,0,203,204,3,34,
  	17,0,204,205,3,22,11,2,205,207,1,0,0,0,206,198,1,0,0,0,206,202,1,0,0,
  	0,207,210,1,0,0,0,208,206,1,0,0,0,208,209,1,0,0,0,209,23,1,0,0,0,210,
  	208,1,0,0,0,211,212,7,3,0,0,212,25,1,0,0,0,213,214,7,4,0,0,214,27,1,0,
  	0,0,215,216,7,5,0,0,216,29,1,0,0,0,217,218,7,6,0,0,218,31,1,0,0,0,219,
  	220,7,7,0,0,220,33,1,0,0,0,221,222,7,8,0,0,222,35,1,0,0,0,223,224,7,9,
  	0,0,224,37,1,0,0,0,225,226,7,10,0,0,226,39,1,0,0,0,227,228,3,38,19,0,
  	228,230,5,5,0,0,229,231,3,16,8,0,230,229,1,0,0,0,230,231,1,0,0,0,231,
  	232,1,0,0,0,232,233,5,6,0,0,233,41,1,0,0,0,234,235,7,11,0,0,235,43,1,
  	0,0,0,236,237,5,87,0,0,237,45,1,0,0,0,238,239,5,87,0,0,239,47,1,0,0,0,
  	240,241,5,87,0,0,241,49,1,0,0,0,26,52,62,69,74,80,86,93,105,108,111,116,
  	120,127,135,143,155,159,164,168,177,180,182,196,206,208,230
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  filterexpressionsyntaxParserStaticData = staticData.release();
}

}

FilterExpressionSyntaxParser::FilterExpressionSyntaxParser(TokenStream *input) : FilterExpressionSyntaxParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

FilterExpressionSyntaxParser::FilterExpressionSyntaxParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  FilterExpressionSyntaxParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *filterexpressionsyntaxParserStaticData->atn, filterexpressionsyntaxParserStaticData->decisionToDFA, filterexpressionsyntaxParserStaticData->sharedContextCache, options);
}

FilterExpressionSyntaxParser::~FilterExpressionSyntaxParser() {
  delete _interpreter;
}

const atn::ATN& FilterExpressionSyntaxParser::getATN() const {
  return *filterexpressionsyntaxParserStaticData->atn;
}

std::string FilterExpressionSyntaxParser::getGrammarFileName() const {
  return "FilterExpressionSyntax.g4";
}

const std::vector<std::string>& FilterExpressionSyntaxParser::getRuleNames() const {
  return filterexpressionsyntaxParserStaticData->ruleNames;
}

const dfa::Vocabulary& FilterExpressionSyntaxParser::getVocabulary() const {
  return filterexpressionsyntaxParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView FilterExpressionSyntaxParser::getSerializedATN() const {
  return filterexpressionsyntaxParserStaticData->serializedATN;
}


//----------------- ParseContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::ParseContext::ParseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::ParseContext::EOF() {
  return getToken(FilterExpressionSyntaxParser::EOF, 0);
}

FilterExpressionSyntaxParser::FilterExpressionStatementListContext* FilterExpressionSyntaxParser::ParseContext::filterExpressionStatementList() {
  return getRuleContext<FilterExpressionSyntaxParser::FilterExpressionStatementListContext>(0);
}

FilterExpressionSyntaxParser::ErrContext* FilterExpressionSyntaxParser::ParseContext::err() {
  return getRuleContext<FilterExpressionSyntaxParser::ErrContext>(0);
}


size_t FilterExpressionSyntaxParser::ParseContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleParse;
}

void FilterExpressionSyntaxParser::ParseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParse(this);
}

void FilterExpressionSyntaxParser::ParseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParse(this);
}

FilterExpressionSyntaxParser::ParseContext* FilterExpressionSyntaxParser::parse() {
  ParseContext *_localctx = _tracker.createInstance<ParseContext>(_ctx, getState());
  enterRule(_localctx, 0, FilterExpressionSyntaxParser::RuleParse);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(52);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case FilterExpressionSyntaxParser::T__0:
      case FilterExpressionSyntaxParser::T__2:
      case FilterExpressionSyntaxParser::T__3:
      case FilterExpressionSyntaxParser::T__4:
      case FilterExpressionSyntaxParser::T__6:
      case FilterExpressionSyntaxParser::T__7:
      case FilterExpressionSyntaxParser::K_ABS:
      case FilterExpressionSyntaxParser::K_CEILING:
      case FilterExpressionSyntaxParser::K_COALESCE:
      case FilterExpressionSyntaxParser::K_CONVERT:
      case FilterExpressionSyntaxParser::K_CONTAINS:
      case FilterExpressionSyntaxParser::K_DATEADD:
      case FilterExpressionSyntaxParser::K_DATEDIFF:
      case FilterExpressionSyntaxParser::K_DATEPART:
      case FilterExpressionSyntaxParser::K_ENDSWITH:
      case FilterExpressionSyntaxParser::K_FILTER:
      case FilterExpressionSyntaxParser::K_FLOOR:
      case FilterExpressionSyntaxParser::K_IIF:
      case FilterExpressionSyntaxParser::K_INDEXOF:
      case FilterExpressionSyntaxParser::K_ISDATE:
      case FilterExpressionSyntaxParser::K_ISINTEGER:
      case FilterExpressionSyntaxParser::K_ISGUID:
      case FilterExpressionSyntaxParser::K_ISNULL:
      case FilterExpressionSyntaxParser::K_ISNUMERIC:
      case FilterExpressionSyntaxParser::K_LASTINDEXOF:
      case FilterExpressionSyntaxParser::K_LEN:
      case FilterExpressionSyntaxParser::K_LOWER:
      case FilterExpressionSyntaxParser::K_MAXOF:
      case FilterExpressionSyntaxParser::K_MINOF:
      case FilterExpressionSyntaxParser::K_NOT:
      case FilterExpressionSyntaxParser::K_NOW:
      case FilterExpressionSyntaxParser::K_NTHINDEXOF:
      case FilterExpressionSyntaxParser::K_NULL:
      case FilterExpressionSyntaxParser::K_POWER:
      case FilterExpressionSyntaxParser::K_REGEXMATCH:
      case FilterExpressionSyntaxParser::K_REGEXVAL:
      case FilterExpressionSyntaxParser::K_REPLACE:
      case FilterExpressionSyntaxParser::K_REVERSE:
      case FilterExpressionSyntaxParser::K_ROUND:
      case FilterExpressionSyntaxParser::K_SQRT:
      case FilterExpressionSyntaxParser::K_SPLIT:
      case FilterExpressionSyntaxParser::K_STARTSWITH:
      case FilterExpressionSyntaxParser::K_STRCOUNT:
      case FilterExpressionSyntaxParser::K_STRCMP:
      case FilterExpressionSyntaxParser::K_SUBSTR:
      case FilterExpressionSyntaxParser::K_TRIM:
      case FilterExpressionSyntaxParser::K_TRIMLEFT:
      case FilterExpressionSyntaxParser::K_TRIMRIGHT:
      case FilterExpressionSyntaxParser::K_UPPER:
      case FilterExpressionSyntaxParser::K_UTCNOW:
      case FilterExpressionSyntaxParser::BOOLEAN_LITERAL:
      case FilterExpressionSyntaxParser::IDENTIFIER:
      case FilterExpressionSyntaxParser::INTEGER_LITERAL:
      case FilterExpressionSyntaxParser::NUMERIC_LITERAL:
      case FilterExpressionSyntaxParser::GUID_LITERAL:
      case FilterExpressionSyntaxParser::MEASUREMENT_KEY_LITERAL:
      case FilterExpressionSyntaxParser::POINT_TAG_LITERAL:
      case FilterExpressionSyntaxParser::STRING_LITERAL:
      case FilterExpressionSyntaxParser::DATETIME_LITERAL: {
        setState(50);
        filterExpressionStatementList();
        break;
      }

      case FilterExpressionSyntaxParser::UNEXPECTED_CHAR: {
        setState(51);
        err();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(54);
    match(FilterExpressionSyntaxParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ErrContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::ErrContext::ErrContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::ErrContext::UNEXPECTED_CHAR() {
  return getToken(FilterExpressionSyntaxParser::UNEXPECTED_CHAR, 0);
}


size_t FilterExpressionSyntaxParser::ErrContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleErr;
}

void FilterExpressionSyntaxParser::ErrContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterErr(this);
}

void FilterExpressionSyntaxParser::ErrContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitErr(this);
}

FilterExpressionSyntaxParser::ErrContext* FilterExpressionSyntaxParser::err() {
  ErrContext *_localctx = _tracker.createInstance<ErrContext>(_ctx, getState());
  enterRule(_localctx, 2, FilterExpressionSyntaxParser::RuleErr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(56);
    antlrcpp::downCast<ErrContext *>(_localctx)->unexpected_charToken = match(FilterExpressionSyntaxParser::UNEXPECTED_CHAR);

         throw RuntimeException("Unexpected character: " + (antlrcpp::downCast<ErrContext *>(_localctx)->unexpected_charToken != nullptr ? antlrcpp::downCast<ErrContext *>(_localctx)->unexpected_charToken->getText() : ""));
       
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FilterExpressionStatementListContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::FilterExpressionStatementListContext::FilterExpressionStatementListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<FilterExpressionSyntaxParser::FilterExpressionStatementContext *> FilterExpressionSyntaxParser::FilterExpressionStatementListContext::filterExpressionStatement() {
  return getRuleContexts<FilterExpressionSyntaxParser::FilterExpressionStatementContext>();
}

FilterExpressionSyntaxParser::FilterExpressionStatementContext* FilterExpressionSyntaxParser::FilterExpressionStatementListContext::filterExpressionStatement(size_t i) {
  return getRuleContext<FilterExpressionSyntaxParser::FilterExpressionStatementContext>(i);
}


size_t FilterExpressionSyntaxParser::FilterExpressionStatementListContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleFilterExpressionStatementList;
}

void FilterExpressionSyntaxParser::FilterExpressionStatementListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFilterExpressionStatementList(this);
}

void FilterExpressionSyntaxParser::FilterExpressionStatementListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFilterExpressionStatementList(this);
}

FilterExpressionSyntaxParser::FilterExpressionStatementListContext* FilterExpressionSyntaxParser::filterExpressionStatementList() {
  FilterExpressionStatementListContext *_localctx = _tracker.createInstance<FilterExpressionStatementListContext>(_ctx, getState());
  enterRule(_localctx, 4, FilterExpressionSyntaxParser::RuleFilterExpressionStatementList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(62);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == FilterExpressionSyntaxParser::T__0) {
      setState(59);
      match(FilterExpressionSyntaxParser::T__0);
      setState(64);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(65);
    filterExpressionStatement();
    setState(74);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(67); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(66);
          match(FilterExpressionSyntaxParser::T__0);
          setState(69); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == FilterExpressionSyntaxParser::T__0);
        setState(71);
        filterExpressionStatement(); 
      }
      setState(76);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx);
    }
    setState(80);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == FilterExpressionSyntaxParser::T__0) {
      setState(77);
      match(FilterExpressionSyntaxParser::T__0);
      setState(82);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FilterExpressionStatementContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::FilterExpressionStatementContext::FilterExpressionStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

FilterExpressionSyntaxParser::IdentifierStatementContext* FilterExpressionSyntaxParser::FilterExpressionStatementContext::identifierStatement() {
  return getRuleContext<FilterExpressionSyntaxParser::IdentifierStatementContext>(0);
}

FilterExpressionSyntaxParser::FilterStatementContext* FilterExpressionSyntaxParser::FilterExpressionStatementContext::filterStatement() {
  return getRuleContext<FilterExpressionSyntaxParser::FilterStatementContext>(0);
}

FilterExpressionSyntaxParser::ExpressionContext* FilterExpressionSyntaxParser::FilterExpressionStatementContext::expression() {
  return getRuleContext<FilterExpressionSyntaxParser::ExpressionContext>(0);
}


size_t FilterExpressionSyntaxParser::FilterExpressionStatementContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleFilterExpressionStatement;
}

void FilterExpressionSyntaxParser::FilterExpressionStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFilterExpressionStatement(this);
}

void FilterExpressionSyntaxParser::FilterExpressionStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFilterExpressionStatement(this);
}

FilterExpressionSyntaxParser::FilterExpressionStatementContext* FilterExpressionSyntaxParser::filterExpressionStatement() {
  FilterExpressionStatementContext *_localctx = _tracker.createInstance<FilterExpressionStatementContext>(_ctx, getState());
  enterRule(_localctx, 6, FilterExpressionSyntaxParser::RuleFilterExpressionStatement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(86);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(83);
      identifierStatement();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(84);
      filterStatement();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(85);
      expression(0);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdentifierStatementContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::IdentifierStatementContext::IdentifierStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::IdentifierStatementContext::GUID_LITERAL() {
  return getToken(FilterExpressionSyntaxParser::GUID_LITERAL, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::IdentifierStatementContext::MEASUREMENT_KEY_LITERAL() {
  return getToken(FilterExpressionSyntaxParser::MEASUREMENT_KEY_LITERAL, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::IdentifierStatementContext::POINT_TAG_LITERAL() {
  return getToken(FilterExpressionSyntaxParser::POINT_TAG_LITERAL, 0);
}


size_t FilterExpressionSyntaxParser::IdentifierStatementContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleIdentifierStatement;
}

void FilterExpressionSyntaxParser::IdentifierStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIdentifierStatement(this);
}

void FilterExpressionSyntaxParser::IdentifierStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIdentifierStatement(this);
}

FilterExpressionSyntaxParser::IdentifierStatementContext* FilterExpressionSyntaxParser::identifierStatement() {
  IdentifierStatementContext *_localctx = _tracker.createInstance<IdentifierStatementContext>(_ctx, getState());
  enterRule(_localctx, 8, FilterExpressionSyntaxParser::RuleIdentifierStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(88);
    _la = _input->LA(1);
    if (!((((_la - 90) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 90)) & 7) != 0)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FilterStatementContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::FilterStatementContext::FilterStatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::FilterStatementContext::K_FILTER() {
  return getToken(FilterExpressionSyntaxParser::K_FILTER, 0);
}

FilterExpressionSyntaxParser::TableNameContext* FilterExpressionSyntaxParser::FilterStatementContext::tableName() {
  return getRuleContext<FilterExpressionSyntaxParser::TableNameContext>(0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FilterStatementContext::K_WHERE() {
  return getToken(FilterExpressionSyntaxParser::K_WHERE, 0);
}

FilterExpressionSyntaxParser::ExpressionContext* FilterExpressionSyntaxParser::FilterStatementContext::expression() {
  return getRuleContext<FilterExpressionSyntaxParser::ExpressionContext>(0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FilterStatementContext::K_TOP() {
  return getToken(FilterExpressionSyntaxParser::K_TOP, 0);
}

FilterExpressionSyntaxParser::TopLimitContext* FilterExpressionSyntaxParser::FilterStatementContext::topLimit() {
  return getRuleContext<FilterExpressionSyntaxParser::TopLimitContext>(0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FilterStatementContext::K_ORDER() {
  return getToken(FilterExpressionSyntaxParser::K_ORDER, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FilterStatementContext::K_BY() {
  return getToken(FilterExpressionSyntaxParser::K_BY, 0);
}

std::vector<FilterExpressionSyntaxParser::OrderingTermContext *> FilterExpressionSyntaxParser::FilterStatementContext::orderingTerm() {
  return getRuleContexts<FilterExpressionSyntaxParser::OrderingTermContext>();
}

FilterExpressionSyntaxParser::OrderingTermContext* FilterExpressionSyntaxParser::FilterStatementContext::orderingTerm(size_t i) {
  return getRuleContext<FilterExpressionSyntaxParser::OrderingTermContext>(i);
}


size_t FilterExpressionSyntaxParser::FilterStatementContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleFilterStatement;
}

void FilterExpressionSyntaxParser::FilterStatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFilterStatement(this);
}

void FilterExpressionSyntaxParser::FilterStatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFilterStatement(this);
}

FilterExpressionSyntaxParser::FilterStatementContext* FilterExpressionSyntaxParser::filterStatement() {
  FilterStatementContext *_localctx = _tracker.createInstance<FilterStatementContext>(_ctx, getState());
  enterRule(_localctx, 10, FilterExpressionSyntaxParser::RuleFilterStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(90);
    match(FilterExpressionSyntaxParser::K_FILTER);
    setState(93);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == FilterExpressionSyntaxParser::K_TOP) {
      setState(91);
      match(FilterExpressionSyntaxParser::K_TOP);
      setState(92);
      topLimit();
    }
    setState(95);
    tableName();
    setState(96);
    match(FilterExpressionSyntaxParser::K_WHERE);
    setState(97);
    expression(0);
    setState(108);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == FilterExpressionSyntaxParser::K_ORDER) {
      setState(98);
      match(FilterExpressionSyntaxParser::K_ORDER);
      setState(99);
      match(FilterExpressionSyntaxParser::K_BY);
      setState(100);
      orderingTerm();
      setState(105);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == FilterExpressionSyntaxParser::T__1) {
        setState(101);
        match(FilterExpressionSyntaxParser::T__1);
        setState(102);
        orderingTerm();
        setState(107);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TopLimitContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::TopLimitContext::TopLimitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::TopLimitContext::INTEGER_LITERAL() {
  return getToken(FilterExpressionSyntaxParser::INTEGER_LITERAL, 0);
}


size_t FilterExpressionSyntaxParser::TopLimitContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleTopLimit;
}

void FilterExpressionSyntaxParser::TopLimitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTopLimit(this);
}

void FilterExpressionSyntaxParser::TopLimitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTopLimit(this);
}

FilterExpressionSyntaxParser::TopLimitContext* FilterExpressionSyntaxParser::topLimit() {
  TopLimitContext *_localctx = _tracker.createInstance<TopLimitContext>(_ctx, getState());
  enterRule(_localctx, 12, FilterExpressionSyntaxParser::RuleTopLimit);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(111);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == FilterExpressionSyntaxParser::T__2

    || _la == FilterExpressionSyntaxParser::T__3) {
      setState(110);
      _la = _input->LA(1);
      if (!(_la == FilterExpressionSyntaxParser::T__2

      || _la == FilterExpressionSyntaxParser::T__3)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(113);
    match(FilterExpressionSyntaxParser::INTEGER_LITERAL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OrderingTermContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::OrderingTermContext::OrderingTermContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

FilterExpressionSyntaxParser::OrderByColumnNameContext* FilterExpressionSyntaxParser::OrderingTermContext::orderByColumnName() {
  return getRuleContext<FilterExpressionSyntaxParser::OrderByColumnNameContext>(0);
}

FilterExpressionSyntaxParser::ExactMatchModifierContext* FilterExpressionSyntaxParser::OrderingTermContext::exactMatchModifier() {
  return getRuleContext<FilterExpressionSyntaxParser::ExactMatchModifierContext>(0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::OrderingTermContext::K_ASC() {
  return getToken(FilterExpressionSyntaxParser::K_ASC, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::OrderingTermContext::K_DESC() {
  return getToken(FilterExpressionSyntaxParser::K_DESC, 0);
}


size_t FilterExpressionSyntaxParser::OrderingTermContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleOrderingTerm;
}

void FilterExpressionSyntaxParser::OrderingTermContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOrderingTerm(this);
}

void FilterExpressionSyntaxParser::OrderingTermContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOrderingTerm(this);
}

FilterExpressionSyntaxParser::OrderingTermContext* FilterExpressionSyntaxParser::orderingTerm() {
  OrderingTermContext *_localctx = _tracker.createInstance<OrderingTermContext>(_ctx, getState());
  enterRule(_localctx, 14, FilterExpressionSyntaxParser::RuleOrderingTerm);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(116);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == FilterExpressionSyntaxParser::T__8

    || _la == FilterExpressionSyntaxParser::K_BINARY) {
      setState(115);
      exactMatchModifier();
    }
    setState(118);
    orderByColumnName();
    setState(120);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == FilterExpressionSyntaxParser::K_ASC

    || _la == FilterExpressionSyntaxParser::K_DESC) {
      setState(119);
      _la = _input->LA(1);
      if (!(_la == FilterExpressionSyntaxParser::K_ASC

      || _la == FilterExpressionSyntaxParser::K_DESC)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionListContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::ExpressionListContext::ExpressionListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<FilterExpressionSyntaxParser::ExpressionContext *> FilterExpressionSyntaxParser::ExpressionListContext::expression() {
  return getRuleContexts<FilterExpressionSyntaxParser::ExpressionContext>();
}

FilterExpressionSyntaxParser::ExpressionContext* FilterExpressionSyntaxParser::ExpressionListContext::expression(size_t i) {
  return getRuleContext<FilterExpressionSyntaxParser::ExpressionContext>(i);
}


size_t FilterExpressionSyntaxParser::ExpressionListContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleExpressionList;
}

void FilterExpressionSyntaxParser::ExpressionListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpressionList(this);
}

void FilterExpressionSyntaxParser::ExpressionListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpressionList(this);
}

FilterExpressionSyntaxParser::ExpressionListContext* FilterExpressionSyntaxParser::expressionList() {
  ExpressionListContext *_localctx = _tracker.createInstance<ExpressionListContext>(_ctx, getState());
  enterRule(_localctx, 16, FilterExpressionSyntaxParser::RuleExpressionList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(122);
    expression(0);
    setState(127);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == FilterExpressionSyntaxParser::T__1) {
      setState(123);
      match(FilterExpressionSyntaxParser::T__1);
      setState(124);
      expression(0);
      setState(129);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

FilterExpressionSyntaxParser::NotOperatorContext* FilterExpressionSyntaxParser::ExpressionContext::notOperator() {
  return getRuleContext<FilterExpressionSyntaxParser::NotOperatorContext>(0);
}

std::vector<FilterExpressionSyntaxParser::ExpressionContext *> FilterExpressionSyntaxParser::ExpressionContext::expression() {
  return getRuleContexts<FilterExpressionSyntaxParser::ExpressionContext>();
}

FilterExpressionSyntaxParser::ExpressionContext* FilterExpressionSyntaxParser::ExpressionContext::expression(size_t i) {
  return getRuleContext<FilterExpressionSyntaxParser::ExpressionContext>(i);
}

FilterExpressionSyntaxParser::PredicateExpressionContext* FilterExpressionSyntaxParser::ExpressionContext::predicateExpression() {
  return getRuleContext<FilterExpressionSyntaxParser::PredicateExpressionContext>(0);
}

FilterExpressionSyntaxParser::LogicalOperatorContext* FilterExpressionSyntaxParser::ExpressionContext::logicalOperator() {
  return getRuleContext<FilterExpressionSyntaxParser::LogicalOperatorContext>(0);
}


size_t FilterExpressionSyntaxParser::ExpressionContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleExpression;
}

void FilterExpressionSyntaxParser::ExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpression(this);
}

void FilterExpressionSyntaxParser::ExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpression(this);
}


FilterExpressionSyntaxParser::ExpressionContext* FilterExpressionSyntaxParser::expression() {
   return expression(0);
}

FilterExpressionSyntaxParser::ExpressionContext* FilterExpressionSyntaxParser::expression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  FilterExpressionSyntaxParser::ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, parentState);
  FilterExpressionSyntaxParser::ExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 18;
  enterRecursionRule(_localctx, 18, FilterExpressionSyntaxParser::RuleExpression, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(135);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx)) {
    case 1: {
      setState(131);
      notOperator();
      setState(132);
      expression(3);
      break;
    }

    case 2: {
      setState(134);
      predicateExpression(0);
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(143);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression);
        setState(137);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(138);
        logicalOperator();
        setState(139);
        expression(3); 
      }
      setState(145);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- PredicateExpressionContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::PredicateExpressionContext::PredicateExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

FilterExpressionSyntaxParser::ValueExpressionContext* FilterExpressionSyntaxParser::PredicateExpressionContext::valueExpression() {
  return getRuleContext<FilterExpressionSyntaxParser::ValueExpressionContext>(0);
}

std::vector<FilterExpressionSyntaxParser::PredicateExpressionContext *> FilterExpressionSyntaxParser::PredicateExpressionContext::predicateExpression() {
  return getRuleContexts<FilterExpressionSyntaxParser::PredicateExpressionContext>();
}

FilterExpressionSyntaxParser::PredicateExpressionContext* FilterExpressionSyntaxParser::PredicateExpressionContext::predicateExpression(size_t i) {
  return getRuleContext<FilterExpressionSyntaxParser::PredicateExpressionContext>(i);
}

FilterExpressionSyntaxParser::ComparisonOperatorContext* FilterExpressionSyntaxParser::PredicateExpressionContext::comparisonOperator() {
  return getRuleContext<FilterExpressionSyntaxParser::ComparisonOperatorContext>(0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::PredicateExpressionContext::K_LIKE() {
  return getToken(FilterExpressionSyntaxParser::K_LIKE, 0);
}

FilterExpressionSyntaxParser::NotOperatorContext* FilterExpressionSyntaxParser::PredicateExpressionContext::notOperator() {
  return getRuleContext<FilterExpressionSyntaxParser::NotOperatorContext>(0);
}

FilterExpressionSyntaxParser::ExactMatchModifierContext* FilterExpressionSyntaxParser::PredicateExpressionContext::exactMatchModifier() {
  return getRuleContext<FilterExpressionSyntaxParser::ExactMatchModifierContext>(0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::PredicateExpressionContext::K_IN() {
  return getToken(FilterExpressionSyntaxParser::K_IN, 0);
}

FilterExpressionSyntaxParser::ExpressionListContext* FilterExpressionSyntaxParser::PredicateExpressionContext::expressionList() {
  return getRuleContext<FilterExpressionSyntaxParser::ExpressionListContext>(0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::PredicateExpressionContext::K_IS() {
  return getToken(FilterExpressionSyntaxParser::K_IS, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::PredicateExpressionContext::K_NULL() {
  return getToken(FilterExpressionSyntaxParser::K_NULL, 0);
}


size_t FilterExpressionSyntaxParser::PredicateExpressionContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RulePredicateExpression;
}

void FilterExpressionSyntaxParser::PredicateExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPredicateExpression(this);
}

void FilterExpressionSyntaxParser::PredicateExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPredicateExpression(this);
}


FilterExpressionSyntaxParser::PredicateExpressionContext* FilterExpressionSyntaxParser::predicateExpression() {
   return predicateExpression(0);
}

FilterExpressionSyntaxParser::PredicateExpressionContext* FilterExpressionSyntaxParser::predicateExpression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  FilterExpressionSyntaxParser::PredicateExpressionContext *_localctx = _tracker.createInstance<PredicateExpressionContext>(_ctx, parentState);
  FilterExpressionSyntaxParser::PredicateExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 20;
  enterRecursionRule(_localctx, 20, FilterExpressionSyntaxParser::RulePredicateExpression, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(147);
    valueExpression(0);
    _ctx->stop = _input->LT(-1);
    setState(182);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(180);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<PredicateExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RulePredicateExpression);
          setState(149);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(150);
          comparisonOperator();
          setState(151);
          predicateExpression(4);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<PredicateExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RulePredicateExpression);
          setState(153);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(155);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == FilterExpressionSyntaxParser::T__6

          || _la == FilterExpressionSyntaxParser::K_NOT) {
            setState(154);
            notOperator();
          }
          setState(157);
          match(FilterExpressionSyntaxParser::K_LIKE);
          setState(159);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == FilterExpressionSyntaxParser::T__8

          || _la == FilterExpressionSyntaxParser::K_BINARY) {
            setState(158);
            exactMatchModifier();
          }
          setState(161);
          predicateExpression(3);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<PredicateExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RulePredicateExpression);
          setState(162);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(164);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == FilterExpressionSyntaxParser::T__6

          || _la == FilterExpressionSyntaxParser::K_NOT) {
            setState(163);
            notOperator();
          }
          setState(166);
          match(FilterExpressionSyntaxParser::K_IN);
          setState(168);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == FilterExpressionSyntaxParser::T__8

          || _la == FilterExpressionSyntaxParser::K_BINARY) {
            setState(167);
            exactMatchModifier();
          }
          setState(170);
          match(FilterExpressionSyntaxParser::T__4);
          setState(171);
          expressionList();
          setState(172);
          match(FilterExpressionSyntaxParser::T__5);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<PredicateExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RulePredicateExpression);
          setState(174);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(175);
          match(FilterExpressionSyntaxParser::K_IS);
          setState(177);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if (_la == FilterExpressionSyntaxParser::T__6

          || _la == FilterExpressionSyntaxParser::K_NOT) {
            setState(176);
            notOperator();
          }
          setState(179);
          match(FilterExpressionSyntaxParser::K_NULL);
          break;
        }

        default:
          break;
        } 
      }
      setState(184);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- ValueExpressionContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::ValueExpressionContext::ValueExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

FilterExpressionSyntaxParser::LiteralValueContext* FilterExpressionSyntaxParser::ValueExpressionContext::literalValue() {
  return getRuleContext<FilterExpressionSyntaxParser::LiteralValueContext>(0);
}

FilterExpressionSyntaxParser::ColumnNameContext* FilterExpressionSyntaxParser::ValueExpressionContext::columnName() {
  return getRuleContext<FilterExpressionSyntaxParser::ColumnNameContext>(0);
}

FilterExpressionSyntaxParser::FunctionExpressionContext* FilterExpressionSyntaxParser::ValueExpressionContext::functionExpression() {
  return getRuleContext<FilterExpressionSyntaxParser::FunctionExpressionContext>(0);
}

FilterExpressionSyntaxParser::UnaryOperatorContext* FilterExpressionSyntaxParser::ValueExpressionContext::unaryOperator() {
  return getRuleContext<FilterExpressionSyntaxParser::UnaryOperatorContext>(0);
}

std::vector<FilterExpressionSyntaxParser::ValueExpressionContext *> FilterExpressionSyntaxParser::ValueExpressionContext::valueExpression() {
  return getRuleContexts<FilterExpressionSyntaxParser::ValueExpressionContext>();
}

FilterExpressionSyntaxParser::ValueExpressionContext* FilterExpressionSyntaxParser::ValueExpressionContext::valueExpression(size_t i) {
  return getRuleContext<FilterExpressionSyntaxParser::ValueExpressionContext>(i);
}

FilterExpressionSyntaxParser::ExpressionContext* FilterExpressionSyntaxParser::ValueExpressionContext::expression() {
  return getRuleContext<FilterExpressionSyntaxParser::ExpressionContext>(0);
}

FilterExpressionSyntaxParser::MathOperatorContext* FilterExpressionSyntaxParser::ValueExpressionContext::mathOperator() {
  return getRuleContext<FilterExpressionSyntaxParser::MathOperatorContext>(0);
}

FilterExpressionSyntaxParser::BitwiseOperatorContext* FilterExpressionSyntaxParser::ValueExpressionContext::bitwiseOperator() {
  return getRuleContext<FilterExpressionSyntaxParser::BitwiseOperatorContext>(0);
}


size_t FilterExpressionSyntaxParser::ValueExpressionContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleValueExpression;
}

void FilterExpressionSyntaxParser::ValueExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValueExpression(this);
}

void FilterExpressionSyntaxParser::ValueExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValueExpression(this);
}


FilterExpressionSyntaxParser::ValueExpressionContext* FilterExpressionSyntaxParser::valueExpression() {
   return valueExpression(0);
}

FilterExpressionSyntaxParser::ValueExpressionContext* FilterExpressionSyntaxParser::valueExpression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  FilterExpressionSyntaxParser::ValueExpressionContext *_localctx = _tracker.createInstance<ValueExpressionContext>(_ctx, parentState);
  FilterExpressionSyntaxParser::ValueExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 22;
  enterRecursionRule(_localctx, 22, FilterExpressionSyntaxParser::RuleValueExpression, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(196);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case FilterExpressionSyntaxParser::K_NULL:
      case FilterExpressionSyntaxParser::BOOLEAN_LITERAL:
      case FilterExpressionSyntaxParser::INTEGER_LITERAL:
      case FilterExpressionSyntaxParser::NUMERIC_LITERAL:
      case FilterExpressionSyntaxParser::GUID_LITERAL:
      case FilterExpressionSyntaxParser::STRING_LITERAL:
      case FilterExpressionSyntaxParser::DATETIME_LITERAL: {
        setState(186);
        literalValue();
        break;
      }

      case FilterExpressionSyntaxParser::IDENTIFIER: {
        setState(187);
        columnName();
        break;
      }

      case FilterExpressionSyntaxParser::K_ABS:
      case FilterExpressionSyntaxParser::K_CEILING:
      case FilterExpressionSyntaxParser::K_COALESCE:
      case FilterExpressionSyntaxParser::K_CONVERT:
      case FilterExpressionSyntaxParser::K_CONTAINS:
      case FilterExpressionSyntaxParser::K_DATEADD:
      case FilterExpressionSyntaxParser::K_DATEDIFF:
      case FilterExpressionSyntaxParser::K_DATEPART:
      case FilterExpressionSyntaxParser::K_ENDSWITH:
      case FilterExpressionSyntaxParser::K_FLOOR:
      case FilterExpressionSyntaxParser::K_IIF:
      case FilterExpressionSyntaxParser::K_INDEXOF:
      case FilterExpressionSyntaxParser::K_ISDATE:
      case FilterExpressionSyntaxParser::K_ISINTEGER:
      case FilterExpressionSyntaxParser::K_ISGUID:
      case FilterExpressionSyntaxParser::K_ISNULL:
      case FilterExpressionSyntaxParser::K_ISNUMERIC:
      case FilterExpressionSyntaxParser::K_LASTINDEXOF:
      case FilterExpressionSyntaxParser::K_LEN:
      case FilterExpressionSyntaxParser::K_LOWER:
      case FilterExpressionSyntaxParser::K_MAXOF:
      case FilterExpressionSyntaxParser::K_MINOF:
      case FilterExpressionSyntaxParser::K_NOW:
      case FilterExpressionSyntaxParser::K_NTHINDEXOF:
      case FilterExpressionSyntaxParser::K_POWER:
      case FilterExpressionSyntaxParser::K_REGEXMATCH:
      case FilterExpressionSyntaxParser::K_REGEXVAL:
      case FilterExpressionSyntaxParser::K_REPLACE:
      case FilterExpressionSyntaxParser::K_REVERSE:
      case FilterExpressionSyntaxParser::K_ROUND:
      case FilterExpressionSyntaxParser::K_SQRT:
      case FilterExpressionSyntaxParser::K_SPLIT:
      case FilterExpressionSyntaxParser::K_STARTSWITH:
      case FilterExpressionSyntaxParser::K_STRCOUNT:
      case FilterExpressionSyntaxParser::K_STRCMP:
      case FilterExpressionSyntaxParser::K_SUBSTR:
      case FilterExpressionSyntaxParser::K_TRIM:
      case FilterExpressionSyntaxParser::K_TRIMLEFT:
      case FilterExpressionSyntaxParser::K_TRIMRIGHT:
      case FilterExpressionSyntaxParser::K_UPPER:
      case FilterExpressionSyntaxParser::K_UTCNOW: {
        setState(188);
        functionExpression();
        break;
      }

      case FilterExpressionSyntaxParser::T__2:
      case FilterExpressionSyntaxParser::T__3:
      case FilterExpressionSyntaxParser::T__6:
      case FilterExpressionSyntaxParser::T__7:
      case FilterExpressionSyntaxParser::K_NOT: {
        setState(189);
        unaryOperator();
        setState(190);
        valueExpression(4);
        break;
      }

      case FilterExpressionSyntaxParser::T__4: {
        setState(192);
        match(FilterExpressionSyntaxParser::T__4);
        setState(193);
        expression(0);
        setState(194);
        match(FilterExpressionSyntaxParser::T__5);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(208);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(206);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ValueExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleValueExpression);
          setState(198);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(199);
          mathOperator();
          setState(200);
          valueExpression(3);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ValueExpressionContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleValueExpression);
          setState(202);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(203);
          bitwiseOperator();
          setState(204);
          valueExpression(2);
          break;
        }

        default:
          break;
        } 
      }
      setState(210);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- NotOperatorContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::NotOperatorContext::NotOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::NotOperatorContext::K_NOT() {
  return getToken(FilterExpressionSyntaxParser::K_NOT, 0);
}


size_t FilterExpressionSyntaxParser::NotOperatorContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleNotOperator;
}

void FilterExpressionSyntaxParser::NotOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNotOperator(this);
}

void FilterExpressionSyntaxParser::NotOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNotOperator(this);
}

FilterExpressionSyntaxParser::NotOperatorContext* FilterExpressionSyntaxParser::notOperator() {
  NotOperatorContext *_localctx = _tracker.createInstance<NotOperatorContext>(_ctx, getState());
  enterRule(_localctx, 24, FilterExpressionSyntaxParser::RuleNotOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(211);
    _la = _input->LA(1);
    if (!(_la == FilterExpressionSyntaxParser::T__6

    || _la == FilterExpressionSyntaxParser::K_NOT)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryOperatorContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::UnaryOperatorContext::UnaryOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::UnaryOperatorContext::K_NOT() {
  return getToken(FilterExpressionSyntaxParser::K_NOT, 0);
}


size_t FilterExpressionSyntaxParser::UnaryOperatorContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleUnaryOperator;
}

void FilterExpressionSyntaxParser::UnaryOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryOperator(this);
}

void FilterExpressionSyntaxParser::UnaryOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryOperator(this);
}

FilterExpressionSyntaxParser::UnaryOperatorContext* FilterExpressionSyntaxParser::unaryOperator() {
  UnaryOperatorContext *_localctx = _tracker.createInstance<UnaryOperatorContext>(_ctx, getState());
  enterRule(_localctx, 26, FilterExpressionSyntaxParser::RuleUnaryOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(213);
    _la = _input->LA(1);
    if (!(((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 1152921504606847384) != 0)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExactMatchModifierContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::ExactMatchModifierContext::ExactMatchModifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::ExactMatchModifierContext::K_BINARY() {
  return getToken(FilterExpressionSyntaxParser::K_BINARY, 0);
}


size_t FilterExpressionSyntaxParser::ExactMatchModifierContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleExactMatchModifier;
}

void FilterExpressionSyntaxParser::ExactMatchModifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExactMatchModifier(this);
}

void FilterExpressionSyntaxParser::ExactMatchModifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExactMatchModifier(this);
}

FilterExpressionSyntaxParser::ExactMatchModifierContext* FilterExpressionSyntaxParser::exactMatchModifier() {
  ExactMatchModifierContext *_localctx = _tracker.createInstance<ExactMatchModifierContext>(_ctx, getState());
  enterRule(_localctx, 28, FilterExpressionSyntaxParser::RuleExactMatchModifier);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(215);
    _la = _input->LA(1);
    if (!(_la == FilterExpressionSyntaxParser::T__8

    || _la == FilterExpressionSyntaxParser::K_BINARY)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ComparisonOperatorContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::ComparisonOperatorContext::ComparisonOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t FilterExpressionSyntaxParser::ComparisonOperatorContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleComparisonOperator;
}

void FilterExpressionSyntaxParser::ComparisonOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComparisonOperator(this);
}

void FilterExpressionSyntaxParser::ComparisonOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComparisonOperator(this);
}

FilterExpressionSyntaxParser::ComparisonOperatorContext* FilterExpressionSyntaxParser::comparisonOperator() {
  ComparisonOperatorContext *_localctx = _tracker.createInstance<ComparisonOperatorContext>(_ctx, getState());
  enterRule(_localctx, 30, FilterExpressionSyntaxParser::RuleComparisonOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(217);
    _la = _input->LA(1);
    if (!(((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 523776) != 0)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LogicalOperatorContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::LogicalOperatorContext::LogicalOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::LogicalOperatorContext::K_AND() {
  return getToken(FilterExpressionSyntaxParser::K_AND, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::LogicalOperatorContext::K_OR() {
  return getToken(FilterExpressionSyntaxParser::K_OR, 0);
}


size_t FilterExpressionSyntaxParser::LogicalOperatorContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleLogicalOperator;
}

void FilterExpressionSyntaxParser::LogicalOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalOperator(this);
}

void FilterExpressionSyntaxParser::LogicalOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalOperator(this);
}

FilterExpressionSyntaxParser::LogicalOperatorContext* FilterExpressionSyntaxParser::logicalOperator() {
  LogicalOperatorContext *_localctx = _tracker.createInstance<LogicalOperatorContext>(_ctx, getState());
  enterRule(_localctx, 32, FilterExpressionSyntaxParser::RuleLogicalOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(219);
    _la = _input->LA(1);
    if (!((((_la - 19) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 19)) & 35184372090883) != 0)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BitwiseOperatorContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::BitwiseOperatorContext::BitwiseOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::BitwiseOperatorContext::K_XOR() {
  return getToken(FilterExpressionSyntaxParser::K_XOR, 0);
}


size_t FilterExpressionSyntaxParser::BitwiseOperatorContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleBitwiseOperator;
}

void FilterExpressionSyntaxParser::BitwiseOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitwiseOperator(this);
}

void FilterExpressionSyntaxParser::BitwiseOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitwiseOperator(this);
}

FilterExpressionSyntaxParser::BitwiseOperatorContext* FilterExpressionSyntaxParser::bitwiseOperator() {
  BitwiseOperatorContext *_localctx = _tracker.createInstance<BitwiseOperatorContext>(_ctx, getState());
  enterRule(_localctx, 34, FilterExpressionSyntaxParser::RuleBitwiseOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(221);
    _la = _input->LA(1);
    if (!(((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 65011712) != 0 || _la == FilterExpressionSyntaxParser::K_XOR)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MathOperatorContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::MathOperatorContext::MathOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t FilterExpressionSyntaxParser::MathOperatorContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleMathOperator;
}

void FilterExpressionSyntaxParser::MathOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMathOperator(this);
}

void FilterExpressionSyntaxParser::MathOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMathOperator(this);
}

FilterExpressionSyntaxParser::MathOperatorContext* FilterExpressionSyntaxParser::mathOperator() {
  MathOperatorContext *_localctx = _tracker.createInstance<MathOperatorContext>(_ctx, getState());
  enterRule(_localctx, 36, FilterExpressionSyntaxParser::RuleMathOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(223);
    _la = _input->LA(1);
    if (!(((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 469762072) != 0)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionNameContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::FunctionNameContext::FunctionNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_ABS() {
  return getToken(FilterExpressionSyntaxParser::K_ABS, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_CEILING() {
  return getToken(FilterExpressionSyntaxParser::K_CEILING, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_COALESCE() {
  return getToken(FilterExpressionSyntaxParser::K_COALESCE, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_CONVERT() {
  return getToken(FilterExpressionSyntaxParser::K_CONVERT, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_CONTAINS() {
  return getToken(FilterExpressionSyntaxParser::K_CONTAINS, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_DATEADD() {
  return getToken(FilterExpressionSyntaxParser::K_DATEADD, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_DATEDIFF() {
  return getToken(FilterExpressionSyntaxParser::K_DATEDIFF, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_DATEPART() {
  return getToken(FilterExpressionSyntaxParser::K_DATEPART, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_ENDSWITH() {
  return getToken(FilterExpressionSyntaxParser::K_ENDSWITH, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_FLOOR() {
  return getToken(FilterExpressionSyntaxParser::K_FLOOR, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_IIF() {
  return getToken(FilterExpressionSyntaxParser::K_IIF, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_INDEXOF() {
  return getToken(FilterExpressionSyntaxParser::K_INDEXOF, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_ISDATE() {
  return getToken(FilterExpressionSyntaxParser::K_ISDATE, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_ISINTEGER() {
  return getToken(FilterExpressionSyntaxParser::K_ISINTEGER, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_ISGUID() {
  return getToken(FilterExpressionSyntaxParser::K_ISGUID, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_ISNULL() {
  return getToken(FilterExpressionSyntaxParser::K_ISNULL, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_ISNUMERIC() {
  return getToken(FilterExpressionSyntaxParser::K_ISNUMERIC, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_LASTINDEXOF() {
  return getToken(FilterExpressionSyntaxParser::K_LASTINDEXOF, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_LEN() {
  return getToken(FilterExpressionSyntaxParser::K_LEN, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_LOWER() {
  return getToken(FilterExpressionSyntaxParser::K_LOWER, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_MAXOF() {
  return getToken(FilterExpressionSyntaxParser::K_MAXOF, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_MINOF() {
  return getToken(FilterExpressionSyntaxParser::K_MINOF, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_NOW() {
  return getToken(FilterExpressionSyntaxParser::K_NOW, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_NTHINDEXOF() {
  return getToken(FilterExpressionSyntaxParser::K_NTHINDEXOF, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_POWER() {
  return getToken(FilterExpressionSyntaxParser::K_POWER, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_REGEXMATCH() {
  return getToken(FilterExpressionSyntaxParser::K_REGEXMATCH, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_REGEXVAL() {
  return getToken(FilterExpressionSyntaxParser::K_REGEXVAL, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_REPLACE() {
  return getToken(FilterExpressionSyntaxParser::K_REPLACE, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_REVERSE() {
  return getToken(FilterExpressionSyntaxParser::K_REVERSE, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_ROUND() {
  return getToken(FilterExpressionSyntaxParser::K_ROUND, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_SPLIT() {
  return getToken(FilterExpressionSyntaxParser::K_SPLIT, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_SQRT() {
  return getToken(FilterExpressionSyntaxParser::K_SQRT, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_STARTSWITH() {
  return getToken(FilterExpressionSyntaxParser::K_STARTSWITH, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_STRCOUNT() {
  return getToken(FilterExpressionSyntaxParser::K_STRCOUNT, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_STRCMP() {
  return getToken(FilterExpressionSyntaxParser::K_STRCMP, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_SUBSTR() {
  return getToken(FilterExpressionSyntaxParser::K_SUBSTR, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_TRIM() {
  return getToken(FilterExpressionSyntaxParser::K_TRIM, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_TRIMLEFT() {
  return getToken(FilterExpressionSyntaxParser::K_TRIMLEFT, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_TRIMRIGHT() {
  return getToken(FilterExpressionSyntaxParser::K_TRIMRIGHT, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_UPPER() {
  return getToken(FilterExpressionSyntaxParser::K_UPPER, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::FunctionNameContext::K_UTCNOW() {
  return getToken(FilterExpressionSyntaxParser::K_UTCNOW, 0);
}


size_t FilterExpressionSyntaxParser::FunctionNameContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleFunctionName;
}

void FilterExpressionSyntaxParser::FunctionNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionName(this);
}

void FilterExpressionSyntaxParser::FunctionNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionName(this);
}

FilterExpressionSyntaxParser::FunctionNameContext* FilterExpressionSyntaxParser::functionName() {
  FunctionNameContext *_localctx = _tracker.createInstance<FunctionNameContext>(_ctx, getState());
  enterRule(_localctx, 38, FilterExpressionSyntaxParser::RuleFunctionName);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(225);
    _la = _input->LA(1);
    if (!((((_la - 29) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 29)) & 35465724524081121) != 0)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionExpressionContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::FunctionExpressionContext::FunctionExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

FilterExpressionSyntaxParser::FunctionNameContext* FilterExpressionSyntaxParser::FunctionExpressionContext::functionName() {
  return getRuleContext<FilterExpressionSyntaxParser::FunctionNameContext>(0);
}

FilterExpressionSyntaxParser::ExpressionListContext* FilterExpressionSyntaxParser::FunctionExpressionContext::expressionList() {
  return getRuleContext<FilterExpressionSyntaxParser::ExpressionListContext>(0);
}


size_t FilterExpressionSyntaxParser::FunctionExpressionContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleFunctionExpression;
}

void FilterExpressionSyntaxParser::FunctionExpressionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionExpression(this);
}

void FilterExpressionSyntaxParser::FunctionExpressionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionExpression(this);
}

FilterExpressionSyntaxParser::FunctionExpressionContext* FilterExpressionSyntaxParser::functionExpression() {
  FunctionExpressionContext *_localctx = _tracker.createInstance<FunctionExpressionContext>(_ctx, getState());
  enterRule(_localctx, 40, FilterExpressionSyntaxParser::RuleFunctionExpression);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(227);
    functionName();
    setState(228);
    match(FilterExpressionSyntaxParser::T__4);
    setState(230);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & -72420449518091848) != 0 || (((_la - 66) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 66)) & 435417087) != 0) {
      setState(229);
      expressionList();
    }
    setState(232);
    match(FilterExpressionSyntaxParser::T__5);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LiteralValueContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::LiteralValueContext::LiteralValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::LiteralValueContext::INTEGER_LITERAL() {
  return getToken(FilterExpressionSyntaxParser::INTEGER_LITERAL, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::LiteralValueContext::NUMERIC_LITERAL() {
  return getToken(FilterExpressionSyntaxParser::NUMERIC_LITERAL, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::LiteralValueContext::STRING_LITERAL() {
  return getToken(FilterExpressionSyntaxParser::STRING_LITERAL, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::LiteralValueContext::DATETIME_LITERAL() {
  return getToken(FilterExpressionSyntaxParser::DATETIME_LITERAL, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::LiteralValueContext::GUID_LITERAL() {
  return getToken(FilterExpressionSyntaxParser::GUID_LITERAL, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::LiteralValueContext::BOOLEAN_LITERAL() {
  return getToken(FilterExpressionSyntaxParser::BOOLEAN_LITERAL, 0);
}

tree::TerminalNode* FilterExpressionSyntaxParser::LiteralValueContext::K_NULL() {
  return getToken(FilterExpressionSyntaxParser::K_NULL, 0);
}


size_t FilterExpressionSyntaxParser::LiteralValueContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleLiteralValue;
}

void FilterExpressionSyntaxParser::LiteralValueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLiteralValue(this);
}

void FilterExpressionSyntaxParser::LiteralValueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLiteralValue(this);
}

FilterExpressionSyntaxParser::LiteralValueContext* FilterExpressionSyntaxParser::literalValue() {
  LiteralValueContext *_localctx = _tracker.createInstance<LiteralValueContext>(_ctx, getState());
  enterRule(_localctx, 42, FilterExpressionSyntaxParser::RuleLiteralValue);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(234);
    _la = _input->LA(1);
    if (!((((_la - 63) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 63)) & 3464495105) != 0)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TableNameContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::TableNameContext::TableNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::TableNameContext::IDENTIFIER() {
  return getToken(FilterExpressionSyntaxParser::IDENTIFIER, 0);
}


size_t FilterExpressionSyntaxParser::TableNameContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleTableName;
}

void FilterExpressionSyntaxParser::TableNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTableName(this);
}

void FilterExpressionSyntaxParser::TableNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTableName(this);
}

FilterExpressionSyntaxParser::TableNameContext* FilterExpressionSyntaxParser::tableName() {
  TableNameContext *_localctx = _tracker.createInstance<TableNameContext>(_ctx, getState());
  enterRule(_localctx, 44, FilterExpressionSyntaxParser::RuleTableName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(236);
    match(FilterExpressionSyntaxParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ColumnNameContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::ColumnNameContext::ColumnNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::ColumnNameContext::IDENTIFIER() {
  return getToken(FilterExpressionSyntaxParser::IDENTIFIER, 0);
}


size_t FilterExpressionSyntaxParser::ColumnNameContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleColumnName;
}

void FilterExpressionSyntaxParser::ColumnNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterColumnName(this);
}

void FilterExpressionSyntaxParser::ColumnNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitColumnName(this);
}

FilterExpressionSyntaxParser::ColumnNameContext* FilterExpressionSyntaxParser::columnName() {
  ColumnNameContext *_localctx = _tracker.createInstance<ColumnNameContext>(_ctx, getState());
  enterRule(_localctx, 46, FilterExpressionSyntaxParser::RuleColumnName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(238);
    match(FilterExpressionSyntaxParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OrderByColumnNameContext ------------------------------------------------------------------

FilterExpressionSyntaxParser::OrderByColumnNameContext::OrderByColumnNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FilterExpressionSyntaxParser::OrderByColumnNameContext::IDENTIFIER() {
  return getToken(FilterExpressionSyntaxParser::IDENTIFIER, 0);
}


size_t FilterExpressionSyntaxParser::OrderByColumnNameContext::getRuleIndex() const {
  return FilterExpressionSyntaxParser::RuleOrderByColumnName;
}

void FilterExpressionSyntaxParser::OrderByColumnNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOrderByColumnName(this);
}

void FilterExpressionSyntaxParser::OrderByColumnNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FilterExpressionSyntaxListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOrderByColumnName(this);
}

FilterExpressionSyntaxParser::OrderByColumnNameContext* FilterExpressionSyntaxParser::orderByColumnName() {
  OrderByColumnNameContext *_localctx = _tracker.createInstance<OrderByColumnNameContext>(_ctx, getState());
  enterRule(_localctx, 48, FilterExpressionSyntaxParser::RuleOrderByColumnName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(240);
    match(FilterExpressionSyntaxParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool FilterExpressionSyntaxParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 9: return expressionSempred(antlrcpp::downCast<ExpressionContext *>(context), predicateIndex);
    case 10: return predicateExpressionSempred(antlrcpp::downCast<PredicateExpressionContext *>(context), predicateIndex);
    case 11: return valueExpressionSempred(antlrcpp::downCast<ValueExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool FilterExpressionSyntaxParser::expressionSempred(ExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

bool FilterExpressionSyntaxParser::predicateExpressionSempred(PredicateExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 1: return precpred(_ctx, 3);
    case 2: return precpred(_ctx, 2);
    case 3: return precpred(_ctx, 5);
    case 4: return precpred(_ctx, 4);

  default:
    break;
  }
  return true;
}

bool FilterExpressionSyntaxParser::valueExpressionSempred(ValueExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 5: return precpred(_ctx, 2);
    case 6: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

void FilterExpressionSyntaxParser::initialize() {
  ::antlr4::internal::call_once(filterexpressionsyntaxParserOnceFlag, filterexpressionsyntaxParserInitialize);
}
