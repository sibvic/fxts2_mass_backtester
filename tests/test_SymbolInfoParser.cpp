#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <nlohmann/json.hpp>
#include "../src/SymbolInfoParser.h"

class SymbolInfoParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        testDir = std::filesystem::temp_directory_path() / "symbol_info_parser_test";
        std::filesystem::create_directories(testDir);
    }

    void TearDown() override {
        // Clean up test directory
        try {
            std::filesystem::remove_all(testDir);
        } catch (const std::exception& e) {
            // Ignore cleanup errors
        }
    }

    // Helper function to create a test JSON file
    void createTestFile(const std::string& filename, const std::string& content) {
        std::string filePath = (testDir / filename).string();
        std::ofstream file(filePath);
        file << content;
        file.close();
    }

    // Helper function to read file content
    std::string readFileContent(const std::string& filePath) {
        std::ifstream file(filePath);
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    }

    std::filesystem::path testDir;
};

// Test parsing a complete valid JSON file
TEST_F(SymbolInfoParserTest, ParseCompleteValidJson) {
    std::string jsonContent = R"({
        "Provider": "FXCM",
        "ContractCurrency": "USD",
        "ProfitCurrency": "USD",
        "BaseUnitSize": 100000.0,
        "ContractMultiplier": 1.0,
        "InstrumentType": 1,
        "MMR": 0.02,
        "PipSize": 0.0001,
        "Precision": 5,
        "Name": "EURUSD",
        "MarginEnabled": true,
        "WithoutHistory": false,
        "EndOfHistoryReached": false
    })";

    createTestFile("complete.json", jsonContent);
    std::string filePath = (testDir / "complete.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.provider, "FXCM");
    EXPECT_EQ(info.contractCurrency, "USD");
    EXPECT_EQ(info.profitCurrency, "USD");
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 100000.0);
    EXPECT_DOUBLE_EQ(info.contractMultiplier, 1.0);
    EXPECT_EQ(info.instrumentType, 1);
    EXPECT_DOUBLE_EQ(info.mmr, 0.02);
    EXPECT_DOUBLE_EQ(info.pipSize, 0.0001);
    EXPECT_EQ(info.precision, 5);
    EXPECT_EQ(info.name, "EURUSD");
    EXPECT_TRUE(info.marginEnabled);
    EXPECT_FALSE(info.withoutHistory);
    EXPECT_FALSE(info.endOfHistoryReached);
}

// Test parsing with missing fields (should use default values)
TEST_F(SymbolInfoParserTest, ParseWithMissingFields) {
    std::string jsonContent = R"({
        "Provider": "OANDA",
        "Name": "GBPUSD"
    })";

    createTestFile("partial.json", jsonContent);
    std::string filePath = (testDir / "partial.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.provider, "OANDA");
    EXPECT_EQ(info.name, "GBPUSD");
    EXPECT_EQ(info.contractCurrency, ""); // Default empty string
    EXPECT_EQ(info.profitCurrency, "");   // Default empty string
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 0.0); // Default 0.0
    EXPECT_DOUBLE_EQ(info.contractMultiplier, 0.0); // Default 0.0
    EXPECT_EQ(info.instrumentType, 0); // Default 0
    EXPECT_DOUBLE_EQ(info.mmr, 0.0); // Default 0.0
    EXPECT_DOUBLE_EQ(info.pipSize, 0.0); // Default 0.0
    EXPECT_EQ(info.precision, 0); // Default 0
    EXPECT_FALSE(info.marginEnabled); // Default false
    EXPECT_FALSE(info.withoutHistory); // Default false
    EXPECT_FALSE(info.endOfHistoryReached); // Default false
}

// Test parsing with different boolean values
TEST_F(SymbolInfoParserTest, ParseBooleanValues) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "MarginEnabled": false,
        "WithoutHistory": true,
        "EndOfHistoryReached": true
    })";

    createTestFile("booleans.json", jsonContent);
    std::string filePath = (testDir / "booleans.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_FALSE(info.marginEnabled);
    EXPECT_TRUE(info.withoutHistory);
    EXPECT_TRUE(info.endOfHistoryReached);
}

// Test parsing with numeric values in different formats
TEST_F(SymbolInfoParserTest, ParseNumericValues) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": 50000,
        "ContractMultiplier": 0.5,
        "InstrumentType": 2,
        "MMR": 0.01,
        "PipSize": 0.00001,
        "Precision": 3
    })";

    createTestFile("numbers.json", jsonContent);
    std::string filePath = (testDir / "numbers.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_DOUBLE_EQ(info.baseUnitSize, 50000.0);
    EXPECT_DOUBLE_EQ(info.contractMultiplier, 0.5);
    EXPECT_EQ(info.instrumentType, 2);
    EXPECT_DOUBLE_EQ(info.mmr, 0.01);
    EXPECT_DOUBLE_EQ(info.pipSize, 0.00001);
    EXPECT_EQ(info.precision, 3);
}

