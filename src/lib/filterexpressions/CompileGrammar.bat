@echo off
echo Compiling FilterExpressionSyntax grammar for C++...
java -jar antlr-4.9.2-complete.jar -Dlanguage=Cpp FilterExpressionSyntax.g4
echo Finished.
pause