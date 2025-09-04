#include <string>
#include <vector>

#pragma once

class Instrument {
public:
    std::string name;
    double mmr;
    double pipSize;
    int precision;
    std::string contractCurrency;
    std::string profitCurrency;
    int contractMultiplier;
    int baseUnitSize;
    int instrumentType;
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