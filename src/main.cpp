#include <iostream>
#include <string>
#include <map>
#include <vector>

struct AppConfig {
    std::string sourcesPath;
    std::string strategyId;
    std::string tradingSymbol;
    bool helpRequested = false;
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --sources_path PATH    Path to data sources" << std::endl;
    std::cout << "  --strategy_id ID       Strategy identifier" << std::endl;
    std::cout << "  --trading_symbol SYMBOL Trading symbol (e.g., EURUSD)" << std::endl;
    std::cout << "  --help                 Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  " << programName << " --sources_path ./data --strategy_id MA_CROSS --trading_symbol EURUSD" << std::endl;
}

AppConfig parseArguments(int argc, char* argv[]) {
    AppConfig config;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            config.helpRequested = true;
            return config;
        }
        else if (arg == "--sources_path" && i + 1 < argc) {
            config.sourcesPath = argv[++i];
        }
        else if (arg == "--strategy_id" && i + 1 < argc) {
            config.strategyId = argv[++i];
        }
        else if (arg == "--trading_symbol" && i + 1 < argc) {
            config.tradingSymbol = argv[++i];
        }
        else {
            std::cerr << "Error: Unknown argument or missing value: " << arg << std::endl;
            std::cerr << "Use --help for usage information." << std::endl;
            exit(1);
        }
    }
    
    return config;
}

bool validateConfig(const AppConfig& config) {
    if (config.sourcesPath.empty()) {
        std::cerr << "Error: --sources_path is required" << std::endl;
        return false;
    }
    
    if (config.strategyId.empty()) {
        std::cerr << "Error: --strategy_id is required" << std::endl;
        return false;
    }
    
    if (config.tradingSymbol.empty()) {
        std::cerr << "Error: --trading_symbol is required" << std::endl;
        return false;
    }
    
    return true;
}

void printConfig(const AppConfig& config) {
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Sources Path: " << config.sourcesPath << std::endl;
    std::cout << "  Strategy ID: " << config.strategyId << std::endl;
    std::cout << "  Trading Symbol: " << config.tradingSymbol << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "Welcome to FXTS2 Mass Backtester Console Application!" << std::endl;
    std::cout << "==================================================" << std::endl;
    
    // Parse command line arguments
    AppConfig config = parseArguments(argc, argv);
    
    if (config.helpRequested) {
        printUsage(argv[0]);
        return 0;
    }
    
    // Validate configuration
    if (!validateConfig(config)) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Display configuration
    printConfig(config);
    
    std::cout << "Starting backtesting process..." << std::endl;
    
    
    
    return 0;
}
