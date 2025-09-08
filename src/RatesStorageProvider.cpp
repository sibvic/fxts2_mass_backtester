#include "RatesStorageProvider.h"
#include <filesystem>
#include <algorithm>

RatesStorageProvider::RatesStorageProvider(const std::string& historyPath) {
    this->historyPath = historyPath;
}

int RatesStorageProvider::getWeekNumber(const std::tm& date) {
    std::tm firstJan = date;
    std::mktime(&firstJan);
    return firstJan.tm_yday / 7 + 1;
}

std::string RatesStorageProvider::escapeSymbol(const std::string& symbol) {
    std::string escapedSymbol = symbol;
    escapedSymbol.erase(std::remove(escapedSymbol.begin(), escapedSymbol.end(), '/'), escapedSymbol.end());
    return escapedSymbol;
}

std::optional<SymbolInfo> RatesStorageProvider::getSymbolInfo(const std::string& symbol) {
    std::string escapedSymbol = escapeSymbol(symbol);
    std::string symbolInfoPath = historyPath + "/" + escapedSymbol + "/info.json";
    if (!std::filesystem::exists(symbolInfoPath)) {
        return std::nullopt;
    }
    return SymbolInfoParser::parse(symbolInfoPath);
}

std::string RatesStorageProvider::prepareWeekData(const std::string& symbol, std::tm currentDate) {
    int week = getWeekNumber(currentDate);
    std::string escapedSymbol = escapeSymbol(symbol);
    return historyPath + "/" + escapedSymbol + "/" + std::to_string(currentDate.tm_year + 1900) + "-" + std::to_string(week) + ".csv";
}
