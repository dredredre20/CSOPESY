#include "CommandManager.hpp"
#include "../Process/Process.hpp"
#include "../Commands/ICommand.hpp"
#include "../Design_Utils/DesignAssets.hpp"
#include <iostream>
#include <string>

using namespace std;

bool CommandManager::processCommand() {
    designAssets da;

    string command;
    cout << "\nEnter a command: ";
    getline(std::cin, command);

    if (command == "initialize") {
         std::cout << command << " command recognized. Doing something.";
    }

    else if (command == "scheduler-start") {
        // Run the main scheduler loop on a separate background thread
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