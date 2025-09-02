# FXTS2 Mass Backtester - C++ Console Application

This is a basic C++ console application for the FXTS2 Mass Backtester project.

## Prerequisites

- **Windows**: Visual Studio 2019/2022 with C++ workload, or MinGW-w64
- **Linux/macOS**: GCC or Clang compiler
- **CMake**: Version 3.10 or higher (optional, for CMake builds)

## Building the Application

### Option 1: Using CMake (Recommended)

1. **Windows with Visual Studio:**
   ```batch
   build.bat
   ```
   This will automatically configure and build the project.

2. **Manual CMake build:**
   ```bash
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Release
   ```

3. **Linux/macOS:**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

### Option 2: Using Makefile

```bash
make
```

## Running the Application

### Windows
```batch
build\bin\Release\FXTS2MassBacktester.exe
```

### Linux/macOS
```bash
./FXTS2MassBacktester
```

## Project Structure

```
├── src/
│   └── main.cpp          # Main application source
├── CMakeLists.txt        # CMake build configuration
├── Makefile              # Make build configuration
├── build.bat             # Windows build script
└── BUILD_README.md       # This file
```

## Development

The application is written in C++17 and follows modern C++ practices. The main loop handles user input and provides a foundation for adding more sophisticated backtesting functionality.
