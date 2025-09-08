#include "StorageReader.h"
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>

std::optional<Data> StorageReader::readNext(std::ifstream& file) {
    std::string line;
    if (std::getline(file, line)) {
        // Split line by semicolon
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
        while (std::getline(ss, token, ';')) {
            tokens.push_back(token);
        }
        
        // Expected format: time;open bid;high bid;low bid;close bid;open ask;high ask;low ask;close ask;volume bid;volume ask
        if (tokens.size() != 11) {
            return std::nullopt;
        }
        
        // Helper function to convert comma decimal to dot decimal
        auto parseDecimal = [](const std::string& str) -> double {
            std::string normalized = str;
            std::replace(normalized.begin(), normalized.end(), ',', '.');
            return std::stod(normalized);
        };

        // Helper function to parse timestamp
        auto parseTimestamp = [](const std::string& str) -> std::optional<std::tm> {
            std::tm tm = {};
            std::istringstream ss(str);
            ss >> std::get_time(&tm, "%d.%m.%Y %H:%M:%S");
            
            // Check if parsing failed
            if (ss.fail()) {
                return std::nullopt;
            }
            
            return tm;
        };

        try {
            // Parse timestamp first and check if it succeeded
            auto timestampResult = parseTimestamp(tokens[0]);
            if (!timestampResult.has_value()) {
                return std::nullopt;
            }
            
            Data data;
            data.timestamp = timestampResult.value();
            data.bid.open = parseDecimal(tokens[1]);
            data.bid.high = parseDecimal(tokens[2]);
            data.bid.low = parseDecimal(tokens[3]);
            data.bid.close = parseDecimal(tokens[4]);
            data.bid.volume = parseDecimal(tokens[9]);
            data.ask.open = parseDecimal(tokens[5]);
            data.ask.high = parseDecimal(tokens[6]);
            data.ask.low = parseDecimal(tokens[7]);
            data.ask.close = parseDecimal(tokens[8]);
            data.ask.volume = parseDecimal(tokens[10]);
            
            return data;
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}
