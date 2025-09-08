cmake_minimum_required(VERSION 3.10)
project(FXTS2MassBacktester)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add executable with all source files
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/BacktestProjectSerializer.cpp
    src/ConsoleBacktester.cpp
    src/DatesIterator.cpp
    src/RatesStorageProvider.cpp
    src/StorageReader.cpp
    src/SymbolInfoParser.cpp
    src/IndicoreRatesSerializer.cpp
)

# Set compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Link nlohmann_json to main executable
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Google Test setup using FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# nlohmann/json setup using FetchContent
FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# Test executables
add_executable(
  BacktestProjectSerializerTests
  tests/test_BacktestProjectSerializer.cpp
  src/BacktestProjectSerializer.cpp
  src/DatesIterator.cpp
)

add_executable(
  DatesIteratorTests
  tests/test_DatesIterator.cpp
  src/DatesIterator.cpp
)

add_executable(
  SymbolInfoParserTests
  tests/test_SymbolInfoParser.cpp
  src/SymbolInfoParser.cpp
)

add_executable(
  StorageReaderTests
  tests/test_StorageReader.cpp
  src/StorageReader.cpp
)

add_executable(
  IndicoreRatesSerializerTests
  tests/test_IndicoreRatesSerializer.cpp
  src/IndicoreRatesSerializer.cpp
)

# Link test executables with gtest
target_link_libraries(
  BacktestProjectSerializerTests
  gtest_main
)

target_link_libraries(
  DatesIteratorTests
  gtest_main
)

target_link_libraries(
  SymbolInfoParserTests
  gtest_main
  nlohmann_json::nlohmann_json
)

target_link_libraries(
  StorageReaderTests
  gtest_main
)

target_link_libraries(
  IndicoreRatesSerializerTests
  gtest_main
)

# Include directories for tests
target_include_directories(BacktestProjectSerializerTests PRIVATE src)
target_include_directories(DatesIteratorTests PRIVATE src)
target_include_directories(SymbolInfoParserTests PRIVATE src)
target_include_directories(StorageReaderTests PRIVATE src)
target_include_directories(IndicoreRatesSerializerTests PRIVATE src)

# Enable testing
enable_testing()

# Add tests to CTest
add_test(NAME BacktestProjectSerializerTests COMMAND BacktestProjectSerializerTests)
add_test(NAME DatesIteratorTests COMMAND DatesIteratorTests)
add_test(NAME SymbolInfoParserTests COMMAND SymbolInfoParserTests)
add_test(NAME StorageReaderTests COMMAND StorageReaderTests)
add_test(NAME IndicoreRatesSerializerTests COMMAND IndicoreRatesSerializerTests)

# Print configuration info
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
