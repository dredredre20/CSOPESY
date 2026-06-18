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
void FCFSScheduler::runScheduler() {
    std::vector<std::thread> threads;

    for (int core = 0; core < numCores; ++core) {

		// at each core, create a thread that continuously checks for processes in its queue and executes them
        threads.emplace_back([this, core]() {

            while (running) {
                Process* currentProcess = nullptr;

                { // lock queue for safe access 
                    std::lock_guard<std::mutex> lock(queueMutex);
                    // check if queue has processes, if there is, execute 
                    if (!processQueues[core].empty()) {
                        currentProcess = &processQueues[core].back();
                        processQueues[core].pop_back();
                    }
                }

                if (currentProcess) {
                    // create a log file for the current process
                    ofstream logFile(currentProcess->getName() + ".txt");
                    logFile << "Process name: " << currentProcess->getName() << "\nLogs:\n\n";

                    // execute process until finished, log each instruction execution
                    while (!currentProcess->hasFinished()) {
                        std::string ts = getTimestamp();
                        currentProcess->executeInstruction();
                        logFile << ts << " Core:" << core
                            << " \"Hello world from " << currentProcess->getName() << "!\"\n";
                    }

                    {   // store finished process
                        std::lock_guard<std::mutex> lock(queueMutex);
                        finishedProcesses.push_back(*currentProcess);
                    }
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            });
    }

    for (auto& t : threads) t.join(); // join all threads after finishing
    
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
