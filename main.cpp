#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include "ConsoleUI/ConsoleManager.hpp"
#include "Scheduler/FCFSScheduler.hpp"
#include "Design_Utils/DesignAssets.hpp"

int main() {
    ConsoleManager::initialize();
    bool keepRunning = true;
    ConsoleManager* console_manager = ConsoleManager::getInstance();

    // The loop runs until keepRunning returns false
    while (keepRunning) {
        console_manager -> process();

        keepRunning = ConsoleManager::getInstance() -> isRunning();
    }

    ConsoleManager::destroy(); // Destroy console manager instance before exiting

    return 0;
}