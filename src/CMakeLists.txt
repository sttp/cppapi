#******************************************************************************************************
#  CMakeLists.txt - Gbtc
#
#  Copyright © 2019, Grid Protection Alliance.  All Rights Reserved.
#
#  Licensed to the Grid Protection Alliance (GPA) under one or more contributor license agreements. See
#  the NOTICE file distributed with this work for additional information regarding copyright ownership.
#  The GPA licenses this file to you under the Eclipse Public License -v 1.0 (the "License"); you may
#  not use this file except in compliance with the License. You may obtain a copy of the License at:
#
#      http://opensource.org/licenses/MIT
#
#  Unless agreed to in writing, the subject software distributed under the License is distributed on an
#  "AS-IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. Refer to the
#  License for the specific language governing permissions and limitations.
#
#  Code Modification History:
#  ----------------------------------------------------------------------------------------------------
#  04/08/2019 - J. Ritchie Carroll
#       Generated original version of source code.
#
#******************************************************************************************************

cmake_minimum_required (VERSION 2.8)
project (sttp)

set (HEADER_OUTPUT_DIRECTORY Include/sttp)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY Output)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY Libraries)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY Libraries)

set (STTP_HEADER_FILES
    "lib/Common/CommonTypes.h"
    "lib/Common/Convert.h"
    "lib/Common/EndianConverter.h"
    "lib/Common/Nullable.h"
    "lib/Common/pugixml.hpp"
    "lib/Common/ThreadSafeQueue.h"
    "lib/Common/Timer.h"
    "lib/Data/DataColumn.h"
    "lib/Data/DataRow.h"
    "lib/Data/DataSet.h"
    "lib/Data/DataTable.h"
    "lib/FilterExpressions/ExpressionTree.h"
    "lib/FilterExpressions/FilterExpressionParser.h"
    "lib/FilterExpressions/FilterExpressionSyntaxBaseListener.h"
    "lib/FilterExpressions/FilterExpressionSyntaxLexer.h"
    "lib/FilterExpressions/FilterExpressionSyntaxListener.h"
    "lib/FilterExpressions/FilterExpressionSyntaxParser.h"
    "lib/FilterExpressions/antlr4-runtime/ANTLRErrorListener.h"
    "lib/FilterExpressions/antlr4-runtime/ANTLRErrorStrategy.h"
    "lib/FilterExpressions/antlr4-runtime/ANTLRFileStream.h"
    "lib/FilterExpressions/antlr4-runtime/ANTLRInputStream.h"
    "lib/FilterExpressions/antlr4-runtime/BailErrorStrategy.h"
    "lib/FilterExpressions/antlr4-runtime/BaseErrorListener.h"
    "lib/FilterExpressions/antlr4-runtime/BufferedTokenStream.h"
    "lib/FilterExpressions/antlr4-runtime/CharStream.h"
    "lib/FilterExpressions/antlr4-runtime/CommonToken.h"
    "lib/FilterExpressions/antlr4-runtime/CommonTokenFactory.h"
    "lib/FilterExpressions/antlr4-runtime/CommonTokenStream.h"
    "lib/FilterExpressions/antlr4-runtime/ConsoleErrorListener.h"
    "lib/FilterExpressions/antlr4-runtime/DefaultErrorStrategy.h"
    "lib/FilterExpressions/antlr4-runtime/DiagnosticErrorListener.h"
    "lib/FilterExpressions/antlr4-runtime/Exceptions.h"
    "lib/FilterExpressions/antlr4-runtime/FailedPredicateException.h"
    "lib/FilterExpressions/antlr4-runtime/InputMismatchException.h"
    "lib/FilterExpressions/antlr4-runtime/IntStream.h"
    "lib/FilterExpressions/antlr4-runtime/InterpreterRuleContext.h"
    "lib/FilterExpressions/antlr4-runtime/Lexer.h"
    "lib/FilterExpressions/antlr4-runtime/LexerInterpreter.h"
    "lib/FilterExpressions/antlr4-runtime/LexerNoViableAltException.h"
    "lib/FilterExpressions/antlr4-runtime/ListTokenSource.h"
    "lib/FilterExpressions/antlr4-runtime/NoViableAltException.h"
    "lib/FilterExpressions/antlr4-runtime/Parser.h"
    "lib/FilterExpressions/antlr4-runtime/ParserInterpreter.h"
    "lib/FilterExpressions/antlr4-runtime/ParserRuleContext.h"
    "lib/FilterExpressions/antlr4-runtime/ProxyErrorListener.h"
    "lib/FilterExpressions/antlr4-runtime/RecognitionException.h"
    "lib/FilterExpressions/antlr4-runtime/Recognizer.h"
    "lib/FilterExpressions/antlr4-runtime/RuleContext.h"
    "lib/FilterExpressions/antlr4-runtime/RuleContextWithAltNum.h"
    "lib/FilterExpressions/antlr4-runtime/RuntimeMetaData.h"
    "lib/FilterExpressions/antlr4-runtime/Token.h"
    "lib/FilterExpressions/antlr4-runtime/TokenSource.h"
    "lib/FilterExpressions/antlr4-runtime/TokenStream.h"
    "lib/FilterExpressions/antlr4-runtime/TokenStreamRewriter.h"
    "lib/FilterExpressions/antlr4-runtime/UnbufferedCharStream.h"
    "lib/FilterExpressions/antlr4-runtime/UnbufferedTokenStream.h"
    "lib/FilterExpressions/antlr4-runtime/Vocabulary.h"
    "lib/FilterExpressions/antlr4-runtime/WritableToken.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ATN.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNConfig.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNConfigSet.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNDeserializationOptions.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNDeserializer.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNSerializer.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNSimulator.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/AbstractPredicateTransition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ActionTransition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/AmbiguityInfo.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ArrayPredictionContext.h"
    "lib/FilterExpressions/antlr4-runtime/atn/AtomTransition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/BasicBlockStartState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/BasicState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/BlockEndState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/BlockStartState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ContextSensitivityInfo.h"
    "lib/FilterExpressions/antlr4-runtime/atn/DecisionEventInfo.h"
    "lib/FilterExpressions/antlr4-runtime/atn/DecisionInfo.h"
    "lib/FilterExpressions/antlr4-runtime/atn/DecisionState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/EmptyPredictionContext.h"
    "lib/FilterExpressions/antlr4-runtime/atn/EpsilonTransition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ErrorInfo.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LL1Analyzer.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerATNConfig.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerATNSimulator.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerAction.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerActionExecutor.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerChannelAction.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerCustomAction.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerIndexedCustomAction.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerModeAction.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerMoreAction.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerPopModeAction.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerPushModeAction.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerSkipAction.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerTypeAction.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LookaheadEventInfo.h"
    "lib/FilterExpressions/antlr4-runtime/atn/LoopEndState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/NotSetTransition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/OrderedATNConfigSet.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ParseInfo.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ParserATNSimulator.h"
    "lib/FilterExpressions/antlr4-runtime/atn/PlusBlockStartState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/PlusLoopbackState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/PrecedencePredicateTransition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/PredicateEvalInfo.h"
    "lib/FilterExpressions/antlr4-runtime/atn/PredicateTransition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/PredictionContext.h"
    "lib/FilterExpressions/antlr4-runtime/atn/PredictionMode.h"
    "lib/FilterExpressions/antlr4-runtime/atn/ProfilingATNSimulator.h"
    "lib/FilterExpressions/antlr4-runtime/atn/RangeTransition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/RuleStartState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/RuleStopState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/RuleTransition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/SemanticContext.h"
    "lib/FilterExpressions/antlr4-runtime/atn/SetTransition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/SingletonPredictionContext.h"
    "lib/FilterExpressions/antlr4-runtime/atn/StarBlockStartState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/StarLoopEntryState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/StarLoopbackState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/TokensStartState.h"
    "lib/FilterExpressions/antlr4-runtime/atn/Transition.h"
    "lib/FilterExpressions/antlr4-runtime/atn/WildcardTransition.h"
    "lib/FilterExpressions/antlr4-runtime/dfa/DFA.h"
    "lib/FilterExpressions/antlr4-runtime/dfa/DFASerializer.h"
    "lib/FilterExpressions/antlr4-runtime/dfa/DFAState.h"
    "lib/FilterExpressions/antlr4-runtime/dfa/LexerDFASerializer.h"
    "lib/FilterExpressions/antlr4-runtime/misc/InterpreterDataReader.h"
    "lib/FilterExpressions/antlr4-runtime/misc/Interval.h"
    "lib/FilterExpressions/antlr4-runtime/misc/IntervalSet.h"
    "lib/FilterExpressions/antlr4-runtime/misc/MurmurHash.h"
    "lib/FilterExpressions/antlr4-runtime/misc/Predicate.h"
    "lib/FilterExpressions/antlr4-runtime/support/Any.h"
    "lib/FilterExpressions/antlr4-runtime/support/Arrays.h"
    "lib/FilterExpressions/antlr4-runtime/support/CPPUtils.h"
    "lib/FilterExpressions/antlr4-runtime/support/StringUtils.h"
    "lib/FilterExpressions/antlr4-runtime/support/guid.h"
    "lib/FilterExpressions/antlr4-runtime/tree/ErrorNode.h"
    "lib/FilterExpressions/antlr4-runtime/tree/ErrorNodeImpl.h"
    "lib/FilterExpressions/antlr4-runtime/tree/IterativeParseTreeWalker.h"
    "lib/FilterExpressions/antlr4-runtime/tree/ParseTree.h"
    "lib/FilterExpressions/antlr4-runtime/tree/ParseTreeListener.h"
    "lib/FilterExpressions/antlr4-runtime/tree/ParseTreeVisitor.h"
    "lib/FilterExpressions/antlr4-runtime/tree/ParseTreeWalker.h"
    "lib/FilterExpressions/antlr4-runtime/tree/TerminalNode.h"
    "lib/FilterExpressions/antlr4-runtime/tree/TerminalNodeImpl.h"
    "lib/FilterExpressions/antlr4-runtime/tree/Trees.h"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/Chunk.h"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/ParseTreeMatch.h"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/ParseTreePattern.h"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/ParseTreePatternMatcher.h"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/RuleTagToken.h"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/TagChunk.h"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/TextChunk.h"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/TokenTagToken.h"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPath.h"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathElement.h"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathLexer.h"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathLexerErrorListener.h"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathRuleAnywhereElement.h"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathRuleElement.h"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathTokenAnywhereElement.h"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathTokenElement.h"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathWildcardAnywhereElement.h"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathWildcardElement.h"
    "lib/Transport/ActiveMeasurementsSchema.h"
    "lib/Transport/CompactMeasurement.h"
    "lib/Transport/Constants.h"
    "lib/Transport/DataPublisher.h"
    "lib/Transport/DataSubscriber.h"
    "lib/Transport/MetadataSchema.h"
    "lib/Transport/PublisherInstance.h"
    "lib/Transport/RoutingTables.h"
    "lib/Transport/SignalIndexCache.h"
    "lib/Transport/SubscriberConnection.h"
    "lib/Transport/SubscriberInstance.h"
    "lib/Transport/TransportTypes.h"
    "lib/Transport/TSSCEncoder.h"
    "lib/Transport/TSSCDecoder.h"
    "lib/Transport/Version.h"
)

