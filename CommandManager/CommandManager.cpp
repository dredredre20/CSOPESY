#include "CommandManager.h"
#include "../process/process.h"
#include "../Commands/ICommand.hpp"
#include "../Design_Utils/designAssets.h"
#include <iostream>
#include <string>

using namespace std;

bool CommandManager::processCommand() {
    designAssets da;

    string command;
    cout << "\nEnter a command: ";
    getline(std::cin, command);

    if (command == "initialize") {
        // Create 10 processes, each with 100 PRINT commands
        for (int i = 1; i <= 10; i++) {
            string name = "process" + string(i < 10 ? "0" : "") + to_string(i);
            Process p(i, name); // Process(p_id, name)
            
            for (int j = 0; j < 100; j++) {
                p.addCommand(ICommand::PRINT); // 100 commands each
            }

            fcfs.addProcess(p);
        }
        cout << "10 processes initialized with 100 print commands each.\n";
    }

    else if (command == "scheduler-start") {
        schedulerThread = thread(&FCFSScheduler::runScheduler, &fcfs);
        schedulerThread.detach();
        cout << "Scheduler started.\n"; 
    }
    
    else if (command == "scheduler-stop") {
        fcfs.stopScheduler();
    }

    else if (command == "screen -ls") {
        fcfs.screenLs();
    }

    else if (command == "screen") {
        std::cout << command << " command recognized. Doing something.";
    }

    else if (command == "report-util") {
        std::cout << command << " command recognized. Doing something.";
    }

    else if (command == "clear") {
        std::cout << "\033[2J\033[1;1H"; // Clears screen and moves cursor to top-left
        da.welcomeMenu();
    }

    else if (command == "exit") {
        return false;
    }

    else {
        std::cout << "Unknown command. Try again";
    }

    return true;
}