#include <string>
#include <vector>
#include <thread>

#include "../process/process.h"
#include "../scheduler/FCFSScheduler.h"
#include "../Design_Utils/designAssets.h"
#include "CommandManager.h"

using namespace std;

bool CommandManager::processCommand() {
    //FCFSScheduler fcfs(4);
    designAssets da;
    //thread schedulerThread;

    string command;
    cout << "\nEnter a command: ";
    getline(std::cin, command);

    if (command == "initialize") {
        
		// create processes and add them to the the ready queue of the scheduler
        for (int i = 0; i < 10; i++) {
			string name = "screen_0" + to_string(i);
			Process p(name, i + 1, 100); 
			fcfs.addProcess(p, i % 4); // distributing across cores
        }

        cout << "10 Processes added." << endl;
    }
    else if (command == "screen") {
        std::cout << command << " command recognized. Doing something.";
    }
    else if (command == "scheduler-start") {
        // starte scheduler in a separate thread to allow concurrent command processing
        schedulerThread = thread(&FCFSScheduler::runScheduler, &fcfs);
		schedulerThread.detach(); // detach the thread to allow it to run independently

    }
    else if (command == "scheduler-stop") {
        fcfs.stopScheduler();
    }
    else if (command == "screen -ls") {
        fcfs.screenLs();
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

