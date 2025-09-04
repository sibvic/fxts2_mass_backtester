#include "SymbolInfoParser.h"
#include <fstream>
#include <nlohmann/json.hpp>

SymbolInfo SymbolInfoParser::parse(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    
    nlohmann::json j;
    file >> j;
    
    SymbolInfo info;
    
    // Parse string fields with default empty strings
    info.provider = j.value("Provider", "");
    info.contractCurrency = j.value("ContractCurrency", "");
    info.profitCurrency = j.value("ProfitCurrency", "");
    info.name = j.value("Name", "");
    
    // Parse numeric fields with default values
    info.baseUnitSize = j.value("BaseUnitSize", 0.0);
    info.contractMultiplier = j.value("ContractMultiplier", 0.0);
    info.instrumentType = j.value("InstrumentType", 0);
    info.mmr = j.value("MMR", 0.0);
    info.pipSize = j.value("PipSize", 0.0);
    info.precision = j.value("Precision", 0);
    
    // Parse boolean fields with default false
    info.marginEnabled = j.value("MarginEnabled", false);
    info.withoutHistory = j.value("WithoutHistory", false);
    info.endOfHistoryReached = j.value("EndOfHistoryReached", false);
    
    return info;
}