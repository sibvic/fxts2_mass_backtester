#pragma once
#include <fstream>
#include <ctime>

// Forward declaration
class Data;

class IndicoreRatesSerializer {
public:
    static void addHeader(std::ofstream& file, const std::tm& from, const std::tm& to, bool hasVolume, double pipSize);
    static void serialize(std::ofstream& file, const Data& data, bool hasVolume);
};