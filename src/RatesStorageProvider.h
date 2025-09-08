#include <string>
#include <optional>
#include "SymbolInfoParser.h"

#pragma once

class RatesStorageProvider {
    std::string historyPath;
public:
    RatesStorageProvider(const std::string& historyPath);
    std::optional<SymbolInfo> getSymbolInfo(const std::string& symbol);
    std::string prepareWeekData(const std::string& symbol, std::tm currentDate);
private:
    int getWeekNumber(const std::tm& date);
    std::string escapeSymbol(const std::string& symbol);
};