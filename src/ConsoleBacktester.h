#include <string>
class ConsoleBacktester {
private:
    std::string pathToBacktester;
public:
    ConsoleBacktester(const std::string& pathToBacktester);
    void run();
};