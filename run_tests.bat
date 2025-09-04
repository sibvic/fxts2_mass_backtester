@echo off
echo Running FXTS2 Mass Backtester Tests...

if not exist "build" (
    echo Build directory not found. Please run build.bat first.
    pause
    exit /b 1
)

cd build

echo Running tests...
ctest --output-on-failure -C Release

echo.
echo Running individual test executables...
echo Running BacktestProjectSerializerTests...
.\bin\Release\BacktestProjectSerializerTests.exe

echo.
echo Running DatesIteratorTests...
.\bin\Release\DatesIteratorTests.exe

echo.
echo Running SymbolInfoParserTests...
.\bin\Release\SymbolInfoParserTests.exe

if %ERRORLEVEL% NEQ 0 (
    echo Tests failed!
    cd ..
    pause
    exit /b 1
)

echo All tests passed!
cd ..
pause
