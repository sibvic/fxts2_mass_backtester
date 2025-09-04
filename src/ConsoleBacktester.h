#include <string>
#include "BacktestProject.h"

class ConsoleBacktester {
private:
    std::string pathToBacktester;
    std::string id;
public:
    ConsoleBacktester(const std::string& pathToBacktester, const std::string& id);
    void run(const BacktestProject& project);
};