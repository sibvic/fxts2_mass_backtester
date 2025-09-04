#include "ConsoleBacktester.h"
#include <string>
#include <iostream>

ConsoleBacktester::ConsoleBacktester(const std::string& pathToBacktester) {
    this->pathToBacktester = pathToBacktester;
}

void ConsoleBacktester::run() {
    if (pathToBacktester.empty()) {
        std::cerr << "Error: Path to backtester is not set" << std::endl;
        return;
    }

    std::cout << "Running backtester from " << pathToBacktester << std::endl;
}