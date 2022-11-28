@echo off
echo Compiling FilterExpressionSyntax grammar for C++...
java -jar antlr-4.11.1-complete.jar -Dlanguage=Cpp FilterExpressionSyntax.g4
echo Replacing Go "panic" code (from UNEXPECTED_CHAR grammar handler) with C++ "throw RuntimeException"...
powershell -Command "(Get-Content FilterExpressionSyntaxParser.cpp) -replace 'panic', 'throw RuntimeException' | Set-Content -encoding UTF8 FilterExpressionSyntaxParser.cpp"
echo Finished.