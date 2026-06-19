#include <string>
#include <vector>

#include "../process/process.h"
#include "../scheduler/FCFSScheduler.h"
#include "../Design_Utils/designAssets.h"
#include "CommandManager.h"

bool CommandManager::processCommand() {
    designAssets da;

    std::string command;
    std::cout << "\nEnter a command: ";
    std::getline(std::cin, command);

    if (command == "initialize") {
        std::cout << command << " command recognized. Doing something.";
    }
    else if (command == "screen") {
        std::cout << command << " command recognized. Doing something.";
    }
    else if (command == "scheduler-start") {
        std::cout << command << " command recognized. Doing something.";


    }
    else if (command == "scheduler-test") {
        /* if (timestamp is 0.5 seconds ago) {
            // Create x processes to be added to your scheduler ready queue every 0.5 seconds
        } */
        // Add processes
        for (const auto& process : this->processes) {
            this->fcfsScheduler.addProcess(process);
        }

        // Launch scheduler on a background thread
        /*
        schedulerThread = std::thread([this]() {
            this->fcfsScheduler.runScheduler(std::ref(this->outFile));
        });*/


    }
    else if (command == "scheduler-stop") {
        // std::cout << command << " command recognized. Doing something.";
        // batch_scheduler_enabled = false

        this->fcfsScheduler.stopScheduler();



    }
    else if (command == "screen -s") {
        // Create a process to be added to your scheduler ready queue
        // Format: 
        // running process (date) Core: n something/100
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

