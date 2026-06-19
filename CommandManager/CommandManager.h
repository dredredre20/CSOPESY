#pragma once

#include <thread>
#include "../scheduler/FCFSScheduler.h"

class CommandManager {
public:
    CommandManager(int numCores) : fcfs(numCores) {}  
    
    bool processCommand();

private:
    FCFSScheduler fcfs; // Default to 4 cores
    std::thread schedulerThread;
};