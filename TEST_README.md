# Testing Guide for FXTS2 Mass Backtester

This document describes the testing setup and how to run tests for the FXTS2 Mass Backtester project.

## Test Framework

The project uses **Google Test (gtest)** as the testing framework, which is automatically downloaded and configured via CMake's FetchContent.

## Test Structure

### Test Files
- `tests/test_BacktestProjectSerializer.cpp` - Comprehensive tests for the BacktestProjectSerializer class
- `tests/test_DatesIterator.cpp` - Comprehensive tests for the DatesIterator class
- `tests/test_SymbolInfoParser.cpp` - Comprehensive tests for the SymbolInfoParser class

### Test Categories

#### 1. Basic Functionality Tests
- **SerializeBasicProject**: Tests basic serialization with a complete project
- **SerializeEmptyProject**: Tests serialization with minimal project data
- **ValidateXMLStructure**: Verifies correct XML structure and required elements

#### 2. Error Handling Tests
- **SerializeInvalidPath**: Tests error handling for invalid file paths
- **SerializeReadOnlyDirectory**: Tests error handling for read-only directories

#### 3. Data Validation Tests
- **ValidateDateFormatting**: Tests proper date formatting (YYYY-MM-DD HH:MM:SS)
- **ValidateInstrumentSerialization**: Tests instrument data serialization
- **ValidateStrategyParametersSerialization**: Tests strategy parameter serialization

#### 4. Complex Data Tests
- **SerializeMultipleInstruments**: Tests serialization with multiple instruments
- **SerializeMultipleStrategyParameters**: Tests serialization with multiple strategy parameters
- **SerializeSpecialCharacters**: Tests handling of special characters in strings
- **SerializeNumericPrecision**: Tests numeric precision in serialization

#### 5. DatesIterator Tests
- **ConstructorInitializesToCorrectDate**: Tests that constructor starts at January 1, 2000
- **CurrentReturnsSameDate**: Tests that current() returns consistent results
- **NextAdvancesBySevenDays**: Tests that next() advances by exactly 7 days
- **MultipleNextCallsAdvanceCorrectly**: Tests multiple consecutive next() calls
- **MonthBoundaryCrossing**: Tests proper handling of month transitions
- **YearBoundaryCrossing**: Tests proper handling of year transitions
- **LeapYearHandling**: Tests leap year date calculations
- **CurrentReflectsStateAfterNext**: Tests state consistency
- **ConsistencyOverManyIterations**: Tests long-term consistency
- **TimeComponentsRemainAtMidnight**: Tests time component preservation
- **SpecificKnownDates**: Tests specific date calculations
- **LeapYearFebruary29**: Tests February 29 handling in leap years
- **LongPeriodHandling**: Tests very long time period handling

#### 6. SymbolInfoParser Tests
- **ParseCompleteValidJson**: Tests parsing a complete valid JSON file
- **ParseWithMissingFields**: Tests parsing with missing fields (default values)
- **ParseBooleanValues**: Tests parsing boolean values (true/false)
- **ParseNumericValues**: Tests parsing numeric values in different formats
- **ParseScientificNotation**: Tests parsing scientific notation numbers
- **ParseEmptyStringValues**: Tests parsing empty string values
- **ParseWithWhitespace**: Tests parsing with whitespace in values
- **ParseDifferentFieldOrder**: Tests parsing with different field order
- **ParseWithExtraFields**: Tests parsing with extra fields (ignored)
- **ParseWithNestedObjects**: Tests parsing with nested objects
- **ParseWithArrays**: Tests parsing with arrays
- **ParseWithSpecialCharacters**: Tests parsing with special characters
- **ParseLargeNumbers**: Tests parsing with very large numbers
- **ParseNegativeNumbers**: Tests parsing with negative numbers
- **ParseZeroValues**: Tests parsing with zero values
- **ParseWithDuplicateKeys**: Tests parsing with duplicate keys
- **ParseWithLongStrings**: Tests parsing with very long strings
- **ParseWithVerySmallNumbers**: Tests parsing with very small numbers
- **ParseUnquotedBooleanValues**: Tests parsing unquoted boolean values
- **ParseEmptyJson**: Tests parsing with empty JSON object
- **ParseOnlyWhitespace**: Tests parsing with only whitespace
- **ParseInvalidNumericValues**: Tests parsing with invalid numeric values
- **ParseLargeIntegerValues**: Tests parsing with very large integers
- **ParseSmallIntegerValues**: Tests parsing with very small integers
- **ParseFileNotFound**: Tests error handling for missing files
- **ParseInvalidJson**: Tests error handling for malformed JSON

