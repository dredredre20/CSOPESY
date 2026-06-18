#include "FCFSScheduler.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

std::atomic<bool> runningMain(true);

class ASCIIart{
    public:
        void printASCIIart() {
            std::cout << "  ____ ____   ___  ____  _____ ______   __\n";
            std::cout << " / ___/ ___| / _ \\|    \\| ____/ ___\\ \\ / /\n";
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
        CommandManager(std::string fileName, std::vector<Process> processes, int numCores)
            : outFile(fileName), processes(processes), fcfsScheduler(numCores) {
                // Correct way to check if file opened successfully
                if (!outFile.is_open()) {
                    // Throwing an exception is the standard way to handle 
                    // failed object construction
                    throw std::runtime_error("Error: Could not create or open the file!");
                }
            }

        ~CommandManager() {
            if (schedulerThread.joinable()) {
                schedulerThread.join(); // Clean up thread on destruction
            }
            if (outFile.is_open()) {
                outFile.close();
            }
        }

        void processCommand() {
            std::string command;
            std::cout <<"\nEnter a command: ";
            std::getline(std::cin, command);

            if (command == "initialize"){
                std::cout << command << " command recognized. Doing something.";
            } else if (command == "screen"){
                std::cout << command << " command recognized. Doing something.";
            } else if (command == "scheduler-start"){
                std::cout << command << " command recognized. Doing something.";
            } else if (command == "scheduler-test") {
                /* if (timestamp is 0.5 seconds ago) {
                    // Create x processes to be added to your scheduler ready queue every 0.5 seconds
                } */
                // Add processes
                for (const auto& process: this->processes) {
                    this->fcfsScheduler.addProcess(process);
                }

                // Launch scheduler on a background thread
                schedulerThread = std::thread([this]() {
                    this->fcfsScheduler.runScheduler(std::ref(this->outFile));
                });
                

            } else if (command == "scheduler-stop"){
                // std::cout << command << " command recognized. Doing something.";
                // batch_scheduler_enabled = false

                this->fcfsScheduler.stopScheduler();
            } else if (command == "screen -s") {
                // Create a process to be added to your scheduler ready queue
                // Format: 
                // running process (date) Core: n something/100
            } else if (command == "report-util"){
                std::cout << command << " command recognized. Doing something.";
            } else if (command == "clear"){
                std::cout << "\033[2J\033[1;1H"; // Clears screen and moves cursor to top-left
                printMenu();
            
            } else if (command == "exit"){
                runningMain = false;
            } else {
                std::cout << "Unknown command. Try again";
            }
        }
    private:
        std::ofstream outFile;
        std::vector<Process> processes;
        FCFSScheduler fcfsScheduler;
        std::thread schedulerThread; // ← add this
};

int main() {
    printMenu();
    
    // given list of processes
    std::vector<Process> processes;
    processes.emplace_back("Process_1", 1, 10);
    processes.emplace_back("Process_2", 2, 10);
    processes.emplace_back("Process_3", 3, 10);
    processes.emplace_back("Process_4", 4, 10);
    processes.emplace_back("Process_5", 5, 10);
    
    // Batch_scheduler_enabled
    CommandManager cmdManager("example.txt", processes, 4);

    // The loop runs until processCommand() returns false
    while (runningMain) {
        // Loop continues
        cmdManager.processCommand();
    }

    return 0;
}