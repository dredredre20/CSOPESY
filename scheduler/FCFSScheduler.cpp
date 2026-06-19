#include "FCFSScheduler.h"
#include "../process/process.h"
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;

// Get current date and time 
static string getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm tm = *std::localtime(&t);
    
    ostringstream oss;
    oss << put_time(&tm, "(%m/%d/%Y %I:%M:%S%p)");
    return oss.str();
}

// Add a process to the scheduler
void FCFSScheduler::addProcess(const Process& process) {
    lock_guard<mutex> lock(queueMutex);
    processQueues[nextCore].push_back(process);
    nextCore = (nextCore + 1) % numCores;
}

// Run the scheduler
void FCFSScheduler::runScheduler() {
    running = true;
    vector<std::thread> threads;

    for (int core = 0; core < numCores; ++core) {
        threads.emplace_back([this, core]() {
            while (running) {
                Process* currentProcess = nullptr;

                { 
                    lock_guard<mutex> lock(queueMutex); 
                    if (!processQueues[core].empty()) {
                        currentProcess = new Process(processQueues[core].front());
                        processQueues[core].erase(processQueues[core].begin());
                    }
                }

                if (currentProcess) {
                    currentProcess->setCPUCoreID(core);

                    // Track as running
                    {
                        lock_guard<mutex> lock(queueMutex);
                        runningProcesses[core] = currentProcess;
                    }

                    while (!currentProcess->isFinished()) {
                        currentProcess->executeCurrentCommand(core);
                        currentProcess->moveToNextLine();
                        this_thread::sleep_for(chrono::milliseconds(500));
                    }

                    {
                        lock_guard<mutex> lock(queueMutex);
                        runningProcesses.erase(core);          // remove from running
                        finishedProcesses.push_back(*currentProcess);
                    }
                    delete currentProcess;
                } else {
                    this_thread::sleep_for(chrono::milliseconds(10));
                }
            }
        });
    }

    for (auto& t : threads) t.join();
}

void FCFSScheduler::screenLs() {
    lock_guard<mutex> lock(queueMutex);

    cout << "\nRunning Processes:\n";
    if (runningProcesses.empty()) {
        cout << "(none)\n";
    } else {
        for (const auto& [core, p] : runningProcesses) {
            cout << p->getName()
                 << " (" << p->getCreationTimestamp() << ")"
                 << " Core:" << core
                 << "   " << p->getCommandCounter()
                 << "/" << p->getLinesOfCode() << "\n";
        }
    }

    cout << "\nFinished Processes:\n";
    if (finishedProcesses.empty()) {
        cout << "(none)\n";
    }
    for (const auto& p : finishedProcesses) {
        cout << p.getName()
             << " (" << p.getCreationTimestamp() << ")"
             << " Finished   "
             << p.getLinesOfCode() << "/" << p.getLinesOfCode() << "\n";
    }
    cout << "\n";
}

void FCFSScheduler::stopScheduler() {
    cout << "Stopping scheduler...\n";
    running = false;
}