#pragma once

#include <thread>
#include "../scheduler/FCFSScheduler.hpp"

class InstructionManager {
public:
    InstructionManager(int numCores) : fcfs(numCores) {}
    
    bool processCommand();

private:
    FCFSScheduler fcfs; 
    std::thread schedulerThread;
};