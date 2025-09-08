#include <gtest/gtest.h>
#include "StorageReader.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <ctime>

class StorageReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test file
        testFileName = "test_data.txt";
    }

    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(testFileName)) {
            std::filesystem::remove(testFileName);
        }
    }

    void createTestFile(const std::string& content) {
        std::ofstream file(testFileName);
        file << content;
        file.close();
    }

    std::string testFileName;
};

TEST_F(StorageReaderTest, ReadValidData) {
    // Create test data with valid format
    std::string testData = "29.04.2022 14:54:00;118,12;112,75;112,71;112,75;118,17;112,76;112,73;112,76;14;15";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    ASSERT_TRUE(result.has_value());

    const Data& data = result.value();
    
    // Check timestamp
    EXPECT_EQ(data.timestamp.tm_year, 122); // 2022 - 1900
    EXPECT_EQ(data.timestamp.tm_mon, 3);    // April (0-based)
    EXPECT_EQ(data.timestamp.tm_mday, 29);
    EXPECT_EQ(data.timestamp.tm_hour, 14);
    EXPECT_EQ(data.timestamp.tm_min, 54);
    EXPECT_EQ(data.timestamp.tm_sec, 0);

    // Check bid data
    EXPECT_DOUBLE_EQ(data.bid.open, 118.12);
    EXPECT_DOUBLE_EQ(data.bid.high, 112.75);
    EXPECT_DOUBLE_EQ(data.bid.low, 112.71);
    EXPECT_DOUBLE_EQ(data.bid.close, 112.75);
    EXPECT_DOUBLE_EQ(data.bid.volume, 14.0);

    // Check ask data
    EXPECT_DOUBLE_EQ(data.ask.open, 118.17);
    EXPECT_DOUBLE_EQ(data.ask.high, 112.76);
    EXPECT_DOUBLE_EQ(data.ask.low, 112.73);
    EXPECT_DOUBLE_EQ(data.ask.close, 112.76);
    EXPECT_DOUBLE_EQ(data.ask.volume, 15.0);
}

TEST_F(StorageReaderTest, ReadMultipleLines) {
    // Create test data with multiple lines
    std::string testData = 
        "29.04.2022 14:54:00;118,12;112,75;112,71;112,75;118,17;112,76;112,73;112,76;14;15\n"
        "29.04.2022 15:00:00;119,50;113,25;113,20;113,25;119,55;113,30;113,25;113,30;20;25\n";
    createTestFile(testData);

    std::ifstream file(testFileName);

    // Read first line
    auto result1 = StorageReader::readNext(file);
    ASSERT_TRUE(result1.has_value());
    EXPECT_DOUBLE_EQ(result1.value().bid.open, 118.12);
    EXPECT_EQ(result1.value().timestamp.tm_hour, 14);

    // Read second line
    auto result2 = StorageReader::readNext(file);
    ASSERT_TRUE(result2.has_value());
    EXPECT_DOUBLE_EQ(result2.value().bid.open, 119.50);
    EXPECT_EQ(result2.value().timestamp.tm_hour, 15);

    // No more data
    auto result3 = StorageReader::readNext(file);
    EXPECT_FALSE(result3.has_value());
}

TEST_F(StorageReaderTest, HandleDotDecimalSeparator) {
    // Test with dot decimal separator
    std::string testData = "29.04.2022 14:54:00;118.12;112.75;112.71;112.75;118.17;112.76;112.73;112.76;14.5;15.5";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    ASSERT_TRUE(result.has_value());

    const Data& data = result.value();
    EXPECT_DOUBLE_EQ(data.bid.open, 118.12);
    EXPECT_DOUBLE_EQ(data.bid.volume, 14.5);
    EXPECT_DOUBLE_EQ(data.ask.volume, 15.5);
}

TEST_F(StorageReaderTest, HandleCommaDecimalSeparator) {
    // Test with comma decimal separator
    std::string testData = "29.04.2022 14:54:00;118,12;112,75;112,71;112,75;118,17;112,76;112,73;112,76;14,5;15,5";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    ASSERT_TRUE(result.has_value());

    const Data& data = result.value();
    EXPECT_DOUBLE_EQ(data.bid.open, 118.12);
    EXPECT_DOUBLE_EQ(data.bid.volume, 14.5);
    EXPECT_DOUBLE_EQ(data.ask.volume, 15.5);
}

TEST_F(StorageReaderTest, HandleInvalidTokenCount) {
    // Test with too few tokens
    std::string testData = "29.04.2022 14:54:00;118,12;112,75;112,71;112,75;118,17;112,76;112,73;112,76;14";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    EXPECT_FALSE(result.has_value());
}

TEST_F(StorageReaderTest, HandleTooManyTokens) {
    // Test with too many tokens
    std::string testData = "29.04.2022 14:54:00;118,12;112,75;112,71;112,75;118,17;112,76;112,73;112,76;14;15;extra";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    EXPECT_FALSE(result.has_value());
}

