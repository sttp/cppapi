@echo off
setlocal
for %%I in ("%~dp0..\..\..") do set "REPO=%%~fI"
for /f "usebackq tokens=*" %%I in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS=%%I"
if not defined VS exit /b 1
call "%VS%\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 exit /b 1
if not defined STTP_BOOST_ROOT set "STTP_BOOST_ROOT=%REPO%\..\boost"
if not defined STTP_PLATFORM_TOOLSET set "STTP_PLATFORM_TOOLSET=v145"
"%VS%\MSBuild\Current\Bin\MSBuild.exe" "%REPO%\src\lib\sttp.cpp.vcxproj" /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=%STTP_PLATFORM_TOOLSET% /p:SolutionDir="%REPO%\src\\" /nologo /v:minimal
if errorlevel 1 exit /b 1
if not exist "%REPO%\build\tests\SubscriberShutdownTest" mkdir "%REPO%\build\tests\SubscriberShutdownTest"
pushd "%REPO%\build\tests\SubscriberShutdownTest"
cl /nologo /std:c++latest /O2 /MD /EHsc /D_WIN32_WINNT=0x0601 /DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE /DUSE_UTF8_INSTEAD_OF_CODECVT /D_HAS_AUTO_PTR_ETC /DANTLR4CPP_STATIC /I"%STTP_BOOST_ROOT%" /Fe:SubscriberShutdownTest.exe /Fo:SubscriberShutdownTest.obj "%REPO%\src\samples\SubscriberShutdownTest\SubscriberShutdownTest.cpp" "%REPO%\build\output\x64\Release\lib\sttp.cpp.lib" /link /LTCG /LIBPATH:"%STTP_BOOST_ROOT%\stage\lib" ws2_32.lib
set "RESULT=%ERRORLEVEL%"
popd
exit /b %RESULT%




