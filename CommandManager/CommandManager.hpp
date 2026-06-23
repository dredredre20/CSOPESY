#pragma once

#include <thread>
#include "../Scheduler/FCFSScheduler.hpp"

class CommandManager {
public:
    CommandManager(int numCores) : fcfs(numCores) {}  
    
    bool processCommand();

private:
    FCFSScheduler fcfs; 
    std::thread schedulerThread;
};