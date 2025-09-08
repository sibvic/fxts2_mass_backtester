#include "IndicoreRatesSerializer.h"
#include "StorageReader.h"
#include <iomanip>

void IndicoreRatesSerializer::serialize(std::ofstream& file, const Data& data) {
    // Format: YYYY.MM.DD,HH:MM,open,high,low,close,volume
    file << data.timestamp.tm_year + 1900 << "." 
         << std::setfill('0') << std::setw(2) << data.timestamp.tm_mon + 1 << "." 
         << std::setfill('0') << std::setw(2) << data.timestamp.tm_mday << ","
         << std::setfill('0') << std::setw(2) << data.timestamp.tm_hour << ":"
         << std::setfill('0') << std::setw(2) << data.timestamp.tm_min << ","
         << data.bid.open << ","
         << data.bid.high << ","
         << data.bid.low << ","
         << data.bid.close << ","
         << data.volume << "\n";
}