TEST_F(StorageReaderTest, HandleInvalidDecimalFormat) {
    // Test with invalid decimal format
    std::string testData = "29.04.2022 14:54:00;invalid;112,75;112,71;112,75;118,17;112,76;112,73;112,76;14;15";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    EXPECT_FALSE(result.has_value());
}

TEST_F(StorageReaderTest, HandleInvalidTimestampFormat) {
    // Test with invalid timestamp format
    std::string testData = "invalid-timestamp;118,12;112,75;112,71;112,75;118,17;112,76;112,73;112,76;14;15";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    EXPECT_FALSE(result.has_value());
}

TEST_F(StorageReaderTest, HandleMalformedTimestamp) {
    // Test with malformed timestamp (wrong format)
    std::string testData = "2022-04-29 14:54:00;118,12;112,75;112,71;112,75;118,17;112,76;112,73;112,76;14;15";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    EXPECT_FALSE(result.has_value());
}

TEST_F(StorageReaderTest, HandleEmptyTimestamp) {
    // Test with empty timestamp
    std::string testData = ";118,12;112,75;112,71;112,75;118,17;112,76;112,73;112,76;14;15";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    EXPECT_FALSE(result.has_value());
}

TEST_F(StorageReaderTest, HandleEmptyFile) {
    // Test with empty file
    createTestFile("");

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    EXPECT_FALSE(result.has_value());
}

TEST_F(StorageReaderTest, HandleEmptyLine) {
    // Test with empty line
    createTestFile("\n");

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    EXPECT_FALSE(result.has_value());
}

TEST_F(StorageReaderTest, HandleWhitespaceOnlyLine) {
    // Test with whitespace only line
    createTestFile("   \n");

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    EXPECT_FALSE(result.has_value());
}

TEST_F(StorageReaderTest, HandleMixedDecimalSeparators) {
    // Test with mixed decimal separators
    std::string testData = "29.04.2022 14:54:00;118,12;112.75;112,71;112.75;118,17;112.76;112,73;112.76;14,5;15.5";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    ASSERT_TRUE(result.has_value());

    const Data& data = result.value();
    EXPECT_DOUBLE_EQ(data.bid.open, 118.12);
    EXPECT_DOUBLE_EQ(data.bid.high, 112.75);
    EXPECT_DOUBLE_EQ(data.bid.low, 112.71);
    EXPECT_DOUBLE_EQ(data.bid.close, 112.75);
    EXPECT_DOUBLE_EQ(data.ask.open, 118.17);
    EXPECT_DOUBLE_EQ(data.ask.high, 112.76);
    EXPECT_DOUBLE_EQ(data.ask.low, 112.73);
    EXPECT_DOUBLE_EQ(data.ask.close, 112.76);
    EXPECT_DOUBLE_EQ(data.bid.volume, 14.5);
    EXPECT_DOUBLE_EQ(data.ask.volume, 15.5);
}

TEST_F(StorageReaderTest, HandleZeroValues) {
    // Test with zero values
    std::string testData = "29.04.2022 14:54:00;0;0;0;0;0;0;0;0;0;0";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    ASSERT_TRUE(result.has_value());

    const Data& data = result.value();
    EXPECT_DOUBLE_EQ(data.bid.open, 0.0);
    EXPECT_DOUBLE_EQ(data.bid.high, 0.0);
    EXPECT_DOUBLE_EQ(data.bid.low, 0.0);
    EXPECT_DOUBLE_EQ(data.bid.close, 0.0);
    EXPECT_DOUBLE_EQ(data.bid.volume, 0.0);
    EXPECT_DOUBLE_EQ(data.ask.open, 0.0);
    EXPECT_DOUBLE_EQ(data.ask.high, 0.0);
    EXPECT_DOUBLE_EQ(data.ask.low, 0.0);
    EXPECT_DOUBLE_EQ(data.ask.close, 0.0);
    EXPECT_DOUBLE_EQ(data.ask.volume, 0.0);
}

TEST_F(StorageReaderTest, HandleNegativeValues) {
    // Test with negative values (if supported by the format)
    std::string testData = "29.04.2022 14:54:00;-118,12;-112,75;-112,71;-112,75;-118,17;-112,76;-112,73;-112,76;-14;-15";
    createTestFile(testData);

    std::ifstream file(testFileName);

    auto result = StorageReader::readNext(file);
    ASSERT_TRUE(result.has_value());

    const Data& data = result.value();
    EXPECT_DOUBLE_EQ(data.bid.open, -118.12);
    EXPECT_DOUBLE_EQ(data.bid.high, -112.75);
    EXPECT_DOUBLE_EQ(data.bid.low, -112.71);
    EXPECT_DOUBLE_EQ(data.bid.close, -112.75);
    EXPECT_DOUBLE_EQ(data.bid.volume, -14.0);
    EXPECT_DOUBLE_EQ(data.ask.open, -118.17);
    EXPECT_DOUBLE_EQ(data.ask.high, -112.76);
    EXPECT_DOUBLE_EQ(data.ask.low, -112.73);
    EXPECT_DOUBLE_EQ(data.ask.close, -112.76);
    EXPECT_DOUBLE_EQ(data.ask.volume, -15.0);
}
