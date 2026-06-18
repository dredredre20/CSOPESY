#pragma once
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>

#include "../process/process.h"

// a template for an FCFS scheduler
class FCFSScheduler {
private:
    int numCores;
    std::vector<std::vector<Process>> processQueues; // One queue for each core

public:
    FCFSScheduler(int cores) : numCores(cores), processQueues(cores) {}

    // Add a process to the scheduler
    void addProcess(const Process& process, int core = 0);

    // Sort the process queues based on remaining instructions (FCFS)
    void sortProcessQueues();

    // Run the scheduler
    void runScheduler(std::ofstream& outFile);

    int screenLs();

    void stopScheduler();


};