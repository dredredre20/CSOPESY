#include <string>
#include <vector>

#include "../process/process.h"
#include "../scheduler/FCFSScheduler.h"


class CommandManager {

public:
    CommandManager(int numCores) : fcfs(numCores) {}

    ~CommandManager() {
        if (schedulerThread.joinable()) {
            fcfs.stopScheduler();
            schedulerThread.join(); // Clean up thread on destruction
        }
    }

    // lambda function to start scheduler in separate thread
    void startScheduler() {
        schedulerThread = std::thread([this]() {
            fcfs.runScheduler();
        });
    }

    bool processCommand();


private:
    FCFSScheduler fcfs;
    std::thread schedulerThread; 
};
