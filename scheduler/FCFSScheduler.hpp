#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include "../Process/Process.hpp"

class FCFSScheduler {
private:
    int numCores;
    int nextCore = 0;
    std::vector<std::vector<Process>> processQueues; // One queue for each core
    std::map<int, Process*> runningProcesses;
    std::atomic<bool> running{false};
    std::mutex queueMutex;
    std::vector<Process> finishedProcesses;

public:
    FCFSScheduler(int numCores) : numCores(numCores), processQueues(numCores) {}

    void addProcess(const Process& process);
    void runScheduler();
    void screenLs();
    void stopScheduler();
};