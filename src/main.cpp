#include <iostream>
#include <string>

int main() {
    std::cout << "Welcome to FXTS2 Mass Backtester Console Application!" << std::endl;
    std::cout << "==================================================" << std::endl;
    
    std::string userInput;
    
    while (true) {
        std::cout << "\nEnter a command (or 'quit' to exit): ";
        std::getline(std::cin, userInput);
        
        if (userInput == "quit" || userInput == "exit") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        
        if (userInput == "help") {
            std::cout << "Available commands:" << std::endl;
            std::cout << "  help    - Show this help message" << std::endl;
            std::cout << "  quit    - Exit the application" << std::endl;
            std::cout << "  exit    - Exit the application" << std::endl;
        } else if (userInput.empty()) {
            continue;
        } else {
            std::cout << "You entered: " << userInput << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
    return 0;
}
