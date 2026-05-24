#include <iostream>
#include <cstring>
#include <string>

class ASCIIart{
    public:
        void printASCIIart(){
            char buffer[5][45];
            strcpy(buffer[0], "  ____ ____   ___  ____  _____ ______   __");
            strcpy(buffer[1], " / ___/ ___| / _ \\|    \\| ____/ ___\\ | / /");
            strcpy(buffer[2], "| |   \\___ \\| | | | |_) |  _| \\___ \\\\ V /");
            strcpy(buffer[3], "| |___ ___) | |_| |  __/| |___ ___) || |  ");
            strcpy(buffer[4], " \\____|____/ \\___/|_|   |_____|____/ |_|  ");

            for (int i = 0; i < 5; i++) {
                printf("%s\n", buffer[i]);
            }
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
                std::cout << "\033[2J\033[1;1H";    
                printMenu();
            
            } else if (command == "exit"){
                flag = false;
                exit(0);
            } else {
                std::cout << "Unknown command. Try again";
            }

            return flag;
        }
};

int main(){
    printMenu();
    CommandManager cmdManager;

    while (cmdManager.processCommand()){

    }
}