#include <ctime>

class DatesIterator {
    std::tm currentDate;
public:
    DatesIterator();
    std::tm current();
    std::tm next();
};