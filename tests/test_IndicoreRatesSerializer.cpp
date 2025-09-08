#include <gtest/gtest.h>
#include "IndicoreRatesSerializer.h"
#include "StorageReader.h"
#include <fstream>
#include <filesystem>
#include <ctime>
#include <sstream>

class IndicoreRatesSerializerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test file
        testFileName = "test_output.txt";
    }

    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(testFileName)) {
            std::filesystem::remove(testFileName);
        }
    }

    std::string readFileContent() {
        std::ifstream file(testFileName);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    Data createTestData() {
        Data data;
        data.timestamp.tm_year = 122; // 2022 - 1900
        data.timestamp.tm_mon = 3;    // April (0-based)
        data.timestamp.tm_mday = 29;
        data.timestamp.tm_hour = 14;
        data.timestamp.tm_min = 54;
        data.timestamp.tm_sec = 0;
        
        data.bid.open = 118.12;
        data.bid.high = 112.75;
        data.bid.low = 112.71;
        data.bid.close = 112.75;
        data.ask.open = 118.17;
        data.ask.high = 112.76;
        data.ask.low = 112.73;
        data.ask.close = 112.76;
        data.volume = 14;
        
        return data;
    }

    std::tm createTestTimestamp(int year, int month, int day, int hour, int min, int sec) {
        std::tm tm = {};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = min;
        tm.tm_sec = sec;
        return tm;
    }

    std::string testFileName;
};


TEST_F(IndicoreRatesSerializerTest, SerializeDataBasic) {
    std::ofstream file(testFileName);
    
    Data data = createTestData();
    IndicoreRatesSerializer::serialize(file, data);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "2022.04.29,14:54,118.12,112.75,112.71,112.75,14\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataWithZeroValues) {
    std::ofstream file(testFileName);
    
    Data data;
    data.timestamp = createTestTimestamp(2022, 4, 29, 14, 54, 0);
    data.bid.open = data.bid.high = data.bid.low = data.bid.close = 0.0;
    data.ask.open = data.ask.high = data.ask.low = data.ask.close = 0.0;
    data.volume = 0;
    
    IndicoreRatesSerializer::serialize(file, data);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "2022.04.29,14:54,0,0,0,0,0\n");
}


TEST_F(IndicoreRatesSerializerTest, SerializeDataWithNegativeValues) {
    std::ofstream file(testFileName);
    
    Data data;
    data.timestamp = createTestTimestamp(2022, 4, 29, 14, 54, 0);
    data.bid.open = -118.12;
    data.bid.high = -112.75;
    data.bid.low = -112.71;
    data.bid.close = -112.75;
    data.volume = -14;
    
    IndicoreRatesSerializer::serialize(file, data);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "2022.04.29,14:54,-118.12,-112.75,-112.71,-112.75,-14\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataWithLargeValues) {
    std::ofstream file(testFileName);
    
    Data data;
    data.timestamp = createTestTimestamp(2022, 4, 29, 14, 54, 0);
    data.bid.open = 1234567.89;
    data.bid.high = 1234567.90;
    data.bid.low = 1234567.88;
    data.bid.close = 1234567.89;
    data.volume = 999999;
    
    IndicoreRatesSerializer::serialize(file, data);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "2022.04.29,14:54,1234567.89,1234567.90,1234567.88,1234567.89,999999\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataWithPreciseDecimals) {
    std::ofstream file(testFileName);
    
    Data data;
    data.timestamp = createTestTimestamp(2022, 4, 29, 14, 54, 0);
    data.bid.open = 1.23456789;
    data.bid.high = 1.23456790;
    data.bid.low = 1.23456788;
    data.bid.close = 1.23456789;
    data.volume = 1;
    
    IndicoreRatesSerializer::serialize(file, data);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "2022.04.29,14:54,1.23457,1.23457,1.23457,1.23457,1\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataDifferentTimestamp) {
    std::ofstream file(testFileName);
    
    Data data;
    data.timestamp = createTestTimestamp(2023, 12, 31, 23, 59, 59);
    data.bid.open = 1.0000;
    data.bid.high = 1.0001;
    data.bid.low = 0.9999;
    data.bid.close = 1.0000;
    data.volume = 100;
    
    IndicoreRatesSerializer::serialize(file, data);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "2023.12.31,23:59,1,1.0001,0.9999,1,100\n");
}

TEST_F(IndicoreRatesSerializerTest, CompleteWorkflowMultipleDataEntries) {
    std::ofstream file(testFileName);
    
    // Add first data entry
    Data data1 = createTestData();
    IndicoreRatesSerializer::serialize(file, data1);
    
    // Add second data entry
    Data data2;
    data2.timestamp = createTestTimestamp(2022, 4, 29, 14, 55, 0);
    data2.bid.open = 119.50;
    data2.bid.high = 113.25;
    data2.bid.low = 113.20;
    data2.bid.close = 113.25;
    data2.volume = 20;
    
    IndicoreRatesSerializer::serialize(file, data2);
    file.close();
    
    std::string content = readFileContent();
    std::string expected = 
        "2022.04.29,14:54,118.12,112.75,112.71,112.75,14\n"
        "2022.04.29,14:55,119.5,113.25,113.2,113.25,20\n";
    
    EXPECT_EQ(content, expected);
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataWithExampleFormat) {
    std::ofstream file(testFileName);
    
    // Test with the exact format from the example: 2016.05.16,20:46,1.132090,1.132130,1.132090,1.132090,0
    Data data;
    data.timestamp = createTestTimestamp(2016, 5, 16, 20, 46, 0);
    data.bid.open = 1.132090;
    data.bid.high = 1.132130;
    data.bid.low = 1.132090;
    data.bid.close = 1.132090;
    data.volume = 0;
    
    IndicoreRatesSerializer::serialize(file, data);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "2016.05.16,20:46,1.132090,1.132130,1.132090,1.132090,0\n");
}
