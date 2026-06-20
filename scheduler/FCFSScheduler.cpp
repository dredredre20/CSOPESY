#include "FCFSScheduler.hpp"
#include "../Process/Process.hpp"
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

// Formats and returns the current system timestamp
static string getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm tm = *std::localtime(&t);
    
    ostringstream oss;
    oss << put_time(&tm, "(%m/%d/%Y %I:%M:%S%p)");
    return oss.str();
}

// Distributes incoming processes across the ready queues of the cores
void FCFSScheduler::addProcess(const Process& process) {
    lock_guard<mutex> lock(queueMutex);
    processQueues[nextCore].push_back(process);
    nextCore = (nextCore + 1) % numCores;
}

// Run the scheduler
void FCFSScheduler::runScheduler() {
    running = true;
    vector<std::thread> threads;

    // Create a separate thread loop for each core
    for (int core = 0; core < numCores; ++core) {
        threads.emplace_back([this, core]() {
            while (running) {
                Process* currentProcess = nullptr;

                // Pop the next process from the queue
                { 
                    lock_guard<mutex> lock(queueMutex); 
                    if (!processQueues[core].empty()) {
                        currentProcess = new Process(processQueues[core].front());
                        processQueues[core].erase(processQueues[core].begin());
                    }
                }

                if (currentProcess) {
                    currentProcess->setCPUCoreID(core);

                    // Track process as running 
                    {
                        lock_guard<mutex> lock(queueMutex);
                        runningProcesses[core] = currentProcess;
                    }

                    // Loop through the commands one by one
                    while (!currentProcess->isFinished()) {
                        currentProcess->executeCurrentCommand(core);
                        currentProcess->moveToNextLine();
                        
                        // simulate command execution by adding a 0.5 second delay
                        this_thread::sleep_for(chrono::milliseconds(500)); 
                    }

                    // Remove from active processes and add to finished processes
                    {
                        lock_guard<mutex> lock(queueMutex);
                        runningProcesses.erase(core);         
                        finishedProcesses.push_back(*currentProcess);
                    }
                    delete currentProcess;
                } else {
                    this_thread::sleep_for(chrono::milliseconds(50));
                }
            }
        });
    }

    // Generate 10 dummy processes every 0.5 seconds
    for (int i = 1; i <= 10; i++) {
        if (!running) break; 

        string name = "process" + string(i < 10 ? "0" : "") + to_string(i);
        Process p(i, name); 
        
        // Add 100 PRINT commands to each process
        for (int j = 0; j < 100; j++) {
            p.addCommand(ICommand::PRINT); 
        }

        // Add the process to the scheduler
        this->addProcess(p);

        // Wait for 0.5 seconds before creating the next process 
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    // Block the main thread until all thread cores exit
    for (auto& t : threads) {
        t.join();
    }
}

// Display a snapshot of the running and finished processes
void FCFSScheduler::screenLs() {
    lock_guard<mutex> lock(queueMutex);
    std::string border = "--------------------------------";

    // Print current running processes
    cout << "\n";
    cout << border;
    cout << "\nRunning Processes:\n";
    if (runningProcesses.empty()) {
        cout << "\n";
    } else {
        for (const auto& [core, p] : runningProcesses) {
            cout << p->getName()
                 << getTimestamp() 
                 << " Core:" << core
                 << "   " << p->getCommandCounter()
                 << "/" << p->getLinesOfCode() << "\n";
        }
    }

    // Print history log of finished processes
    cout << "\nFinished Processes:\n";
    if (finishedProcesses.empty()) {
        cout << "\n";
    }
    for (const auto& p : finishedProcesses) {
        cout << p.getName()
             << " (" << p.getCreationTimestamp() << ")"
             << " Finished   "
             << p.getLinesOfCode() << "/" << p.getLinesOfCode() << "\n";
    }
    cout << border;
    cout << "\n";

}


// Stop the scheduler
void FCFSScheduler::stopScheduler() {
    cout << "Stopping scheduler...\n";
    running = false;
}