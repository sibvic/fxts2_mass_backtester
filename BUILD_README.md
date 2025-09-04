# FXTS2 Mass Backtester - C++ Console Application

This is a basic C++ console application for the FXTS2 Mass Backtester project.

## Prerequisites

- **Windows**: Visual Studio 2019/2022 with C++ workload, or MinGW-w64
- **Linux/macOS**: GCC or Clang compiler
- **CMake**: Version 3.10 or higher
- **Internet connection**: Required for downloading Google Test during build

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
# Build main application
make

# Build and run tests (if Google Test is available)
make test

# Build tests only
make test-build

# Show help
make help

# Clean build artifacts
make clean
```

```

## Running the Application

The application requires three mandatory command-line parameters:

### Required Parameters

- `--sources_path PATH` - Path to data sources directory
- `--strategy_id ID` - Strategy identifier (e.g., MA_CROSS, RSI_STRATEGY)
- `--trading_symbol SYMBOL` - Trading symbol (e.g., EURUSD, GBPUSD)

### Symbol Information Configuration

The application automatically loads symbol-specific configuration from JSON files located at:
```
{history_path}/{trading_symbol}/info.json
```

**Note:** Forward slashes (`/`) in trading symbols are automatically escaped to underscores (`_`) for file path compatibility. For example:
- Trading symbol `EUR/USD` → File path `{history_path}/EURUSD/info.json`
- Trading symbol `GBP/USD` → File path `{history_path}/GBPUSD/info.json`

**Example symbol info file structure:**
```json
{
    "Provider": "FXCM",
    "ContractCurrency": "USD",
    "ProfitCurrency": "USD",
    "BaseUnitSize": 100000.0,
    "ContractMultiplier": 1.0,
    "InstrumentType": 1,
    "MMR": 0.02,
    "PipSize": 0.0001,
    "Precision": 5,
    "Name": "EURUSD",
    "MarginEnabled": true,
    "WithoutHistory": false,
    "EndOfHistoryReached": false
}
```

**Field Descriptions:**
- `Provider`: Data provider name (e.g., "FXCM", "OANDA") - **Optional field**
- `ContractCurrency`: Contract currency (e.g., "USD", "EUR")
- `ProfitCurrency`: Profit currency (e.g., "USD", "EUR")
- `BaseUnitSize`: Base unit size for the instrument
- `ContractMultiplier`: Contract multiplier
- `InstrumentType`: Instrument type identifier
- `MMR`: Margin requirement
- `PipSize`: Size of one pip
- `Precision`: Decimal precision for price display
- `Name`: Symbol name
- `MarginEnabled`: Whether margin trading is enabled
- `WithoutHistory`: Whether historical data is missing
- `EndOfHistoryReached`: Whether end of history has been reached

If the symbol info file is not found or cannot be parsed, the application will use default values and display a warning message.

**Note:** The `Provider` field is optional. If not specified or set to `null`, the application will display "(not specified)" for the provider information.

### Usage Examples

**Windows:**
```batch
build\bin\Release\FXTS2MassBacktester.exe --sources_path ./data --strategy_id MA_CROSS --trading_symbol EUR/USD --history_path ./history
```

**Linux/macOS:**
```bash
./FXTS2MassBacktester --sources_path ./data --strategy_id MA_CROSS --trading_symbol EUR/USD --history_path ./history
```

**Show help:**
```bash
./FXTS2MassBacktester --help
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
