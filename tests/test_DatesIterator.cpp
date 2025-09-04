#include <gtest/gtest.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "../src/DatesIterator.h"

class DatesIteratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test fixture
    }

    void TearDown() override {
        // Clean up after tests
    }

    // Helper function to format tm struct as string for comparison
    std::string formatDate(const std::tm& date) {
        std::stringstream ss;
        ss << std::put_time(&date, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    // Helper function to create a tm struct for a specific date
    std::tm createDate(int year, int month, int day, int hour = 0, int min = 0, int sec = 0) {
        std::tm date = {};
        date.tm_year = year - 1900; // tm_year is years since 1900
        date.tm_mon = month - 1;    // tm_mon is 0-based
        date.tm_mday = day;
        date.tm_hour = hour;
        date.tm_min = min;
        date.tm_sec = sec;
        std::mktime(&date); // Normalize the date
        return date;
    }

    // Helper function to check if two dates are equal (ignoring time)
    bool datesEqual(const std::tm& date1, const std::tm& date2) {
        return date1.tm_year == date2.tm_year &&
               date1.tm_mon == date2.tm_mon &&
               date1.tm_mday == date2.tm_mday;
    }
};

// Test constructor initializes to January 1, 2000
TEST_F(DatesIteratorTest, ConstructorInitializesToCorrectDate) {
    DatesIterator iterator;
    std::tm current = iterator.current();
    
    EXPECT_EQ(current.tm_year, 100); // 2000 (100 years since 1900)
    EXPECT_EQ(current.tm_mon, 0);    // January (0-based)
    EXPECT_EQ(current.tm_mday, 1);   // 1st day
    EXPECT_EQ(current.tm_hour, 0);   // Midnight
    EXPECT_EQ(current.tm_min, 0);
    EXPECT_EQ(current.tm_sec, 0);
}

// Test that current() returns the same date multiple times
TEST_F(DatesIteratorTest, CurrentReturnsSameDate) {
    DatesIterator iterator;
    std::tm first = iterator.current();
    std::tm second = iterator.current();
    
    EXPECT_TRUE(datesEqual(first, second));
    EXPECT_EQ(formatDate(first), formatDate(second));
}

// Test that next() advances by 7 days
TEST_F(DatesIteratorTest, NextAdvancesBySevenDays) {
    DatesIterator iterator;
    std::tm initial = iterator.current();
    std::tm next = iterator.next();
    
    // Calculate expected date (7 days later)
    std::tm expected = initial;
    expected.tm_mday += 7;
    std::mktime(&expected);
    
    EXPECT_TRUE(datesEqual(next, expected));
}

// Test multiple next() calls advance correctly
TEST_F(DatesIteratorTest, MultipleNextCallsAdvanceCorrectly) {
    DatesIterator iterator;
    
    // First call: Jan 1, 2000 -> Jan 8, 2000
    std::tm first = iterator.next();
    EXPECT_EQ(first.tm_year, 100); // 2000
    EXPECT_EQ(first.tm_mon, 0);    // January
    EXPECT_EQ(first.tm_mday, 8);   // 8th
    
    // Second call: Jan 8, 2000 -> Jan 15, 2000
    std::tm second = iterator.next();
    EXPECT_EQ(second.tm_year, 100); // 2000
    EXPECT_EQ(second.tm_mon, 0);    // January
    EXPECT_EQ(second.tm_mday, 15);  // 15th
    
    // Third call: Jan 15, 2000 -> Jan 22, 2000
    std::tm third = iterator.next();
    EXPECT_EQ(third.tm_year, 100); // 2000
    EXPECT_EQ(third.tm_mon, 0);    // January
    EXPECT_EQ(third.tm_mday, 22);  // 22nd
}

// Test month boundary crossing
TEST_F(DatesIteratorTest, MonthBoundaryCrossing) {
    DatesIterator iterator;
    
    // Advance to January 29, 2000
    for (int i = 0; i < 4; i++) {
        iterator.next();
    }
    
    // Next call should cross to February
    std::tm feb1 = iterator.next();
    EXPECT_EQ(feb1.tm_year, 100); // 2000
    EXPECT_EQ(feb1.tm_mon, 1);    // February (0-based)
    EXPECT_EQ(feb1.tm_mday, 5);   // 5st
}

// Test year boundary crossing
TEST_F(DatesIteratorTest, YearBoundaryCrossing) {
    DatesIterator iterator;
    
    // Advance to December 25, 2000 (approximately 52 weeks)
    for (int i = 0; i < 52; i++) {
        iterator.next();
    }
    
    // Should be in December 2000
    std::tm current = iterator.current();
    EXPECT_EQ(current.tm_year, 100); // 2000
    EXPECT_EQ(current.tm_mon, 11);   // December (0-based)
    
    // Advance a few more weeks to cross year boundary
    for (int i = 0; i < 2; i++) {
        iterator.next();
    }
    
    std::tm newYear = iterator.current();
    EXPECT_EQ(newYear.tm_year, 101); // 2001
    EXPECT_EQ(newYear.tm_mon, 0);    // January
}

// Test leap year handling
TEST_F(DatesIteratorTest, LeapYearHandling) {
    DatesIterator iterator;
    
    // Advance to February 2000 (leap year)
    for (int i = 0; i < 5; i++) {
        iterator.next();
    }
    
    std::tm feb2000 = iterator.current();
    EXPECT_EQ(feb2000.tm_year, 100); // 2000
    EXPECT_EQ(feb2000.tm_mon, 1);    // February
    
    // Advance to February 2001 (non-leap year)
    for (int i = 0; i < 52; i++) {
        iterator.next();
    }
    
    std::tm feb2001 = iterator.current();
    EXPECT_EQ(feb2001.tm_year, 101); // 2001
    EXPECT_EQ(feb2001.tm_mon, 1);    // February
}

// Test that current() reflects the state after next()
TEST_F(DatesIteratorTest, CurrentReflectsStateAfterNext) {
    DatesIterator iterator;
    
    std::tm initial = iterator.current();
    std::tm next = iterator.next();
    std::tm currentAfterNext = iterator.current();
    
    // current() should return the same date as the last next()
    EXPECT_TRUE(datesEqual(next, currentAfterNext));
    EXPECT_FALSE(datesEqual(initial, currentAfterNext));
}

// Test consistency over many iterations
TEST_F(DatesIteratorTest, ConsistencyOverManyIterations) {
    DatesIterator iterator;
    
    std::tm previous = iterator.current();
    
    // Test 100 iterations (about 2 years)
    for (int i = 0; i < 100; i++) {
        std::tm current = iterator.next();
        
        // Each date should be exactly 7 days after the previous
        std::tm expected = previous;
        expected.tm_mday += 7;
        std::mktime(&expected);
        
        EXPECT_TRUE(datesEqual(current, expected)) 
            << "Iteration " << i << ": Expected " << formatDate(expected) 
            << ", got " << formatDate(current);
        
        previous = current;
    }
}

// Test that time components remain at midnight
TEST_F(DatesIteratorTest, TimeComponentsRemainAtMidnight) {
    DatesIterator iterator;
    
    // Test multiple iterations
    for (int i = 0; i < 10; i++) {
        std::tm current = iterator.current();
        EXPECT_EQ(current.tm_hour, 0);
        EXPECT_EQ(current.tm_min, 0);
        EXPECT_EQ(current.tm_sec, 0);
        
        iterator.next();
    }
}

// Test specific known dates
TEST_F(DatesIteratorTest, SpecificKnownDates) {
    DatesIterator iterator;
    
    // Week 1: Jan 1, 2000
    std::tm week1 = iterator.current();
    EXPECT_EQ(week1.tm_year, 100);
    EXPECT_EQ(week1.tm_mon, 0);
    EXPECT_EQ(week1.tm_mday, 1);
    
    // Week 2: Jan 8, 2000
    std::tm week2 = iterator.next();
    EXPECT_EQ(week2.tm_year, 100);
    EXPECT_EQ(week2.tm_mon, 0);
    EXPECT_EQ(week2.tm_mday, 8);
    
    // Week 5: Jan 29, 2000
    for (int i = 0; i < 3; i++) {
        iterator.next();
    }
    std::tm week5 = iterator.current();
    EXPECT_EQ(week5.tm_year, 100);
    EXPECT_EQ(week5.tm_mon, 0);
    EXPECT_EQ(week5.tm_mday, 29);
    
    // Week 6: Feb 5, 2000 (month boundary)
    std::tm week6 = iterator.next();
    EXPECT_EQ(week6.tm_year, 100);
    EXPECT_EQ(week6.tm_mon, 1);
    EXPECT_EQ(week6.tm_mday, 5);
}

// Test edge case: February 29 in leap year
TEST_F(DatesIteratorTest, LeapYearFebruary29) {
    DatesIterator iterator;
    
    // Advance to February 2000 (leap year)
    for (int i = 0; i < 5; i++) {
        iterator.next();
    }
    
    // Should be able to reach over February 29, 2000
    for (int i = 0; i < 4; i++) {
        iterator.next();
    }
    
    std::tm feb29 = iterator.current();
    EXPECT_EQ(feb29.tm_year, 100); // 2000
    EXPECT_EQ(feb29.tm_mon, 2);    // March
    EXPECT_EQ(feb29.tm_mday, 4);  // 4th (leap year)
}

// Test that iterator can handle very long periods
TEST_F(DatesIteratorTest, LongPeriodHandling) {
    DatesIterator iterator;
    
    // Advance 10 years (approximately 522 weeks)
    for (int i = 0; i < 522; i++) {
        iterator.next();
    }
    
    std::tm tenYearsLater = iterator.current();
    EXPECT_EQ(tenYearsLater.tm_year, 110); // 2010
    EXPECT_EQ(tenYearsLater.tm_mon, 0);    // January
    EXPECT_EQ(tenYearsLater.tm_mday, 2);   // 2st
}
