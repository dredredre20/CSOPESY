#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

#include "Scheduler/FCFSScheduler.hpp"
#include "Design_Utils/DesignAssets.hpp"
#include "CommandManager/CommandManager.hpp"

int main() {

    bool keepRunning = true;

    designAssets da;
    da.welcomeMenu();
    
    CommandManager cmdManager(4); // declare 4 cores

    // The loop runs until processCommand() returns false
    while (keepRunning) {
        // Loop continues
        keepRunning = cmdManager.processCommand();
    }

    return 0;
}