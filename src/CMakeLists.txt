#******************************************************************************************************
#  CMakeLists.txt - Gbtc
#
#  Copyright © 2019, Grid Protection Alliance.  All Rights Reserved.
#
#  Licensed to the Grid Protection Alliance (GPA) under one or more contributor license agreements. See
#  the NOTICE file distributed with this work for additional information regarding copyright ownership.
#  The GPA licenses this file to you under the MIT License (MIT), the "License"; you may not use this
#  file except in compliance with the License. You may obtain a copy of the License at:
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
    "lib/CommonTypes.h"
    "lib/Convert.h"
    "lib/EndianConverter.h"
    "lib/Nullable.h"
    "lib/pugixml.hpp"
    "lib/ThreadSafeQueue.h"
    "lib/Timer.h"
    "lib/Version.h"
    "lib/data/DataColumn.h"
    "lib/data/DataRow.h"
    "lib/data/DataSet.h"
    "lib/data/DataTable.h"
    "lib/filterexpressions/ExpressionTree.h"
    "lib/filterexpressions/FilterExpressionParser.h"
    "lib/filterexpressions/FilterExpressionSyntaxBaseListener.h"
    "lib/filterexpressions/FilterExpressionSyntaxLexer.h"
    "lib/filterexpressions/FilterExpressionSyntaxListener.h"
    "lib/filterexpressions/FilterExpressionSyntaxParser.h"
    "lib/filterexpressions/antlr4-runtime/ANTLRErrorListener.h"
    "lib/filterexpressions/antlr4-runtime/ANTLRErrorStrategy.h"
    "lib/filterexpressions/antlr4-runtime/ANTLRFileStream.h"
    "lib/filterexpressions/antlr4-runtime/ANTLRInputStream.h"
    "lib/filterexpressions/antlr4-runtime/BailErrorStrategy.h"
    "lib/filterexpressions/antlr4-runtime/BaseErrorListener.h"
    "lib/filterexpressions/antlr4-runtime/BufferedTokenStream.h"
    "lib/filterexpressions/antlr4-runtime/CharStream.h"
    "lib/filterexpressions/antlr4-runtime/CommonToken.h"
    "lib/filterexpressions/antlr4-runtime/CommonTokenFactory.h"
    "lib/filterexpressions/antlr4-runtime/CommonTokenStream.h"
    "lib/filterexpressions/antlr4-runtime/ConsoleErrorListener.h"
    "lib/filterexpressions/antlr4-runtime/DefaultErrorStrategy.h"
    "lib/filterexpressions/antlr4-runtime/DiagnosticErrorListener.h"
    "lib/filterexpressions/antlr4-runtime/Exceptions.h"
    "lib/filterexpressions/antlr4-runtime/FailedPredicateException.h"
    "lib/filterexpressions/antlr4-runtime/InputMismatchException.h"
    "lib/filterexpressions/antlr4-runtime/IntStream.h"
    "lib/filterexpressions/antlr4-runtime/InterpreterRuleContext.h"
    "lib/filterexpressions/antlr4-runtime/Lexer.h"
    "lib/filterexpressions/antlr4-runtime/LexerInterpreter.h"
    "lib/filterexpressions/antlr4-runtime/LexerNoViableAltException.h"
    "lib/filterexpressions/antlr4-runtime/ListTokenSource.h"
    "lib/filterexpressions/antlr4-runtime/NoViableAltException.h"
    "lib/filterexpressions/antlr4-runtime/Parser.h"
    "lib/filterexpressions/antlr4-runtime/ParserInterpreter.h"
    "lib/filterexpressions/antlr4-runtime/ParserRuleContext.h"
    "lib/filterexpressions/antlr4-runtime/ProxyErrorListener.h"
    "lib/filterexpressions/antlr4-runtime/RecognitionException.h"
    "lib/filterexpressions/antlr4-runtime/Recognizer.h"
    "lib/filterexpressions/antlr4-runtime/RuleContext.h"
    "lib/filterexpressions/antlr4-runtime/RuleContextWithAltNum.h"
    "lib/filterexpressions/antlr4-runtime/RuntimeMetaData.h"
    "lib/filterexpressions/antlr4-runtime/Token.h"
    "lib/filterexpressions/antlr4-runtime/TokenSource.h"
    "lib/filterexpressions/antlr4-runtime/TokenStream.h"
    "lib/filterexpressions/antlr4-runtime/TokenStreamRewriter.h"
    "lib/filterexpressions/antlr4-runtime/UnbufferedCharStream.h"
    "lib/filterexpressions/antlr4-runtime/UnbufferedTokenStream.h"
    "lib/filterexpressions/antlr4-runtime/Vocabulary.h"
    "lib/filterexpressions/antlr4-runtime/WritableToken.h"
    "lib/filterexpressions/antlr4-runtime/atn/ATN.h"
    "lib/filterexpressions/antlr4-runtime/atn/ATNConfig.h"
    "lib/filterexpressions/antlr4-runtime/atn/ATNConfigSet.h"
    "lib/filterexpressions/antlr4-runtime/atn/ATNDeserializationOptions.h"
    "lib/filterexpressions/antlr4-runtime/atn/ATNDeserializer.h"
    "lib/filterexpressions/antlr4-runtime/atn/ATNSerializer.h"
    "lib/filterexpressions/antlr4-runtime/atn/ATNSimulator.h"
    "lib/filterexpressions/antlr4-runtime/atn/ATNState.h"
    "lib/filterexpressions/antlr4-runtime/atn/AbstractPredicateTransition.h"
    "lib/filterexpressions/antlr4-runtime/atn/ActionTransition.h"
    "lib/filterexpressions/antlr4-runtime/atn/AmbiguityInfo.h"
    "lib/filterexpressions/antlr4-runtime/atn/ArrayPredictionContext.h"
    "lib/filterexpressions/antlr4-runtime/atn/AtomTransition.h"
    "lib/filterexpressions/antlr4-runtime/atn/BasicBlockStartState.h"
    "lib/filterexpressions/antlr4-runtime/atn/BasicState.h"
    "lib/filterexpressions/antlr4-runtime/atn/BlockEndState.h"
    "lib/filterexpressions/antlr4-runtime/atn/BlockStartState.h"
    "lib/filterexpressions/antlr4-runtime/atn/ContextSensitivityInfo.h"
    "lib/filterexpressions/antlr4-runtime/atn/DecisionEventInfo.h"
    "lib/filterexpressions/antlr4-runtime/atn/DecisionInfo.h"
    "lib/filterexpressions/antlr4-runtime/atn/DecisionState.h"
    "lib/filterexpressions/antlr4-runtime/atn/EmptyPredictionContext.h"
    "lib/filterexpressions/antlr4-runtime/atn/EpsilonTransition.h"
    "lib/filterexpressions/antlr4-runtime/atn/ErrorInfo.h"
    "lib/filterexpressions/antlr4-runtime/atn/LL1Analyzer.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerATNConfig.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerATNSimulator.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerAction.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerActionExecutor.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerChannelAction.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerCustomAction.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerIndexedCustomAction.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerModeAction.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerMoreAction.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerPopModeAction.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerPushModeAction.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerSkipAction.h"
    "lib/filterexpressions/antlr4-runtime/atn/LexerTypeAction.h"
    "lib/filterexpressions/antlr4-runtime/atn/LookaheadEventInfo.h"
    "lib/filterexpressions/antlr4-runtime/atn/LoopEndState.h"
    "lib/filterexpressions/antlr4-runtime/atn/NotSetTransition.h"
    "lib/filterexpressions/antlr4-runtime/atn/OrderedATNConfigSet.h"
    "lib/filterexpressions/antlr4-runtime/atn/ParseInfo.h"
    "lib/filterexpressions/antlr4-runtime/atn/ParserATNSimulator.h"
    "lib/filterexpressions/antlr4-runtime/atn/PlusBlockStartState.h"
    "lib/filterexpressions/antlr4-runtime/atn/PlusLoopbackState.h"
    "lib/filterexpressions/antlr4-runtime/atn/PrecedencePredicateTransition.h"
    "lib/filterexpressions/antlr4-runtime/atn/PredicateEvalInfo.h"
    "lib/filterexpressions/antlr4-runtime/atn/PredicateTransition.h"
    "lib/filterexpressions/antlr4-runtime/atn/PredictionContext.h"
    "lib/filterexpressions/antlr4-runtime/atn/PredictionMode.h"
    "lib/filterexpressions/antlr4-runtime/atn/ProfilingATNSimulator.h"
    "lib/filterexpressions/antlr4-runtime/atn/RangeTransition.h"
    "lib/filterexpressions/antlr4-runtime/atn/RuleStartState.h"
    "lib/filterexpressions/antlr4-runtime/atn/RuleStopState.h"
    "lib/filterexpressions/antlr4-runtime/atn/RuleTransition.h"
    "lib/filterexpressions/antlr4-runtime/atn/SemanticContext.h"
    "lib/filterexpressions/antlr4-runtime/atn/SetTransition.h"
    "lib/filterexpressions/antlr4-runtime/atn/SingletonPredictionContext.h"
    "lib/filterexpressions/antlr4-runtime/atn/StarBlockStartState.h"
    "lib/filterexpressions/antlr4-runtime/atn/StarLoopEntryState.h"
    "lib/filterexpressions/antlr4-runtime/atn/StarLoopbackState.h"
    "lib/filterexpressions/antlr4-runtime/atn/TokensStartState.h"
    "lib/filterexpressions/antlr4-runtime/atn/Transition.h"
    "lib/filterexpressions/antlr4-runtime/atn/WildcardTransition.h"
    "lib/filterexpressions/antlr4-runtime/dfa/DFA.h"
    "lib/filterexpressions/antlr4-runtime/dfa/DFASerializer.h"
    "lib/filterexpressions/antlr4-runtime/dfa/DFAState.h"
    "lib/filterexpressions/antlr4-runtime/dfa/LexerDFASerializer.h"
    "lib/filterexpressions/antlr4-runtime/misc/InterpreterDataReader.h"
    "lib/filterexpressions/antlr4-runtime/misc/Interval.h"
    "lib/filterexpressions/antlr4-runtime/misc/IntervalSet.h"
    "lib/filterexpressions/antlr4-runtime/misc/MurmurHash.h"
    "lib/filterexpressions/antlr4-runtime/misc/Predicate.h"
    "lib/filterexpressions/antlr4-runtime/support/Any.h"
    "lib/filterexpressions/antlr4-runtime/support/Arrays.h"
    "lib/filterexpressions/antlr4-runtime/support/CPPUtils.h"
    "lib/filterexpressions/antlr4-runtime/support/StringUtils.h"
    "lib/filterexpressions/antlr4-runtime/support/guid.h"
    "lib/filterexpressions/antlr4-runtime/tree/ErrorNode.h"
    "lib/filterexpressions/antlr4-runtime/tree/ErrorNodeImpl.h"
    "lib/filterexpressions/antlr4-runtime/tree/IterativeParseTreeWalker.h"
    "lib/filterexpressions/antlr4-runtime/tree/ParseTree.h"
    "lib/filterexpressions/antlr4-runtime/tree/ParseTreeListener.h"
    "lib/filterexpressions/antlr4-runtime/tree/ParseTreeVisitor.h"
    "lib/filterexpressions/antlr4-runtime/tree/ParseTreeWalker.h"
    "lib/filterexpressions/antlr4-runtime/tree/TerminalNode.h"
    "lib/filterexpressions/antlr4-runtime/tree/TerminalNodeImpl.h"
    "lib/filterexpressions/antlr4-runtime/tree/Trees.h"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/Chunk.h"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/ParseTreeMatch.h"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/ParseTreePattern.h"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/ParseTreePatternMatcher.h"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/RuleTagToken.h"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/TagChunk.h"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/TextChunk.h"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/TokenTagToken.h"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPath.h"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathElement.h"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathLexer.h"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathLexerErrorListener.h"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathRuleAnywhereElement.h"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathRuleElement.h"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathTokenAnywhereElement.h"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathTokenElement.h"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathWildcardAnywhereElement.h"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathWildcardElement.h"
    "lib/transport/ActiveMeasurementsSchema.h"
    "lib/transport/CompactMeasurement.h"
    "lib/transport/Constants.h"
    "lib/transport/DataPublisher.h"
    "lib/transport/DataSubscriber.h"
    "lib/transport/MetadataSchema.h"
    "lib/transport/PublisherInstance.h"
    "lib/transport/RoutingTables.h"
    "lib/transport/SignalIndexCache.h"
    "lib/transport/SubscriberConnection.h"
    "lib/transport/SubscriberInstance.h"
    "lib/transport/TransportTypes.h"
    "lib/transport/tssc/TSSCEncoder.h"
    "lib/transport/tssc/TSSCDecoder.h"
    "lib/transport/tssc/TSSCPointMetadata.h"
)