// Test parsing with scientific notation
TEST_F(SymbolInfoParserTest, ParseScientificNotation) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": 1e5,
        "PipSize": 1e-4,
        "MMR": 2e-2
    })";

    createTestFile("scientific.json", jsonContent);
    std::string filePath = (testDir / "scientific.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_DOUBLE_EQ(info.baseUnitSize, 100000.0);
    EXPECT_DOUBLE_EQ(info.pipSize, 0.0001);
    EXPECT_DOUBLE_EQ(info.mmr, 0.02);
}

// Test parsing with empty string values
TEST_F(SymbolInfoParserTest, ParseEmptyStringValues) {
    std::string jsonContent = R"({
        "Provider": "",
        "ContractCurrency": "",
        "ProfitCurrency": "",
        "Name": "",
        "BaseUnitSize": 100000.0,
        "InstrumentType": 1
    })";

    createTestFile("empty_strings.json", jsonContent);
    std::string filePath = (testDir / "empty_strings.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.provider, "");
    EXPECT_EQ(info.contractCurrency, "");
    EXPECT_EQ(info.profitCurrency, "");
    EXPECT_EQ(info.name, "");
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 100000.0);
    EXPECT_EQ(info.instrumentType, 1);
}

// Test parsing with whitespace in values
TEST_F(SymbolInfoParserTest, ParseWithWhitespace) {
    std::string jsonContent = R"({
        "Provider": "  FXCM  ",
        "Name": "  EURUSD  ",
        "ContractCurrency": " USD ",
        "BaseUnitSize": 100000.0
    })";

    createTestFile("whitespace.json", jsonContent);
    std::string filePath = (testDir / "whitespace.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.provider, "  FXCM  ");
    EXPECT_EQ(info.name, "  EURUSD  ");
    EXPECT_EQ(info.contractCurrency, " USD ");
}

// Test parsing with different field order
TEST_F(SymbolInfoParserTest, ParseDifferentFieldOrder) {
    std::string jsonContent = R"({
        "Name": "USDJPY",
        "Provider": "IC Markets",
        "Precision": 3,
        "PipSize": 0.01,
        "MMR": 0.03,
        "InstrumentType": 1,
        "ContractMultiplier": 1.0,
        "BaseUnitSize": 100000.0,
        "ProfitCurrency": "USD",
        "ContractCurrency": "USD",
        "EndOfHistoryReached": true,
        "WithoutHistory": false,
        "MarginEnabled": true
    })";

    createTestFile("reordered.json", jsonContent);
    std::string filePath = (testDir / "reordered.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.name, "USDJPY");
    EXPECT_EQ(info.provider, "IC Markets");
    EXPECT_EQ(info.precision, 3);
    EXPECT_DOUBLE_EQ(info.pipSize, 0.01);
    EXPECT_DOUBLE_EQ(info.mmr, 0.03);
    EXPECT_EQ(info.instrumentType, 1);
    EXPECT_DOUBLE_EQ(info.contractMultiplier, 1.0);
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 100000.0);
    EXPECT_EQ(info.profitCurrency, "USD");
    EXPECT_EQ(info.contractCurrency, "USD");
    EXPECT_TRUE(info.endOfHistoryReached);
    EXPECT_FALSE(info.withoutHistory);
    EXPECT_TRUE(info.marginEnabled);
}

// Test parsing with extra fields (should ignore them)
TEST_F(SymbolInfoParserTest, ParseWithExtraFields) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": 100000.0,
        "ExtraField1": "ignored",
        "ExtraField2": 123.45,
        "ExtraField3": true,
        "InstrumentType": 1
    })";

    createTestFile("extra_fields.json", jsonContent);
    std::string filePath = (testDir / "extra_fields.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.provider, "Test");
    EXPECT_EQ(info.name, "TEST");
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 100000.0);
    EXPECT_EQ(info.instrumentType, 1);
    // Extra fields should not affect the parsing
}

// Test parsing with nested objects (should handle gracefully)
TEST_F(SymbolInfoParserTest, ParseWithNestedObjects) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": 100000.0,
        "NestedObject": {
            "field1": "value1",
            "field2": 123
        },
        "InstrumentType": 1
    })";

    createTestFile("nested.json", jsonContent);
    std::string filePath = (testDir / "nested.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.provider, "Test");
    EXPECT_EQ(info.name, "TEST");
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 100000.0);
    EXPECT_EQ(info.instrumentType, 1);
}

