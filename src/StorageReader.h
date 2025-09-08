#include <string>
#include <optional>
#include <fstream>
#include <ctime>

#pragma once

class BarData {
public:
    double open;
    double high;
    double low;
    double close;
    double volume;
};

class Data {
public:
    BarData bid;
    BarData ask;
    std::tm timestamp;
};

class StorageReader {
public:
    static std::optional<Data> readNext(std::ifstream& file);
};