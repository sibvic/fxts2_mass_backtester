#pragma once
#include <fstream>
#include <ctime>

// Forward declaration
class Data;

class IndicoreRatesSerializer {
public:
    static void serialize(std::ofstream& file, const Data& data);
};