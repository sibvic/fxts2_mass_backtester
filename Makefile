CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
TARGET = FXTS2MassBacktester
TEST_TARGET = BacktestProjectSerializerTests

# Main application sources
SOURCES = src/main.cpp src/BacktestProjectSerializer.cpp src/ConsoleBacktester.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Test sources
TEST_SOURCES = tests/test_BacktestProjectSerializer.cpp src/BacktestProjectSerializer.cpp
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

.PHONY: all clean run test test-build help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Note: Tests require CMake build with Google Test download
test-build:
	@echo "Tests require CMake build. Use: cmake .. && make"

test:
	@echo "Tests require CMake build. Use: cmake .. && make"

clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(TARGET) $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

help:
	@echo "Available targets:"
	@echo "  all        - Build main application"
	@echo "  test-build - Build tests (requires CMake)"
	@echo "  test       - Build and run tests (requires CMake)"
	@echo "  run        - Build and run main application"
	@echo "  clean      - Remove all build artifacts"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Note: Tests require CMake build with Google Test download"
	@echo "Use: cmake .. && make (in build directory)"

# Windows-specific targets
ifeq ($(OS),Windows_NT)
clean:
	del /Q $(OBJECTS) $(TEST_OBJECTS) $(TARGET).exe $(TEST_TARGET).exe 2>nul || exit 0
run: $(TARGET)
	$(TARGET).exe
test:
	@echo "Tests require CMake build. Use: cmake .. && make"
endif