// Test parsing with arrays (should handle gracefully)
TEST_F(SymbolInfoParserTest, ParseWithArrays) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": 100000.0,
        "ArrayField": ["item1", "item2", "item3"],
        "InstrumentType": 1
    })";

    createTestFile("arrays.json", jsonContent);
    std::string filePath = (testDir / "arrays.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.provider, "Test");
    EXPECT_EQ(info.name, "TEST");
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 100000.0);
    EXPECT_EQ(info.instrumentType, 1);
}

// Test parsing with special characters in strings
TEST_F(SymbolInfoParserTest, ParseWithSpecialCharacters) {
    std::string jsonContent = R"({
        "Provider": "Test-Provider_123",
        "Name": "EUR/USD",
        "ContractCurrency": "USD$",
        "ProfitCurrency": "EUR€",
        "BaseUnitSize": 100000.0
    })";

    createTestFile("special_chars.json", jsonContent);
    std::string filePath = (testDir / "special_chars.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.provider, "Test-Provider_123");
    EXPECT_EQ(info.name, "EUR/USD");
    EXPECT_EQ(info.contractCurrency, "USD$");
    EXPECT_EQ(info.profitCurrency, "EUR€");
}

// Test parsing with very large numbers
TEST_F(SymbolInfoParserTest, ParseLargeNumbers) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": 999999999.99,
        "ContractMultiplier": 0.000001,
        "MMR": 0.0000001,
        "PipSize": 0.00000001,
        "Precision": 8
    })";

    createTestFile("large_numbers.json", jsonContent);
    std::string filePath = (testDir / "large_numbers.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_DOUBLE_EQ(info.baseUnitSize, 999999999.99);
    EXPECT_DOUBLE_EQ(info.contractMultiplier, 0.000001);
    EXPECT_DOUBLE_EQ(info.mmr, 0.0000001);
    EXPECT_DOUBLE_EQ(info.pipSize, 0.00000001);
    EXPECT_EQ(info.precision, 8);
}

// Test parsing with negative numbers
TEST_F(SymbolInfoParserTest, ParseNegativeNumbers) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": -100000.0,
        "ContractMultiplier": -1.0,
        "InstrumentType": -1,
        "MMR": -0.02,
        "PipSize": -0.0001,
        "Precision": -5
    })";

    createTestFile("negative_numbers.json", jsonContent);
    std::string filePath = (testDir / "negative_numbers.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_DOUBLE_EQ(info.baseUnitSize, -100000.0);
    EXPECT_DOUBLE_EQ(info.contractMultiplier, -1.0);
    EXPECT_EQ(info.instrumentType, -1);
    EXPECT_DOUBLE_EQ(info.mmr, -0.02);
    EXPECT_DOUBLE_EQ(info.pipSize, -0.0001);
    EXPECT_EQ(info.precision, -5);
}

// Test parsing with zero values
TEST_F(SymbolInfoParserTest, ParseZeroValues) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": 0,
        "ContractMultiplier": 0.0,
        "InstrumentType": 0,
        "MMR": 0,
        "PipSize": 0.0,
        "Precision": 0
    })";

    createTestFile("zero_values.json", jsonContent);
    std::string filePath = (testDir / "zero_values.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_DOUBLE_EQ(info.baseUnitSize, 0.0);
    EXPECT_DOUBLE_EQ(info.contractMultiplier, 0.0);
    EXPECT_EQ(info.instrumentType, 0);
    EXPECT_DOUBLE_EQ(info.mmr, 0.0);
    EXPECT_DOUBLE_EQ(info.pipSize, 0.0);
    EXPECT_EQ(info.precision, 0);
}

// Test parsing with duplicate keys (should use the last one)
TEST_F(SymbolInfoParserTest, ParseWithDuplicateKeys) {
    std::string jsonContent = R"({
        "Provider": "First",
        "Name": "FIRST",
        "Provider": "Second",
        "Name": "SECOND",
        "BaseUnitSize": 100000.0
    })";

    createTestFile("duplicates.json", jsonContent);
    std::string filePath = (testDir / "duplicates.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    // Should use the last occurrence
    EXPECT_EQ(info.provider, "Second");
    EXPECT_EQ(info.name, "SECOND");
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 100000.0);
}

// Test parsing with very long strings
TEST_F(SymbolInfoParserTest, ParseWithLongStrings) {
    std::string longProvider = std::string(1000, 'A');
    std::string longName = std::string(1000, 'B');
    
    std::string jsonContent = R"({
        "Provider": ")" + longProvider + R"(",
        "Name": ")" + longName + R"(",
        "BaseUnitSize": 100000.0,
        "InstrumentType": 1
    })";

    createTestFile("long_strings.json", jsonContent);
    std::string filePath = (testDir / "long_strings.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.provider, longProvider);
    EXPECT_EQ(info.name, longName);
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 100000.0);
    EXPECT_EQ(info.instrumentType, 1);
}

