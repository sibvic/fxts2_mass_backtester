#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <vector>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include "BacktestProject.h"
#include "ConsoleBacktester.h"
#include "DatesIterator.h"
#include "SymbolInfoParser.h"
#include "RatesStorageProvider.h"

struct AppConfig {
    std::string sourcesPath;
    std::string strategyId;
    std::string tradingSymbol;
    std::string pathToBacktester;
    std::string historyPath;
    bool helpRequested = false;
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --sources_path PATH    Path to data sources" << std::endl;
    std::cout << "  --strategy_id ID       Strategy identifier" << std::endl;
    std::cout << "  --trading_symbol SYMBOL Trading symbol (e.g., EURUSD)" << std::endl;
    std::cout << "  --path_to_backtester PATH Path to backtester" << std::endl;
    std::cout << "  --history_path PATH    Path to history" << std::endl;
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
        else if (arg == "--path_to_backtester" && i + 1 < argc) {
            config.pathToBacktester = argv[++i];
        }
        else if (arg == "--history_path" && i + 1 < argc) {
            config.historyPath = argv[++i];
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
    std::cout << "  Path to Backtester: " << config.pathToBacktester << std::endl;
    std::cout << "  Path to History: " << config.historyPath << std::endl;
    std::cout << std::endl;
}

int getWeekNumber(const std::tm& date) {
    std::tm firstJan = date;
    std::mktime(&firstJan);
    return firstJan.tm_yday / 7 + 1;
}

void printSymbolInfo(const SymbolInfo& symbolInfo) {
    std::cout << "Loaded symbol info from: " << symbolInfo.name << std::endl;
    std::cout << "Symbol: " << symbolInfo.name;
    if (symbolInfo.provider.has_value()) {
        std::cout << ", Provider: " << symbolInfo.provider.value();
    } else {
        std::cout << ", Provider: (not specified)";
    }
    std::cout << std::endl;
    std::cout << "Contract Currency: " << symbolInfo.contractCurrency 
                << ", Profit Currency: " << symbolInfo.profitCurrency << std::endl;
    std::cout << "Base Unit Size: " << symbolInfo.baseUnitSize 
                << ", MMR: " << symbolInfo.mmr 
                << ", Pip Size: " << symbolInfo.pipSize << std::endl;
    std::cout << "Precision: " << symbolInfo.precision << std::endl;
    std::cout << "Margin Enabled: " << symbolInfo.marginEnabled << std::endl;
    std::cout << "Without History: " << symbolInfo.withoutHistory << std::endl;
    std::cout << "End of History Reached: " << symbolInfo.endOfHistoryReached << std::endl;
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
    
    // Get current time and calculate start of current week
    std::time_t now = std::time(nullptr);
    
    DatesIterator datesIterator;
    int totalWeeks = 0;
    int completedWeeks = 0;

    RatesStorageProvider ratesStorageProvider(config.historyPath);
    std::optional<SymbolInfo> symbolInfo = ratesStorageProvider.getSymbolInfo(config.tradingSymbol);
    if (!symbolInfo.has_value()) {
        std::cerr << "Warning: Symbol info not found: " << config.tradingSymbol << std::endl;
        std::cerr << "Using default symbol configuration." << std::endl;
        return 1;
    }
    printSymbolInfo(symbolInfo.value());
    
    auto backtester = ConsoleBacktester(config.pathToBacktester, "1");

    auto project = BacktestProject();
    project.strategy = config.strategyId;
    project.accountCurrency = "USD";
    project.initialAmount = 50000.0;
    project.defaultPeriod = "m1";
    project.accountLotSize = 0;
    
    // Create instrument from symbol info
    project.instruments.emplace_back(
        symbolInfo.value().name,
        symbolInfo.value().mmr,
        symbolInfo.value().pipSize,
        symbolInfo.value().precision,
        symbolInfo.value().contractCurrency,
        symbolInfo.value().profitCurrency,
        symbolInfo.value().contractMultiplier,
        symbolInfo.value().baseUnitSize,
        symbolInfo.value().instrumentType,
        std::optional<std::string>()
    );
    
    // Loop through each week from start date to current week start
    std::tm currentDate = datesIterator.current();
    std::tm nextDate = datesIterator.next();
    while (std::mktime(&nextDate) < now) {
        totalWeeks++;
        
        // Create project for this week
        project.startTime = std::mktime(&currentDate);
        project.endTime = std::mktime(&nextDate);
        std::cout << "Start date: " << std::put_time(&currentDate, "%Y-%m-%d") 
                  << ", End date: " << std::put_time(&nextDate, "%Y-%m-%d") << std::endl;
        
        // Create trading history path
        std::string tradingHistoryPath = ratesStorageProvider.prepareWeekData(config.tradingSymbol, currentDate);
        project.instruments[0].pricesFilePath = tradingHistoryPath;
            
        std::cout << "Running backtest for week " << tradingHistoryPath << std::endl;

        try {
            backtester.run(project);
            completedWeeks++;
        } catch (const std::exception& e) {
            std::cerr << "Error running backtest for week " << tradingHistoryPath 
                      << ": " << e.what() << std::endl;
        }
        
        // Move to next week
        currentDate = nextDate;
        nextDate = datesIterator.next();
    }
    
    std::cout << "Backtest completed. Processed " << completedWeeks << " out of " << totalWeeks << " weeks." << std::endl;
    
    return 0;
}