set (STTP_SOURCE_FILES
    "lib/Common/CommonTypes.cpp"
    "lib/Common/Convert.cpp"
    "lib/Common/EndianConverter.cpp"
    "lib/Common/pugixml.cpp"
    "lib/Data/DataColumn.cpp"
    "lib/Data/DataRow.cpp"
    "lib/Data/DataSet.cpp"
    "lib/Data/DataTable.cpp"
    "lib/FilterExpressions/ExpressionTree.cpp"
    "lib/FilterExpressions/FilterExpressionParser.cpp"
    "lib/FilterExpressions/FilterExpressionSyntaxBaseListener.cpp"
    "lib/FilterExpressions/FilterExpressionSyntaxLexer.cpp"
    "lib/FilterExpressions/FilterExpressionSyntaxListener.cpp"
    "lib/FilterExpressions/FilterExpressionSyntaxParser.cpp"
    "lib/FilterExpressions/antlr4-runtime/ANTLRErrorListener.cpp"
    "lib/FilterExpressions/antlr4-runtime/ANTLRErrorStrategy.cpp"
    "lib/FilterExpressions/antlr4-runtime/ANTLRFileStream.cpp"
    "lib/FilterExpressions/antlr4-runtime/ANTLRInputStream.cpp"
    "lib/FilterExpressions/antlr4-runtime/BailErrorStrategy.cpp"
    "lib/FilterExpressions/antlr4-runtime/BaseErrorListener.cpp"
    "lib/FilterExpressions/antlr4-runtime/BufferedTokenStream.cpp"
    "lib/FilterExpressions/antlr4-runtime/CharStream.cpp"
    "lib/FilterExpressions/antlr4-runtime/CommonToken.cpp"
    "lib/FilterExpressions/antlr4-runtime/CommonTokenFactory.cpp"
    "lib/FilterExpressions/antlr4-runtime/CommonTokenStream.cpp"
    "lib/FilterExpressions/antlr4-runtime/ConsoleErrorListener.cpp"
    "lib/FilterExpressions/antlr4-runtime/DefaultErrorStrategy.cpp"
    "lib/FilterExpressions/antlr4-runtime/DiagnosticErrorListener.cpp"
    "lib/FilterExpressions/antlr4-runtime/Exceptions.cpp"
    "lib/FilterExpressions/antlr4-runtime/FailedPredicateException.cpp"
    "lib/FilterExpressions/antlr4-runtime/InputMismatchException.cpp"
    "lib/FilterExpressions/antlr4-runtime/IntStream.cpp"
    "lib/FilterExpressions/antlr4-runtime/InterpreterRuleContext.cpp"
    "lib/FilterExpressions/antlr4-runtime/Lexer.cpp"
    "lib/FilterExpressions/antlr4-runtime/LexerInterpreter.cpp"
    "lib/FilterExpressions/antlr4-runtime/LexerNoViableAltException.cpp"
    "lib/FilterExpressions/antlr4-runtime/ListTokenSource.cpp"
    "lib/FilterExpressions/antlr4-runtime/NoViableAltException.cpp"
    "lib/FilterExpressions/antlr4-runtime/Parser.cpp"
    "lib/FilterExpressions/antlr4-runtime/ParserInterpreter.cpp"
    "lib/FilterExpressions/antlr4-runtime/ParserRuleContext.cpp"
    "lib/FilterExpressions/antlr4-runtime/ProxyErrorListener.cpp"
    "lib/FilterExpressions/antlr4-runtime/RecognitionException.cpp"
    "lib/FilterExpressions/antlr4-runtime/Recognizer.cpp"
    "lib/FilterExpressions/antlr4-runtime/RuleContext.cpp"
    "lib/FilterExpressions/antlr4-runtime/RuleContextWithAltNum.cpp"
    "lib/FilterExpressions/antlr4-runtime/RuntimeMetaData.cpp"
    "lib/FilterExpressions/antlr4-runtime/Token.cpp"
    "lib/FilterExpressions/antlr4-runtime/TokenSource.cpp"
    "lib/FilterExpressions/antlr4-runtime/TokenStream.cpp"
    "lib/FilterExpressions/antlr4-runtime/TokenStreamRewriter.cpp"
    "lib/FilterExpressions/antlr4-runtime/UnbufferedCharStream.cpp"
    "lib/FilterExpressions/antlr4-runtime/UnbufferedTokenStream.cpp"
    "lib/FilterExpressions/antlr4-runtime/Vocabulary.cpp"
    "lib/FilterExpressions/antlr4-runtime/WritableToken.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ATN.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNConfig.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNConfigSet.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNDeserializationOptions.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNDeserializer.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNSerializer.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNSimulator.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ATNState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/AbstractPredicateTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ActionTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/AmbiguityInfo.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ArrayPredictionContext.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/AtomTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/BasicBlockStartState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/BasicState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/BlockEndState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/BlockStartState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ContextSensitivityInfo.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/DecisionEventInfo.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/DecisionInfo.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/DecisionState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/EmptyPredictionContext.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/EpsilonTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ErrorInfo.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LL1Analyzer.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerATNConfig.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerATNSimulator.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerAction.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerActionExecutor.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerChannelAction.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerCustomAction.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerIndexedCustomAction.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerModeAction.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerMoreAction.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerPopModeAction.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerPushModeAction.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerSkipAction.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LexerTypeAction.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LookaheadEventInfo.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/LoopEndState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/NotSetTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/OrderedATNConfigSet.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ParseInfo.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ParserATNSimulator.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/PlusBlockStartState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/PlusLoopbackState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/PrecedencePredicateTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/PredicateEvalInfo.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/PredicateTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/PredictionContext.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/PredictionMode.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/ProfilingATNSimulator.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/RangeTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/RuleStartState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/RuleStopState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/RuleTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/SemanticContext.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/SetTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/SingletonPredictionContext.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/StarBlockStartState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/StarLoopEntryState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/StarLoopbackState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/TokensStartState.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/Transition.cpp"
    "lib/FilterExpressions/antlr4-runtime/atn/WildcardTransition.cpp"
    "lib/FilterExpressions/antlr4-runtime/dfa/DFA.cpp"
    "lib/FilterExpressions/antlr4-runtime/dfa/DFASerializer.cpp"
    "lib/FilterExpressions/antlr4-runtime/dfa/DFAState.cpp"
    "lib/FilterExpressions/antlr4-runtime/dfa/LexerDFASerializer.cpp"
    "lib/FilterExpressions/antlr4-runtime/misc/InterpreterDataReader.cpp"
    "lib/FilterExpressions/antlr4-runtime/misc/Interval.cpp"
    "lib/FilterExpressions/antlr4-runtime/misc/IntervalSet.cpp"
    "lib/FilterExpressions/antlr4-runtime/misc/MurmurHash.cpp"
    "lib/FilterExpressions/antlr4-runtime/misc/Predicate.cpp"
    "lib/FilterExpressions/antlr4-runtime/support/Any.cpp"
    "lib/FilterExpressions/antlr4-runtime/support/Arrays.cpp"
    "lib/FilterExpressions/antlr4-runtime/support/CPPUtils.cpp"
    "lib/FilterExpressions/antlr4-runtime/support/StringUtils.cpp"
    "lib/FilterExpressions/antlr4-runtime/support/guid.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/ErrorNode.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/ErrorNodeImpl.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/IterativeParseTreeWalker.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/ParseTree.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/ParseTreeListener.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/ParseTreeVisitor.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/ParseTreeWalker.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/TerminalNode.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/TerminalNodeImpl.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/Trees.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/Chunk.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/ParseTreeMatch.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/ParseTreePattern.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/ParseTreePatternMatcher.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/RuleTagToken.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/TagChunk.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/TextChunk.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/pattern/TokenTagToken.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPath.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathElement.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathLexer.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathLexerErrorListener.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathRuleAnywhereElement.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathRuleElement.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathTokenAnywhereElement.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathTokenElement.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathWildcardAnywhereElement.cpp"
    "lib/FilterExpressions/antlr4-runtime/tree/xpath/XPathWildcardElement.cpp"
    "lib/Transport/ActiveMeasurementsSchema.cpp"
    "lib/Transport/CompactMeasurement.cpp"
    "lib/Transport/Constants.cpp"
    "lib/Transport/DataPublisher.cpp"
    "lib/Transport/DataSubscriber.cpp"
    "lib/Transport/MetadataSchema.cpp"
    "lib/Transport/PublisherInstance.cpp"
    "lib/Transport/RoutingTables.cpp"
    "lib/Transport/SignalIndexCache.cpp"
    "lib/Transport/SubscriberConnection.cpp"
    "lib/Transport/SubscriberInstance.cpp"
    "lib/Transport/TransportTypes.cpp"
    "lib/Transport/TSSCEncoder.cpp"
    "lib/Transport/TSSCDecoder.cpp"
)

