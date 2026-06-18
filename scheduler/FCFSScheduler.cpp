#pragma once
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <sstream>
#include <iomanip>

#include "FCFSScheduler.h"
#include "../process/process.h"

using namespace std;

atomic<bool> running(true);
mutex queueMutex;
vector<Process> finishedProcesses;
vector<Process> runninggProcesses;


// get current date and time 
static string getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm tm = *std::localtime(&t);
    ostringstream oss;
    oss << put_time(&tm, "(%m/%d/%Y %I:%M:%S%p)");
    return oss.str();
}

 // Add a process to the scheduler
void FCFSScheduler::addProcess(const Process& process, int core) {
    if (core >= 0 && core < numCores) {
        processQueues[core].push_back(process);
    }
    else {
        std::cerr << "Invalid core specified for process addition.\n";
    }
}

// Sort the process queues based on remaining instructions (FCFS)
void FCFSScheduler::sortProcessQueues() {
    for (auto& queue : processQueues) {
        std::sort(queue.begin(), queue.end(), [](const Process& a, const Process& b) {
            return a.getRemainingInstructions() > b.getRemainingInstructions();
            });
    }
}

// Run the scheduler
void FCFSScheduler::runScheduler(std::ofstream& outFile) {

    while (running) {
        for (int core = 0; core < numCores; ++core) {
            if (!processQueues[core].empty()) {
                std::cout << "Core " << core << "!" << std::endl;
                Process currentProcess = processQueues[core].back();
                processQueues[core].pop_back();

                while (!currentProcess.hasFinished()) {
                    currentProcess.executeInstruction(outFile);
                }

                outFile << "Process " << currentProcess.getID() << " completed on Core " << core + 1 << "." << std::endl;
                // std::cout << "Process " << currentProcess.getRemainingInstructions() << " completed on Core " << core + 1 << "." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    }
}

int FCFSScheduler::screenLs() {
    lock_guard<mutex> lock(queueMutex); // lock the mutex for safe accessing of process queue
     
    cout << "\nRunning Processes:\n";

	// Display processes currently running on each core
	for (int core = 0; core < numCores; ++core) {
		if (!processQueues[core].empty()) {
			for (const auto& p : processQueues[core]) {
                std::cout << p.getName() << " " << getTimestamp() << "  Core: " << core
                    << "  " << p.getRemainingInstructions() << " remaining\n";
			}
			cout << endl;
		}
	}

    cout << "\nFinished Processes:\n";
    for (auto& p : finishedProcesses) {
        cout << p.getName() << " " << getTimestamp() << " " << p.getRemainingInstructions() << " Finished \n";
    }

    return finishedProcesses.size();
}

void FCFSScheduler::stopScheduler() {
    std::cout << "Stopping scheduler!" << std::endl;
    running = false;
}
