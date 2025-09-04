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
)

# Set compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE src)

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

# Test executable
add_executable(
  BacktestProjectSerializerTests
  tests/test_BacktestProjectSerializer.cpp
  src/BacktestProjectSerializer.cpp
)

# Link test executable with gtest
target_link_libraries(
  BacktestProjectSerializerTests
  gtest_main
)

# Include directories for tests
target_include_directories(BacktestProjectSerializerTests PRIVATE src)

# Enable testing
enable_testing()

# Add test to CTest
add_test(NAME BacktestProjectSerializerTests COMMAND BacktestProjectSerializerTests)

# Print configuration info
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
