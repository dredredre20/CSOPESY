#include "CommandManager.hpp"
#include "../process/process.hpp"
#include "../Commands/ICommand.hpp"
#include "../Design_Utils/DesignAssets.hpp"
#include "../Config.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

static bool initialized = false;
static Config config;

bool CommandManager::processCommand() {
    designAssets da;

    string command;
    cout << "\nEnter a command: ";
    getline(std::cin, command);

    if (command == "initialize") { // Read config.txt file
		ifstream file("config.txt");
		if (!file.is_open()) {
			cerr << "Error: Could not open config.txt" << endl;
			return true;
		}

        string key;
		while (file >> key) {
			if (key == "num-cpu") {
				file >> config.numCPU;
			}
			else if (key == "scheduler") {
				file >> config.scheduler;
			}
			else if (key == "quantum-cycles") {
				file >> config.quantumCycles;
			}
			else if (key == "batch-process-freq") {
				file >> config.batchFreq;
			}
			else if (key == "min-ins") {
				file >> config.minIns;
			}
			else if (key == "max-ins") {
				file >> config.maxIns;
			}
			else if (key == "delay-per-exec") {
				file >> config.delayPerExec;
			}
		}

        file.close();
        initialized = true;
        cout << "Successfully initialized." << "\n";

        // TODO: Pass config instance to Scheduler;
    }

    if (!initialized) {
        cout << "Please run 'initialize' first.\n";
        return true;
    }

	// TODO: Create Scheduler class to return the correct scheduler based on config.scheduler
    if (command == "scheduler-start") {
        // Run the main scheduler loop on a separate background thread
        schedulerThread = thread(&FCFSScheduler::runScheduler, &fcfs);
        schedulerThread.detach();
        cout << "Scheduler started.\n"; 
    }
    
    else if (command == "scheduler-stop") {
        fcfs.stopScheduler();
    }

    // TODO: Add commands screen -s -> process-smi, screen -r
    else if (command == "screen -ls") {
        fcfs.screenLs();
    }

    // TODO: Implementation for report-util
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