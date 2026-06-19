#include <string>
#include <vector>

#include "../process/process.h"
#include "../scheduler/FCFSScheduler.h"


class CommandManager {

public:
    CommandManager(std::string fileName, std::vector<Process> processes, int numCores)
        : outFile(fileName), processes(processes), fcfsScheduler(numCores) {
        // Correct way to check if file opened successfully
        if (!outFile.is_open()) {
            // Throwing an exception is the standard way to handle 
            // failed object construction
            throw std::runtime_error("Error: Could not create or open the file!");
        }
    }

    ~CommandManager() {
        if (schedulerThread.joinable()) {
            schedulerThread.join(); // Clean up thread on destruction
        }
        if (outFile.is_open()) {
            outFile.close();
        }
    }

    bool processCommand();


private:
    std::ofstream outFile;
    std::vector<Process> processes;
    FCFSScheduler fcfsScheduler;
    std::thread schedulerThread; // ← add this
};
