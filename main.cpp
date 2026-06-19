#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

#include "scheduler/FCFSScheduler.h"
#include "Design_Utils/designAssets.h"
#include "CommandManager/CommandManager.h"

int main() {

    bool keepRunning = true;

    designAssets da;
    da.welcomeMenu();
    
    CommandManager cmdManager(4);

    // The loop runs until processCommand() returns false
    while (keepRunning) {
        // Loop continues
        keepRunning = cmdManager.processCommand();
    }

    return 0;
}