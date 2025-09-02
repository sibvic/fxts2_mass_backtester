CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
TARGET = FXTS2MassBacktester
SOURCES = src/main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Windows-specific targets
ifeq ($(OS),Windows_NT)
clean:
	del /Q $(OBJECTS) $(TARGET).exe 2>nul || exit 0
run: $(TARGET)
	$(TARGET).exe
endif
