#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

#include "scheduler/FCFSScheduler.h"
#include "Design_Utils/designAssets.h"
#include "CommandManager/CommandManager.h"


// std::atomic<bool> runningMain(true);

int main() {

    bool keepRunning = true;

    designAssets da;
    da.welcomeMenu();
    
    // given list of processes
    //std::vector<Process> processes;
    //processes.emplace_back("Process_2", 2, 10);
    //processes.emplace_back("Process_3", 3, 10);
    //processes.emplace_back("Process_4", 4, 10);
    //processes.emplace_back("Process_5", 5, 10);
    //
    // Batch_scheduler_enabled
    CommandManager cmdManager(4);

    // The loop runs until processCommand() returns false
    while (keepRunning) {
        // Loop continues
        keepRunning = cmdManager.processCommand();
    }

    return 0;
}