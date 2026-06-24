#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

#include "Scheduler/FCFSScheduler.hpp"
#include "Design_Utils/DesignAssets.hpp"
#include "InstructionManager/InstructionManager.hpp"

int main() {

    bool keepRunning = true;

    designAssets da;
    da.welcomeMenu();
    
    InstructionManager instManager(4); // declare 4 cores

    // The loop runs until processCommand() returns false
    while (keepRunning) {
        // Loop continues
        keepRunning = instManager.processCommand();
    }

    return 0;
}