# Option to choose whether to build static or shared libraries
option (BUILD_SHARED_LIBS "Build STTP using shared libraries" OFF)

# Copy header files
foreach (headerFile ${STTP_HEADER_FILES})
    string (REGEX MATCH "(.*)[/\\]" DIR ${headerFile})
    file (COPY ${headerFile} DESTINATION ${HEADER_OUTPUT_DIRECTORY}/${DIR})
endforeach (headerFile)

# Define include directories
include_directories(
    "lib/FilterExpressions/antlr4-runtime"
)

# Define macros
add_definitions(
   -DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE 
   -DBOOST_ASIO_ENABLE_CANCELIO 
   -DANTLR4CPP_STATIC 
)

# Define compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -ftabstop=4 -finput-charset=UTF-8 -fextended-identifiers")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

# Build STTP library
add_library (sttp ${STTP_SOURCE_FILES})
target_link_libraries (sttp pthread m z bz2 boost_system boost_thread boost_date_time boost_iostreams)

# Install headers and library
install (DIRECTORY ${PROJECT_BINARY_DIR}/${HEADER_OUTPUT_DIRECTORY} DESTINATION include)
install (TARGETS sttp DESTINATION lib)

######################
# SAMPLE EXECUTABLES #
######################

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY Samples)

