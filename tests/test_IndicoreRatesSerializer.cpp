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
        data.bid.volume = 14;
        
        data.ask.open = 118.17;
        data.ask.high = 112.76;
        data.ask.low = 112.73;
        data.ask.close = 112.76;
        data.ask.volume = 15;
        
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

TEST_F(IndicoreRatesSerializerTest, AddHeaderWithVolume) {
    std::ofstream file(testFileName);
    
    std::tm from = createTestTimestamp(2022, 4, 29, 14, 0, 0);
    std::tm to = createTestTimestamp(2022, 4, 29, 15, 0, 0);
    
    IndicoreRatesSerializer::addHeader(file, from, to, true, 0.0001);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "HDR;EUR/USD;29.4.2022 14:0:0;29.4.2022 15:0:0;m1;1;0.0001\n");
}

TEST_F(IndicoreRatesSerializerTest, AddHeaderWithoutVolume) {
    std::ofstream file(testFileName);
    
    std::tm from = createTestTimestamp(2022, 4, 29, 14, 0, 0);
    std::tm to = createTestTimestamp(2022, 4, 29, 15, 0, 0);
    
    IndicoreRatesSerializer::addHeader(file, from, to, false, 0.0001);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "HDR;EUR/USD;29.4.2022 14:0:0;29.4.2022 15:0:0;m1;0;0.0001\n");
}

TEST_F(IndicoreRatesSerializerTest, AddHeaderDifferentPipSize) {
    std::ofstream file(testFileName);
    
    std::tm from = createTestTimestamp(2022, 4, 29, 14, 0, 0);
    std::tm to = createTestTimestamp(2022, 4, 29, 15, 0, 0);
    
    IndicoreRatesSerializer::addHeader(file, from, to, true, 0.01);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "HDR;EUR/USD;29.4.2022 14:0:0;29.4.2022 15:0:0;m1;1;0.01\n");
}

TEST_F(IndicoreRatesSerializerTest, AddHeaderDifferentTimeRange) {
    std::ofstream file(testFileName);
    
    std::tm from = createTestTimestamp(2022, 1, 1, 0, 0, 0);
    std::tm to = createTestTimestamp(2022, 12, 31, 23, 59, 59);
    
    IndicoreRatesSerializer::addHeader(file, from, to, true, 0.0001);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "HDR;EUR/USD;1.1.2022 0:0:0;31.12.2022 23:59:59;m1;1;0.0001\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataWithVolume) {
    std::ofstream file(testFileName);
    
    Data data = createTestData();
    IndicoreRatesSerializer::serialize(file, data, true);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "DATA;29.4.2022 14:54:0;118.12;112.75;112.71;112.75;118.17;112.76;112.73;112.76;14;15\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataWithoutVolume) {
    std::ofstream file(testFileName);
    
    Data data = createTestData();
    IndicoreRatesSerializer::serialize(file, data, false);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "DATA;29.4.2022 14:54:0;118.12;112.75;112.71;112.75;118.17;112.76;112.73;112.76\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataWithZeroValues) {
    std::ofstream file(testFileName);
    
    Data data;
    data.timestamp = createTestTimestamp(2022, 4, 29, 14, 54, 0);
    data.bid.open = data.bid.high = data.bid.low = data.bid.close = 0.0;
    data.bid.volume = 0;
    data.ask.open = data.ask.high = data.ask.low = data.ask.close = 0.0;
    data.ask.volume = 0;
    
    IndicoreRatesSerializer::serialize(file, data, true);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "DATA;29.4.2022 14:54:0;0;0;0;0;0;0;0;0;0;0\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataWithNegativeValues) {
    std::ofstream file(testFileName);
    
    Data data;
    data.timestamp = createTestTimestamp(2022, 4, 29, 14, 54, 0);
    data.bid.open = -118.12;
    data.bid.high = -112.75;
    data.bid.low = -112.71;
    data.bid.close = -112.75;
    data.bid.volume = -14;
    data.ask.open = -118.17;
    data.ask.high = -112.76;
    data.ask.low = -112.73;
    data.ask.close = -112.76;
    data.ask.volume = -15;
    
    IndicoreRatesSerializer::serialize(file, data, true);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "DATA;29.4.2022 14:54:0;-118.12;-112.75;-112.71;-112.75;-118.17;-112.76;-112.73;-112.76;-14;-15\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataWithLargeValues) {
    std::ofstream file(testFileName);
    
    Data data;
    data.timestamp = createTestTimestamp(2022, 4, 29, 14, 54, 0);
    data.bid.open = 1234567.89;
    data.bid.high = 1234567.90;
    data.bid.low = 1234567.88;
    data.bid.close = 1234567.89;
    data.bid.volume = 999999;
    data.ask.open = 1234567.94;
    data.ask.high = 1234567.95;
    data.ask.low = 1234567.93;
    data.ask.close = 1234567.94;
    data.ask.volume = 999999;
    
    IndicoreRatesSerializer::serialize(file, data, true);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "DATA;29.4.2022 14:54:0;1234567.89;1234567.90;1234567.88;1234567.89;1234567.94;1234567.95;1234567.93;1234567.94;999999;999999\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataWithPreciseDecimals) {
    std::ofstream file(testFileName);
    
    Data data;
    data.timestamp = createTestTimestamp(2022, 4, 29, 14, 54, 0);
    data.bid.open = 1.23456789;
    data.bid.high = 1.23456790;
    data.bid.low = 1.23456788;
    data.bid.close = 1.23456789;
    data.bid.volume = 1;
    data.ask.open = 1.23456794;
    data.ask.high = 1.23456795;
    data.ask.low = 1.23456793;
    data.ask.close = 1.23456794;
    data.ask.volume = 1;
    
    IndicoreRatesSerializer::serialize(file, data, true);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "DATA;29.4.2022 14:54:0;1.23457;1.23457;1.23457;1.23457;1.23457;1.23457;1.23457;1.23457;1;1\n");
}