// Test parsing with very small numbers
TEST_F(SymbolInfoParserTest, ParseWithVerySmallNumbers) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": 0.000000001,
        "ContractMultiplier": 0.000000001,
        "MMR": 0.000000001,
        "PipSize": 0.000000001
    })";

    createTestFile("small_numbers.json", jsonContent);
    std::string filePath = (testDir / "small_numbers.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_DOUBLE_EQ(info.baseUnitSize, 0.000000001);
    EXPECT_DOUBLE_EQ(info.contractMultiplier, 0.000000001);
    EXPECT_DOUBLE_EQ(info.mmr, 0.000000001);
    EXPECT_DOUBLE_EQ(info.pipSize, 0.000000001);
}

// Test parsing with empty JSON object
TEST_F(SymbolInfoParserTest, ParseEmptyJson) {
    std::string jsonContent = R"({})";

    createTestFile("empty.json", jsonContent);
    std::string filePath = (testDir / "empty.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    // All fields should have default values
    EXPECT_EQ(info.provider, "");
    EXPECT_EQ(info.contractCurrency, "");
    EXPECT_EQ(info.profitCurrency, "");
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 0.0);
    EXPECT_DOUBLE_EQ(info.contractMultiplier, 0.0);
    EXPECT_EQ(info.instrumentType, 0);
    EXPECT_DOUBLE_EQ(info.mmr, 0.0);
    EXPECT_DOUBLE_EQ(info.pipSize, 0.0);
    EXPECT_EQ(info.precision, 0);
    EXPECT_EQ(info.name, "");
    EXPECT_FALSE(info.marginEnabled);
    EXPECT_FALSE(info.withoutHistory);
    EXPECT_FALSE(info.endOfHistoryReached);
}

// Test parsing with only whitespace
TEST_F(SymbolInfoParserTest, ParseOnlyWhitespace) {
    std::string jsonContent = R"({   })";

    createTestFile("whitespace_only.json", jsonContent);
    std::string filePath = (testDir / "whitespace_only.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    // All fields should have default values
    EXPECT_EQ(info.provider, "");
    EXPECT_EQ(info.name, "");
    EXPECT_DOUBLE_EQ(info.baseUnitSize, 0.0);
    EXPECT_EQ(info.instrumentType, 0);
}

// Test parsing with invalid numeric values
TEST_F(SymbolInfoParserTest, ParseInvalidNumericValues) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": "invalid",
        "InstrumentType": "not_a_number"
    })";

    createTestFile("invalid_numbers.json", jsonContent);
    std::string filePath = (testDir / "invalid_numbers.json").string();

    // nlohmann/json will throw a type_error when trying to convert string to number
    EXPECT_THROW({
        SymbolInfo info = SymbolInfoParser::parse(filePath);
    }, nlohmann::json::type_error);
}

// Test parsing with very large integer values
TEST_F(SymbolInfoParserTest, ParseLargeIntegerValues) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "InstrumentType": 2147483647,
        "Precision": 2147483647
    })";

    createTestFile("large_integers.json", jsonContent);
    std::string filePath = (testDir / "large_integers.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.instrumentType, 2147483647);
    EXPECT_EQ(info.precision, 2147483647);
}

// Test parsing with very small integer values
TEST_F(SymbolInfoParserTest, ParseSmallIntegerValues) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "InstrumentType": -2147483648,
        "Precision": -2147483648
    })";

    createTestFile("small_integers.json", jsonContent);
    std::string filePath = (testDir / "small_integers.json").string();

    SymbolInfo info = SymbolInfoParser::parse(filePath);

    EXPECT_EQ(info.instrumentType, -2147483648);
    EXPECT_EQ(info.precision, -2147483648);
}

// Test parsing with file not found
TEST_F(SymbolInfoParserTest, ParseFileNotFound) {
    std::string filePath = (testDir / "nonexistent.json").string();
    
    EXPECT_THROW({
        SymbolInfo info = SymbolInfoParser::parse(filePath);
    }, std::runtime_error);
}

// Test parsing with invalid JSON
TEST_F(SymbolInfoParserTest, ParseInvalidJson) {
    std::string jsonContent = R"({
        "Provider": "Test",
        "Name": "TEST",
        "BaseUnitSize": 100000.0,
        "InstrumentType": 1
        // Missing closing brace
    )";

    createTestFile("invalid.json", jsonContent);
    std::string filePath = (testDir / "invalid.json").string();

    EXPECT_THROW({
        SymbolInfo info = SymbolInfoParser::parse(filePath);
    }, nlohmann::json::parse_error);
}