# Copy sample content files
file (COPY "samples/AdvancedPublish/History.xml" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file (COPY "samples/SimplePublish/Metadata.xml" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file (COPY "samples/FilterExpressionTests/MetadataSample1.xml" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file (COPY "samples/FilterExpressionTests/MetadataSample2.xml" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# SimpleSubscribe sample
add_executable (SimpleSubscribe EXCLUDE_FROM_ALL samples/SimpleSubscribe/SimpleSubscribe.cpp)
target_link_libraries (SimpleSubscribe sttp)

# AdvancedSubscribe sample
add_executable (AdvancedSubscribe EXCLUDE_FROM_ALL samples/AdvancedSubscribe/AdvancedSubscribe.cpp)
target_link_libraries (AdvancedSubscribe sttp)

# AverageFrequencyCalculator sample
add_executable (AverageFrequencyCalculator EXCLUDE_FROM_ALL samples/AverageFrequencyCalculator/AverageFrequencyCalculator.cpp)
target_link_libraries (AverageFrequencyCalculator sttp)

# InstanceSubscribe sample
add_executable (InstanceSubscribe EXCLUDE_FROM_ALL samples/InstanceSubscribe/InstanceSubscribe.cpp samples/InstanceSubscribe/SubscriberHandler.cpp)
target_link_libraries (InstanceSubscribe sttp)

# InstancePublish sample
add_executable (InstancePublish EXCLUDE_FROM_ALL samples/InstancePublish/InstancePublish.cpp samples/InstancePublish/PublisherHandler.cpp)
target_link_libraries (InstancePublish sttp)

# FilterExpressionTests sample
add_executable (FilterExpressionTests EXCLUDE_FROM_ALL samples/FilterExpressionTests/FilterExpressionTests.cpp)
target_link_libraries (FilterExpressionTests sttp boost_filesystem)

# SimplePublish sample
add_executable (SimplePublish EXCLUDE_FROM_ALL samples/SimplePublish/SimplePublish.cpp)
target_link_libraries (SimplePublish sttp)

# AdvancedPublish sample
add_executable (AdvancedPublish EXCLUDE_FROM_ALL samples/AdvancedPublish/AdvancedPublish.cpp samples/AdvancedPublish/GenHistory.cpp samples/AdvancedPublish/TemporalSubscriber.cpp)
target_link_libraries (AdvancedPublish sttp)

# Build with 'make samples'
add_custom_target (samples DEPENDS 
    SimpleSubscribe
    AdvancedSubscribe
    AverageFrequencyCalculator
    InstanceSubscribe
    FilterExpressionTests
    SimplePublish
    AdvancedPublish
)
