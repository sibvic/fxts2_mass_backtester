#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include "../src/BacktestProjectSerializer.h"
#include "../src/BacktestProject.h"

class BacktestProjectSerializerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test directory for temporary files
        testDir = "test_output";
        std::filesystem::create_directories(testDir);
    }

    void TearDown() override {
        // Clean up test files
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    BacktestProject createSampleProject() {
        BacktestProject project;
        project.strategy = "MA_Cross_Strategy";
        project.startTime = 1609459200; // 2021-01-01 00:00:00 UTC
        project.endTime = 1640995200;   // 2022-01-01 00:00:00 UTC
        project.accountCurrency = "USD";
        project.initialAmount = 10000.0;
        project.defaultPeriod = "H1";
        project.accountLotSize = 100000;

        // Add sample instrument
        Instrument instrument;
        instrument.name = "EURUSD";
        instrument.mmr = 0.02;
        instrument.pipSize = 0.0001;
        instrument.precision = 5;
        instrument.contractCurrency = "USD";
        instrument.profitCurrency = "USD";
        instrument.contractMultiplier = 1;
        instrument.baseUnitSize = 100000;
        instrument.instrumentType = 1;
        project.instruments.push_back(instrument);

        // Add sample strategy parameters
        StrategyParameter param1;
        param1.name = "FastMA";
        param1.value = "10";
        project.strategyParameters.push_back(param1);

        StrategyParameter param2;
        param2.name = "SlowMA";
        param2.value = "20";
        project.strategyParameters.push_back(param2);

        return project;
    }

    std::string readFileContent(const std::string& filePath) {
        std::ifstream file(filePath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::string testDir;
};

// Test basic serialization functionality
TEST_F(BacktestProjectSerializerTest, SerializeBasicProject) {
    BacktestProject project = createSampleProject();
    std::string outputPath = testDir + "/test_project.xml";

    // Should not throw
    EXPECT_NO_THROW(BacktestProjectSerializer::serialize(project, outputPath));

    // Check if file was created
    EXPECT_TRUE(std::filesystem::exists(outputPath));

    // Read and verify content
    std::string content = readFileContent(outputPath);
    EXPECT_FALSE(content.empty());
    
    // Check for XML declaration
    EXPECT_TRUE(content.find("<?xml version=\"1.0\" encoding=\"utf-8\"?>") != std::string::npos);
    
    // Check for project root element
    EXPECT_TRUE(content.find("<project>") != std::string::npos);
    EXPECT_TRUE(content.find("</project>") != std::string::npos);
}

// Test serialization with empty project
TEST_F(BacktestProjectSerializerTest, SerializeEmptyProject) {
    BacktestProject project;
    project.strategy = "EmptyStrategy";
    project.startTime = 0;
    project.endTime = 0;
    project.accountCurrency = "USD";
    project.initialAmount = 0.0;
    project.defaultPeriod = "M1";
    project.accountLotSize = 0;

    std::string outputPath = testDir + "/empty_project.xml";

    EXPECT_NO_THROW(BacktestProjectSerializer::serialize(project, outputPath));
    EXPECT_TRUE(std::filesystem::exists(outputPath));

    std::string content = readFileContent(outputPath);
    EXPECT_TRUE(content.find("EmptyStrategy") != std::string::npos);
    EXPECT_TRUE(content.find("<instruments>") != std::string::npos);
    EXPECT_TRUE(content.find("</instruments>") != std::string::npos);
    EXPECT_TRUE(content.find("<strategy-params>") != std::string::npos);
    EXPECT_TRUE(content.find("</strategy-params>") != std::string::npos);
}

// Test error handling for invalid file path
TEST_F(BacktestProjectSerializerTest, SerializeInvalidPath) {
    BacktestProject project = createSampleProject();
    std::string invalidPath = "/invalid/path/that/does/not/exist/test.xml";

    EXPECT_THROW(BacktestProjectSerializer::serialize(project, invalidPath), std::runtime_error);
}

// Test error handling for read-only directory
TEST_F(BacktestProjectSerializerTest, SerializeReadOnlyDirectory) {
    BacktestProject project = createSampleProject();
    std::string readOnlyPath = "/root/test.xml"; // Assuming /root is read-only

    // This test might not work on all systems, so we'll make it conditional
    if (!std::filesystem::exists("/root")) {
        GTEST_SKIP() << "Cannot test read-only directory on this system";
    }

    EXPECT_THROW(BacktestProjectSerializer::serialize(project, readOnlyPath), std::runtime_error);
}

// Test XML structure validation
TEST_F(BacktestProjectSerializerTest, ValidateXMLStructure) {
    BacktestProject project = createSampleProject();
    std::string outputPath = testDir + "/structure_test.xml";

    BacktestProjectSerializer::serialize(project, outputPath);
    std::string content = readFileContent(outputPath);

    // Check required XML elements
    EXPECT_TRUE(content.find("<simplified-format value=\"1\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<strategy value=\"MA_Cross_Strategy\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<account-currency value=\"USD\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<initial-amount value=\"10000.00\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<default-period value=\"H1\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<account-lot-size value=\"100000\"/>") != std::string::npos);
}

// Test date formatting
TEST_F(BacktestProjectSerializerTest, ValidateDateFormatting) {
    BacktestProject project = createSampleProject();
    std::string outputPath = testDir + "/date_test.xml";

    BacktestProjectSerializer::serialize(project, outputPath);
    std::string content = readFileContent(outputPath);

    // Check date format (YYYY-MM-DD HH:MM:SS)
    EXPECT_TRUE(content.find("<date-from value=\"2021-01-01 00:00:00\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<date-to value=\"2022-01-01 00:00:00\"/>") != std::string::npos);
}

// Test instrument serialization
TEST_F(BacktestProjectSerializerTest, ValidateInstrumentSerialization) {
    BacktestProject project = createSampleProject();
    std::string outputPath = testDir + "/instrument_test.xml";

    BacktestProjectSerializer::serialize(project, outputPath);
    std::string content = readFileContent(outputPath);

    // Check instrument elements
    EXPECT_TRUE(content.find("<instrument name=\"EURUSD\">") != std::string::npos);
    EXPECT_TRUE(content.find("<mmr value=\"0.02\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<pipSize value=\"0.0001\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<precision value=\"5\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<contractCurrency value=\"USD\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<profitCurrency value=\"USD\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<contractMultiplier value=\"1\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<baseUnitSize value=\"100000\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<instrumentType value=\"1\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("</instrument>") != std::string::npos);
}

// Test strategy parameters serialization
TEST_F(BacktestProjectSerializerTest, ValidateStrategyParametersSerialization) {
    BacktestProject project = createSampleProject();
    std::string outputPath = testDir + "/params_test.xml";

    BacktestProjectSerializer::serialize(project, outputPath);
    std::string content = readFileContent(outputPath);

    // Check strategy parameter elements
    EXPECT_TRUE(content.find("<strategy-param id=\"FastMA\" value=\"10\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<strategy-param id=\"SlowMA\" value=\"20\"/>") != std::string::npos);
}

// Test multiple instruments
TEST_F(BacktestProjectSerializerTest, SerializeMultipleInstruments) {
    BacktestProject project = createSampleProject();
    
    // Add second instrument
    Instrument instrument2;
    instrument2.name = "GBPUSD";
    instrument2.mmr = 0.03;
    instrument2.pipSize = 0.0001;
    instrument2.precision = 5;
    instrument2.contractCurrency = "USD";
    instrument2.profitCurrency = "USD";
    instrument2.contractMultiplier = 1;
    instrument2.baseUnitSize = 100000;
    instrument2.instrumentType = 1;
    project.instruments.push_back(instrument2);

    std::string outputPath = testDir + "/multiple_instruments.xml";

    BacktestProjectSerializer::serialize(project, outputPath);
    std::string content = readFileContent(outputPath);

    // Check both instruments are present
    EXPECT_TRUE(content.find("<instrument name=\"EURUSD\">") != std::string::npos);
    EXPECT_TRUE(content.find("<instrument name=\"GBPUSD\">") != std::string::npos);
    
    // Count instrument occurrences
    size_t instrumentCount = 0;
    size_t pos = 0;
    while ((pos = content.find("<instrument name=", pos)) != std::string::npos) {
        instrumentCount++;
        pos += 17; // Length of "<instrument name="
    }
    EXPECT_EQ(instrumentCount, 2);
}

// Test multiple strategy parameters
TEST_F(BacktestProjectSerializerTest, SerializeMultipleStrategyParameters) {
    BacktestProject project = createSampleProject();
    
    // Add more strategy parameters
    StrategyParameter param3;
    param3.name = "StopLoss";
    param3.value = "50";
    project.strategyParameters.push_back(param3);

    StrategyParameter param4;
    param4.name = "TakeProfit";
    param4.value = "100";
    project.strategyParameters.push_back(param4);

    std::string outputPath = testDir + "/multiple_params.xml";

    BacktestProjectSerializer::serialize(project, outputPath);
    std::string content = readFileContent(outputPath);

    // Check all parameters are present
    EXPECT_TRUE(content.find("<strategy-param id=\"FastMA\" value=\"10\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<strategy-param id=\"SlowMA\" value=\"20\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<strategy-param id=\"StopLoss\" value=\"50\"/>") != std::string::npos);
    EXPECT_TRUE(content.find("<strategy-param id=\"TakeProfit\" value=\"100\"/>") != std::string::npos);
    
    // Count strategy parameter occurrences
    size_t paramCount = 0;
    size_t pos = 0;
    while ((pos = content.find("<strategy-param id=", pos)) != std::string::npos) {
        paramCount++;
        pos += 19; // Length of "<strategy-param id="
    }
    EXPECT_EQ(paramCount, 4);
}

// Test numeric precision
TEST_F(BacktestProjectSerializerTest, SerializeNumericPrecision) {
    BacktestProject project = createSampleProject();
    project.initialAmount = 12345.6789;
    
    Instrument instrument;
    instrument.name = "TEST";
    instrument.mmr = 0.123456789;
    instrument.pipSize = 0.00001;
    instrument.precision = 5;
    instrument.contractCurrency = "USD";
    instrument.profitCurrency = "USD";
    instrument.contractMultiplier = 1;
    instrument.baseUnitSize = 100000;
    instrument.instrumentType = 1;
    project.instruments.clear();
    project.instruments.push_back(instrument);

    std::string outputPath = testDir + "/precision_test.xml";

    BacktestProjectSerializer::serialize(project, outputPath);
    std::string content = readFileContent(outputPath);

    // Check that numeric values are serialized with appropriate precision
    EXPECT_TRUE(content.find("<initial-amount value=\"12345.68\"/>") != std::string::npos); // Rounded to 2 decimals
    EXPECT_TRUE(content.find("<mmr value=\"0.12\"/>") != std::string::npos); // 2 decimal places
    EXPECT_TRUE(content.find("<pipSize value=\"1e-05\"/>") != std::string::npos); // Scientific notation
}
