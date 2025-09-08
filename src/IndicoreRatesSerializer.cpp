#include "IndicoreRatesSerializer.h"
#include "StorageReader.h"
#include <iomanip>

void IndicoreRatesSerializer::addHeader(std::ofstream& file, const std::tm& from, const std::tm& to, bool hasVolume, double pipSize) {
    file << "HDR;EUR/USD;" 
        << from.tm_mday << "." << from.tm_mon + 1 << "." << from.tm_year + 1900 
        << " " << from.tm_hour << ":" << from.tm_min << ":" << from.tm_sec << ";" 
        << to.tm_mday << "." << to.tm_mon + 1 << "." << to.tm_year + 1900 
        << " " << to.tm_hour << ":" << to.tm_min << ":" << to.tm_sec << ";" 
        << "m1;" << (hasVolume ? "1" : "0") << ";" << pipSize << "\n";
}

void IndicoreRatesSerializer::serialize(std::ofstream& file, const Data& data, bool hasVolume) {
    file << "DATA;" << data.timestamp.tm_mday << "." << data.timestamp.tm_mon + 1 << "." << data.timestamp.tm_year + 1900 
        << " " << data.timestamp.tm_hour << ":" << data.timestamp.tm_min << ":" << data.timestamp.tm_sec << ";" 
        << data.bid.open << ";" << data.bid.high << ";" << data.bid.low << ";" << data.bid.close << ";" 
        << data.ask.open << ";" << data.ask.high << ";" << data.ask.low << ";" << data.ask.close;
    
    if (hasVolume) {
        file << ";" << data.bid.volume << ";" << data.ask.volume;
    }
    
    file << "\n";
}
