#include "BacktestProject.h"
#include <string>

class BacktestProjectSerializer {
public:
    static void serialize(const BacktestProject& project, const std::string& path);
};