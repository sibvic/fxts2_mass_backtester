#include "DatesIterator.h"

DatesIterator::DatesIterator() {
    this->currentDate = std::tm();
    this->currentDate.tm_year = 100; // 2000
    this->currentDate.tm_mon = 0;    // January
    this->currentDate.tm_mday = 1;   // 1st
    this->currentDate.tm_hour = 0;
    this->currentDate.tm_min = 0;
    this->currentDate.tm_sec = 0;
    std::mktime(&this->currentDate);
}

std::tm DatesIterator::current() {
    return this->currentDate;
}

std::tm DatesIterator::next() {
    this->currentDate.tm_mday += 7;
    std::mktime(&this->currentDate);
    return this->currentDate;
}
