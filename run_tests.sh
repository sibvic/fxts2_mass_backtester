#!/bin/bash

echo "Running FXTS2 Mass Backtester Tests..."

if [ ! -d "build" ]; then
    echo "Build directory not found. Please run cmake and make first."
    exit 1
fi

cd build

echo "Running tests..."
ctest --output-on-failure -C Release

echo ""
echo "Running individual test executables..."
echo "Running BacktestProjectSerializerTests..."
./bin/Release/BacktestProjectSerializerTests

echo ""
echo "Running DatesIteratorTests..."
./bin/Release/DatesIteratorTests

if [ $? -ne 0 ]; then
    echo "Tests failed!"
    cd ..
    exit 1
fi

echo "All tests passed!"
cd ..
