#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include "../process/Process.hpp"
#include "../process/ProcessResolver.hpp"

class FCFSScheduler : public ProcessResolver {
private:
    int numCores;
    int nextCore = 0;
    std::vector<std::vector<Process>> processQueues; // One queue for each core
    std::map<int, Process*> runningProcesses;
    std::atomic<bool> running{false};
    std::mutex queueMutex;
    std::vector<Process> finishedProcesses;

    Process* getProcessOnCore(int coreId) override {
        return runningProcesses[coreId];
    }

public:
    FCFSScheduler(int numCores) : numCores(numCores), processQueues(numCores) {}
    Process* findProcessByName(const std::string& name);

    void addProcess(const Process& process);
    void runScheduler();
    void screenLs();
    void stopScheduler();
};