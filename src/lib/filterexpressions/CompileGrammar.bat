@echo off
echo Compiling grammar...
java -jar antlr-4.9-complete.jar -Dlanguage=Cpp FilterExpressionSyntax.g4
echo Finished.
pause