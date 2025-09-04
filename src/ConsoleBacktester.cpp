#include "ConsoleBacktester.h"
#include <string>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include "BacktestProject.h"
#include "BacktestProjectSerializer.h"

ConsoleBacktester::ConsoleBacktester(const std::string& pathToBacktester, const string& id) {
    this->pathToBacktester = pathToBacktester;
    this->id = id;
}

void ConsoleBacktester::run(const BacktestProject& project) {
    if (pathToBacktester.empty()) {
        std::cerr << "Error: Path to backtester is not set" << std::endl;
        return;
    }
    
    // Create temporary directory
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "fxts2_backtester";
    std::filesystem::create_directories(tempDir);
    
    // Create project file path in temp directory
    std::filesystem::path projectPath = tempDir / "project" + id + ".bpj";
    
    try {
        BacktestProjectSerializer::serialize(project, projectPath.string());
    } catch (const std::exception& e) {
        std::cerr << "Error saving project: " << e.what() << std::endl;
    }
    
    std::filesystem::path outputPath = tempDir / "backtest" + id + ".txt";
    std::filesystem::path statsPath = tempDir / "stats" + id + ".txt";
    std::filesystem::path backtesterPath = std::filesystem::path(pathToBacktester) / "ConsoleBacktester.exe";
    
    std::string command = backtesterPath.string() + " " + projectPath.string() + " /o " + outputPath.string() + " /so " + statsPath.string();
    std::system(command.c_str());

    // Delete temporary files
    try {
        if (std::filesystem::exists(projectPath)) {
            std::filesystem::remove(projectPath);
        }
        if (std::filesystem::exists(outputPath)) {
            std::filesystem::remove(outputPath);
        }
        if (std::filesystem::exists(statsPath)) {
            std::filesystem::remove(statsPath);
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to clean up temporary files: " << e.what() << std::endl;
    }
}