## Running Tests

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler
- Internet connection (for downloading Google Test)

### Building and Running Tests

#### Windows
```batch
# Build the project (includes tests)
build.bat

# Run tests
run_tests.bat
```

#### Linux/macOS
```bash
# Build the project
mkdir build
cd build
cmake ..
make

# Run tests
cd ..
./run_tests.sh
```

#### Manual Test Execution
```bash
# Navigate to build directory
cd build

# Run all tests
ctest -C Release

# Run tests with verbose output
ctest --output-on-failure -C Release

# Run specific test
./bin/Release/BacktestProjectSerializerTests
```

## Test Coverage

The tests cover the following aspects of BacktestProjectSerializer:

### ✅ Covered Areas
- Basic serialization functionality
- XML structure validation
- Date formatting
- Instrument serialization
- Strategy parameter serialization
- Multiple data elements
- Error handling for file operations
- Numeric precision handling
- Date iteration functionality
- Week-by-week date advancement
- Month and year boundary handling
- Leap year calculations
- Long-term date consistency
- JSON parsing functionality
- Symbol information extraction
- Boolean value parsing
- Numeric value parsing (integers, doubles, scientific notation)
- String value parsing
- Default value handling
- Malformed JSON handling
- Edge case handling (empty files, null values, etc.)

### ⚠️ Known Limitations
- **File Permission Tests**: Some file permission tests may not work on all systems.

## Test Data

Tests use sample data that includes:
- **Sample Project**: Complete BacktestProject with realistic values
- **Sample Instrument**: EURUSD with typical forex parameters
- **Sample Strategy Parameters**: FastMA and SlowMA parameters
- **Edge Cases**: Empty projects, special characters, numeric precision

## Adding New Tests

To add new tests:

1. **Create test file**: Add new test files in the `tests/` directory
2. **Update CMakeLists.txt**: Add new test executables to the CMake configuration
3. **Follow naming convention**: Use descriptive test names that indicate what is being tested
4. **Use test fixtures**: Inherit from `::testing::Test` for setup/teardown functionality
5. **Document edge cases**: Include tests for error conditions and edge cases

### Example Test Structure
```cpp
class NewFeatureTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }
    
    void TearDown() override {
        // Cleanup code
    }
};

TEST_F(NewFeatureTest, TestSpecificFunctionality) {
    // Test implementation
    EXPECT_TRUE(condition);
}
```

## Continuous Integration

For CI/CD pipelines, use:
```bash
mkdir build
cd build
cmake ..
make
ctest --output-on-failure -C Release
```

## Troubleshooting

### Common Issues

1. **Google Test Download Fails**
   - Ensure internet connection
   - Check CMake version (3.10+)
   - Try clearing CMake cache

2. **Tests Fail on Windows**
   - Ensure Visual Studio C++ workload is installed
   - Check that build.bat completed successfully

3. **Permission Errors**
   - Some tests may fail on systems with restricted permissions
   - Tests are designed to skip gracefully when appropriate

4. **File Path Issues**
   - Tests create temporary directories in `test_output/`
   - Ensure write permissions in project directory

## Test Results Interpretation

- **PASSED**: Test completed successfully
- **FAILED**: Test failed - check output for details
- **SKIPPED**: Test was skipped (usually due to system limitations)

## Performance Considerations

- Tests create temporary files and directories
- Cleanup is automatic via TearDown methods
- Tests are designed to run quickly (< 1 second total)
