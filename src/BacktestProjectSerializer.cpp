#include "BacktestProjectSerializer.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>
#include <vector>
#include <stdexcept>
#include "BacktestProject.h"

std::string formatDateTime(long long timestamp) {
    std::time_t time = static_cast<std::time_t>(timestamp);
    std::tm* tm = std::gmtime(&time);
    std::stringstream ss;
    ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void BacktestProjectSerializer::serialize(const BacktestProject& project, const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    file << "<project>\n";
    file << " <simplified-format value=\"1\"/>\n";
    file << " <strategy value=\"" << project.strategy << "\"/>\n";
    file << " <date-from value=\"" << formatDateTime(project.startTime) << "\"/>\n";
    file << " <date-to value=\"" << formatDateTime(project.endTime) << "\"/>\n";
    file << " <account-currency value=\"" << project.accountCurrency << "\"/>\n";
    file << " <initial-amount value=\"" << std::fixed << std::setprecision(2) << project.initialAmount << std::defaultfloat << "\"/>\n";
    file << " <default-period value=\"" << project.defaultPeriod << "\"/>\n";
    file << " <account-lot-size value=\"" << project.accountLotSize << "\"/>\n";
    file << " <instruments>\n";
    for (const auto& instrument : project.instruments) {
        file << " <instrument name=\"" << instrument.name << "\"";
        if (instrument.pricesFilePath.has_value()) {
            file << " filename=\"" << instrument.pricesFilePath.value() << "\"";
        }
        file << ">\n";
        file << " <mmr value=\"" << std::fixed << std::setprecision(2) << instrument.mmr << std::defaultfloat << "\"/>\n";
        file << " <pipSize value=\"" << instrument.pipSize << "\"/>\n";
        file << " <precision value=\"" << instrument.precision << "\"/>\n";
        file << " <contractCurrency value=\"" << instrument.contractCurrency << "\"/>\n";
        file << " <profitCurrency value=\"" << instrument.profitCurrency << "\"/>\n";
        file << " <contractMultiplier value=\"" << instrument.contractMultiplier << "\"/>\n";
        file << " <baseUnitSize value=\"" << instrument.baseUnitSize << "\"/>\n";
        file << " <instrumentType value=\"" << instrument.instrumentType << "\"/>\n";
        file << " </instrument>\n";
    }
    file << " </instruments>\n";
    file << " <strategy-params>\n";
    for (const auto& strategyParameter : project.strategyParameters) {
        file << " <strategy-param id=\"" << strategyParameter.name << "\" value=\"" << strategyParameter.value << "\"/>\n";
    }
    file << " </strategy-params>\n";
    file << " </project>\n";
    file.close();
}