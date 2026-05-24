#include <iostream>
#include <string>

class ASCIIart{
    public:
        void printASCIIart() {
            std::cout << "  ____ ____   ___  ____  _____ ______   __\n";
            std::cout << " / ___/ ___| / _ \\|    \\| ____/ ___\\ | / /\n";
            std::cout << "| |   \\___ \\| | | | |_) |  _| \\___ \\\\ V / \n";
            std::cout << "| |___ ___) | |_| |  __/| |___ ___) || |  \n";
            std::cout << " \\____|____/ \\___/|_|   |_____|____/ |_|  \n";
        }
};

class MainMenu{
    public:
        void welcomeMenu(){
            ASCIIart ascii;
            ascii.printASCIIart();
            std::cout << "\nHello, Welcome to CSOPESY commandline!";
            std::cout << "\nType 'exit' to quit, 'clear' to clear screen";
            std::cout << "\n\n** IMPORTANT: Type 'initialize' to load config and start system **\n";
        } 
};

// Global function to print menu
void printMenu(){
    MainMenu menu;
    menu.welcomeMenu();
}

class CommandManager{

    public:
        bool processCommand(){
            bool flag = true;

            std::string command;
            std::cout <<"\nEnter a command: ";
            std::getline(std::cin, command);

            if (command == "initialize"){
                std::cout << command << " command recognized. Doing something.";
            } else if (command == "screen"){
                std::cout << command << " command recognized. Doing something.";
            } else if (command == "scheduler-start"){
                std::cout << command << " command recognized. Doing something.";
            } else if (command == "scheduler-stop"){
                std::cout << command << " command recognized. Doing something.";
            } else if (command == "report-util"){
                std::cout << command << " command recognized. Doing something.";
            } else if (command == "clear"){
                std::cout << "\033[2J\033[1;1H"; // Clears screen and moves cursor to top-left
                printMenu();
            
            } else if (command == "exit"){
                flag = false;
            } else {
                std::cout << "Unknown command. Try again";
            }

            return flag;
        }
};

int main() {
    printMenu();
    CommandManager cmdManager;

    // The loop runs until processCommand() returns false
    while (cmdManager.processCommand()) {
        // Loop continues
    }

    return 0;
}