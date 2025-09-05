#include <string>
#include <vector>
#include <optional>

#pragma once

class Instrument {
public:
    Instrument(const std::string& name, double mmr, double pipSize, int precision, const std::string& contractCurrency, const std::string& profitCurrency, int contractMultiplier, int baseUnitSize, int instrumentType, const std::optional<std::string>& pricesFilePath) {
        this->name = name;
        this->mmr = mmr;
        this->pipSize = pipSize;
        this->precision = precision;
        this->contractCurrency = contractCurrency;
        this->profitCurrency = profitCurrency;
        this->contractMultiplier = contractMultiplier;
        this->baseUnitSize = baseUnitSize;
        this->instrumentType = instrumentType;
        this->pricesFilePath = pricesFilePath;
    }
    std::string name;
    double mmr;
    double pipSize;
    int precision;
    std::string contractCurrency;
    std::string profitCurrency;
    int contractMultiplier;
    int baseUnitSize;
    int instrumentType;
    std::optional<std::string> pricesFilePath;
};

class StrategyParameter{
public:
    std::string name;
    std::string value;
};

class BacktestProject {
public:
    std::string strategy;
    long long startTime;
    long long endTime;
    std::string accountCurrency;
    double initialAmount;
    std::string defaultPeriod;
    int accountLotSize;

    std::vector<Instrument> instruments;
    std::vector<StrategyParameter> strategyParameters;
};