set (STTP_SOURCE_FILES
    "lib/CommonTypes.cpp"
    "lib/Convert.cpp"
    "lib/EndianConverter.cpp"
    "lib/pugixml.cpp"
    "lib/data/DataColumn.cpp"
    "lib/data/DataRow.cpp"
    "lib/data/DataSet.cpp"
    "lib/data/DataTable.cpp"
    "lib/filterexpressions/ExpressionTree.cpp"
    "lib/filterexpressions/FilterExpressionParser.cpp"
    "lib/filterexpressions/FilterExpressionSyntaxBaseListener.cpp"
    "lib/filterexpressions/FilterExpressionSyntaxLexer.cpp"
    "lib/filterexpressions/FilterExpressionSyntaxListener.cpp"
    "lib/filterexpressions/FilterExpressionSyntaxParser.cpp"
    "lib/filterexpressions/antlr4-runtime/ANTLRErrorListener.cpp"
    "lib/filterexpressions/antlr4-runtime/ANTLRErrorStrategy.cpp"
    "lib/filterexpressions/antlr4-runtime/ANTLRFileStream.cpp"
    "lib/filterexpressions/antlr4-runtime/ANTLRInputStream.cpp"
    "lib/filterexpressions/antlr4-runtime/BailErrorStrategy.cpp"
    "lib/filterexpressions/antlr4-runtime/BaseErrorListener.cpp"
    "lib/filterexpressions/antlr4-runtime/BufferedTokenStream.cpp"
    "lib/filterexpressions/antlr4-runtime/CharStream.cpp"
    "lib/filterexpressions/antlr4-runtime/CommonToken.cpp"
    "lib/filterexpressions/antlr4-runtime/CommonTokenFactory.cpp"
    "lib/filterexpressions/antlr4-runtime/CommonTokenStream.cpp"
    "lib/filterexpressions/antlr4-runtime/ConsoleErrorListener.cpp"
    "lib/filterexpressions/antlr4-runtime/DefaultErrorStrategy.cpp"
    "lib/filterexpressions/antlr4-runtime/DiagnosticErrorListener.cpp"
    "lib/filterexpressions/antlr4-runtime/Exceptions.cpp"
    "lib/filterexpressions/antlr4-runtime/FailedPredicateException.cpp"
    "lib/filterexpressions/antlr4-runtime/InputMismatchException.cpp"
    "lib/filterexpressions/antlr4-runtime/IntStream.cpp"
    "lib/filterexpressions/antlr4-runtime/InterpreterRuleContext.cpp"
    "lib/filterexpressions/antlr4-runtime/Lexer.cpp"
    "lib/filterexpressions/antlr4-runtime/LexerInterpreter.cpp"
    "lib/filterexpressions/antlr4-runtime/LexerNoViableAltException.cpp"
    "lib/filterexpressions/antlr4-runtime/ListTokenSource.cpp"
    "lib/filterexpressions/antlr4-runtime/NoViableAltException.cpp"
    "lib/filterexpressions/antlr4-runtime/Parser.cpp"
    "lib/filterexpressions/antlr4-runtime/ParserInterpreter.cpp"
    "lib/filterexpressions/antlr4-runtime/ParserRuleContext.cpp"
    "lib/filterexpressions/antlr4-runtime/ProxyErrorListener.cpp"
    "lib/filterexpressions/antlr4-runtime/RecognitionException.cpp"
    "lib/filterexpressions/antlr4-runtime/Recognizer.cpp"
    "lib/filterexpressions/antlr4-runtime/RuleContext.cpp"
    "lib/filterexpressions/antlr4-runtime/RuleContextWithAltNum.cpp"
    "lib/filterexpressions/antlr4-runtime/RuntimeMetaData.cpp"
    "lib/filterexpressions/antlr4-runtime/Token.cpp"
    "lib/filterexpressions/antlr4-runtime/TokenSource.cpp"
    "lib/filterexpressions/antlr4-runtime/TokenStream.cpp"
    "lib/filterexpressions/antlr4-runtime/TokenStreamRewriter.cpp"
    "lib/filterexpressions/antlr4-runtime/UnbufferedCharStream.cpp"
    "lib/filterexpressions/antlr4-runtime/UnbufferedTokenStream.cpp"
    "lib/filterexpressions/antlr4-runtime/Vocabulary.cpp"
    "lib/filterexpressions/antlr4-runtime/WritableToken.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ATN.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ATNConfig.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ATNConfigSet.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ATNDeserializationOptions.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ATNDeserializer.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ATNSerializer.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ATNSimulator.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ATNState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/AbstractPredicateTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ActionTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/AmbiguityInfo.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ArrayPredictionContext.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/AtomTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/BasicBlockStartState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/BasicState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/BlockEndState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/BlockStartState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ContextSensitivityInfo.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/DecisionEventInfo.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/DecisionInfo.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/DecisionState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/EmptyPredictionContext.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/EpsilonTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ErrorInfo.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LL1Analyzer.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerATNConfig.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerATNSimulator.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerAction.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerActionExecutor.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerChannelAction.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerCustomAction.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerIndexedCustomAction.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerModeAction.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerMoreAction.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerPopModeAction.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerPushModeAction.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerSkipAction.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LexerTypeAction.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LookaheadEventInfo.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/LoopEndState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/NotSetTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/OrderedATNConfigSet.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ParseInfo.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ParserATNSimulator.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/PlusBlockStartState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/PlusLoopbackState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/PrecedencePredicateTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/PredicateEvalInfo.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/PredicateTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/PredictionContext.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/PredictionMode.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/ProfilingATNSimulator.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/RangeTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/RuleStartState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/RuleStopState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/RuleTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/SemanticContext.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/SetTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/SingletonPredictionContext.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/StarBlockStartState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/StarLoopEntryState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/StarLoopbackState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/TokensStartState.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/Transition.cpp"
    "lib/filterexpressions/antlr4-runtime/atn/WildcardTransition.cpp"
    "lib/filterexpressions/antlr4-runtime/dfa/DFA.cpp"
    "lib/filterexpressions/antlr4-runtime/dfa/DFASerializer.cpp"
    "lib/filterexpressions/antlr4-runtime/dfa/DFAState.cpp"
    "lib/filterexpressions/antlr4-runtime/dfa/LexerDFASerializer.cpp"
    "lib/filterexpressions/antlr4-runtime/misc/InterpreterDataReader.cpp"
    "lib/filterexpressions/antlr4-runtime/misc/Interval.cpp"
    "lib/filterexpressions/antlr4-runtime/misc/IntervalSet.cpp"
    "lib/filterexpressions/antlr4-runtime/misc/MurmurHash.cpp"
    "lib/filterexpressions/antlr4-runtime/misc/Predicate.cpp"
    "lib/filterexpressions/antlr4-runtime/support/Any.cpp"
    "lib/filterexpressions/antlr4-runtime/support/Arrays.cpp"
    "lib/filterexpressions/antlr4-runtime/support/CPPUtils.cpp"
    "lib/filterexpressions/antlr4-runtime/support/StringUtils.cpp"
    "lib/filterexpressions/antlr4-runtime/support/guid.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/ErrorNode.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/ErrorNodeImpl.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/IterativeParseTreeWalker.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/ParseTree.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/ParseTreeListener.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/ParseTreeVisitor.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/ParseTreeWalker.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/TerminalNode.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/TerminalNodeImpl.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/Trees.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/Chunk.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/ParseTreeMatch.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/ParseTreePattern.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/ParseTreePatternMatcher.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/RuleTagToken.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/TagChunk.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/TextChunk.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/pattern/TokenTagToken.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPath.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathElement.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathLexer.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathLexerErrorListener.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathRuleAnywhereElement.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathRuleElement.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathTokenAnywhereElement.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathTokenElement.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathWildcardAnywhereElement.cpp"
    "lib/filterexpressions/antlr4-runtime/tree/xpath/XPathWildcardElement.cpp"
    "lib/transport/ActiveMeasurementsSchema.cpp"
    "lib/transport/CompactMeasurement.cpp"
    "lib/transport/Constants.cpp"
    "lib/transport/DataPublisher.cpp"
    "lib/transport/DataSubscriber.cpp"
    "lib/transport/MetadataSchema.cpp"
    "lib/transport/PublisherInstance.cpp"
    "lib/transport/RoutingTables.cpp"
    "lib/transport/SignalIndexCache.cpp"
    "lib/transport/SubscriberConnection.cpp"
    "lib/transport/SubscriberInstance.cpp"
    "lib/transport/TransportTypes.cpp"
    "lib/transport/tssc/TSSCEncoder.cpp"
    "lib/transport/tssc/TSSCDecoder.cpp"
    "lib/transport/tssc/TSSCPointMetadata.cpp"
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
    "lib/filterexpressions/antlr4-runtime"
)

# Define macros
add_definitions(
   -DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE 
   -DBOOST_ASIO_ENABLE_CANCELIO 
   -DANTLR4CPP_STATIC 
)

# Define compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -ftabstop=4 -finput-charset=UTF-8 -fextended-identifiers -Wno-attributes")
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

# DynamicMetadataPublish sample
add_executable (DynamicMetadataPublish EXCLUDE_FROM_ALL samples/DynamicMetadataPublish/DynamicMetadataPublish.cpp samples/DynamicMetadataPublish/PublisherHandler.cpp)
target_link_libraries (DynamicMetadataPublish sttp)

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
    InstancePublish
    DynamicMetadataPublish
    FilterExpressionTests
    SimplePublish
    AdvancedPublish
)
