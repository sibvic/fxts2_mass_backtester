#include "RatesStorageProvider.h"
#include <filesystem>
#include "StorageReader.h"
#include "IndicoreRatesSerializer.h"
#include <algorithm>
#include <fstream>

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

std::optional<std::string> RatesStorageProvider::prepareWeekData(const std::string& symbol, const std::tm& currentDate) {
    int week = getWeekNumber(currentDate);
    std::string escapedSymbol = escapeSymbol(symbol);
    auto storagePath = historyPath + "/" + escapedSymbol + "/" + (std::to_string(currentDate.tm_year + 1900) + "-" 
        + std::to_string(week) + ".csv");
    std::ifstream file(storagePath);
    if (!file.is_open()) {
        return std::nullopt;
    }
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "fxts2_backtester";
    auto targetStoragePath = tempDir / (std::to_string(currentDate.tm_year + 1900) + "-" + std::to_string(week) + ".csv");
    std::ofstream targetFile(targetStoragePath);
    if (!targetFile.is_open()) {
        file.close();
        return std::nullopt;
    }
    IndicoreRatesSerializer::addHeader(targetFile, currentDate, currentDate, true, 0.0001);
    while (auto&& data = StorageReader::readNext(file)) {
        IndicoreRatesSerializer::serialize(targetFile, data.value(), true);
    }
    targetFile.close();
    file.close();
    return targetStoragePath.string();
}
