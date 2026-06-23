#pragma once

#include <thread>
#include "../scheduler/FCFSScheduler.hpp"

class CommandManager {
public:
    CommandManager(int numCores) : fcfs(numCores) {}  
    
    bool processCommand();

private:
    FCFSScheduler fcfs; 
    std::thread schedulerThread;
};