TEST_F(IndicoreRatesSerializerTest, SerializeDataDifferentTimestamp) {
    std::ofstream file(testFileName);
    
    Data data;
    data.timestamp = createTestTimestamp(2023, 12, 31, 23, 59, 59);
    data.bid.open = 1.0000;
    data.bid.high = 1.0001;
    data.bid.low = 0.9999;
    data.bid.close = 1.0000;
    data.bid.volume = 100;
    data.ask.open = 1.0002;
    data.ask.high = 1.0003;
    data.ask.low = 1.0001;
    data.ask.close = 1.0002;
    data.ask.volume = 100;
    
    IndicoreRatesSerializer::serialize(file, data, true);
    file.close();
    
    std::string content = readFileContent();
    EXPECT_EQ(content, "DATA;31.12.2023 23:59:59;1;1.0001;0.9999;1;1.0002;1.0003;1.0001;1.0002;100;100\n");
}

TEST_F(IndicoreRatesSerializerTest, CompleteWorkflowWithHeaderAndData) {
    std::ofstream file(testFileName);
    
    // Add header
    std::tm from = createTestTimestamp(2022, 4, 29, 14, 0, 0);
    std::tm to = createTestTimestamp(2022, 4, 29, 15, 0, 0);
    IndicoreRatesSerializer::addHeader(file, from, to, true, 0.0001);
    
    // Add data
    Data data1 = createTestData();
    IndicoreRatesSerializer::serialize(file, data1, true);
    
    // Add another data entry
    Data data2;
    data2.timestamp = createTestTimestamp(2022, 4, 29, 14, 55, 0);
    data2.bid.open = 119.50;
    data2.bid.high = 113.25;
    data2.bid.low = 113.20;
    data2.bid.close = 113.25;
    data2.bid.volume = 20;
    data2.ask.open = 119.55;
    data2.ask.high = 113.30;
    data2.ask.low = 113.25;
    data2.ask.close = 113.30;
    data2.ask.volume = 25;
    
    IndicoreRatesSerializer::serialize(file, data2, true);
    file.close();
    
    std::string content = readFileContent();
    std::string expected = 
        "HDR;EUR/USD;29.4.2022 14:0:0;29.4.2022 15:0:0;m1;1;0.0001\n"
        "DATA;29.4.2022 14:54:0;118.12;112.75;112.71;112.75;118.17;112.76;112.73;112.76;14;15\n"
        "DATA;29.4.2022 14:55:0;119.5;113.25;113.2;113.25;119.55;113.3;113.25;113.3;20;25\n";
    
    EXPECT_EQ(content, expected);
}

TEST_F(IndicoreRatesSerializerTest, CompleteWorkflowWithoutVolume) {
    std::ofstream file(testFileName);
    
    // Add header without volume
    std::tm from = createTestTimestamp(2022, 4, 29, 14, 0, 0);
    std::tm to = createTestTimestamp(2022, 4, 29, 15, 0, 0);
    IndicoreRatesSerializer::addHeader(file, from, to, false, 0.0001);
    
    // Add data without volume
    Data data = createTestData();
    IndicoreRatesSerializer::serialize(file, data, false);
    file.close();
    
    std::string content = readFileContent();
    std::string expected = 
        "HDR;EUR/USD;29.4.2022 14:0:0;29.4.2022 15:0:0;m1;0;0.0001\n"
        "DATA;29.4.2022 14:54:0;118.12;112.75;112.71;112.75;118.17;112.76;112.73;112.76\n";
    
    EXPECT_EQ(content, expected);
}
