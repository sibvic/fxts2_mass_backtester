#include <string>
#include <optional>

class SymbolInfo {
public:
    std::optional<std::string> provider;
    std::string contractCurrency;
    std::string profitCurrency;
    double baseUnitSize;
    double contractMultiplier;
    int instrumentType;
    double mmr;
    double pipSize;
    int precision;
    std::string name;
    bool marginEnabled;
    bool withoutHistory;
    bool endOfHistoryReached;
};

class SymbolInfoParser {
public:
    static SymbolInfo parse(const std::string& path);
};
