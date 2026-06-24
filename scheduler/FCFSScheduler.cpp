#include "FCFSScheduler.hpp"
#include "../process/Process.hpp"
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

void FCFSScheduler::runCycle() {
    while (running) {
        Process* currentProcess = nullptr;
        int assignedCore = -1;

        {
        // Pop the next process from the queue
            lock_guard<mutex> lock(queueMutex);
            for (int core = 0; core < config.numCPU; ++core) {
                if (!processQueues[core].empty() && runningProcesses.find(core) == runningProcesses.end()) 
                {
                    currentProcess = new Process(processQueues[core].front());
                    processQueues[core].erase(processQueues[core].begin());
                    assignedCore = core;
                    break;
                }
            }
        }

        if (currentProcess) {
            currentProcess->setCPUCoreID(assignedCore);

            // Track process as running 
            {
                lock_guard<mutex> lock(queueMutex);
                runningProcesses[assignedCore] = currentProcess;
            }

            // Loop through the commands one by one
            while (!currentProcess->isFinished()) {
                currentProcess->executeCurrentCommand(assignedCore);
                currentProcess->moveToNextLine();

                // simulate command execution by adding a 0.5 second delay
                this_thread::sleep_for(chrono::milliseconds(500));
            }

            // Remove from active processes and add to finished processes
            {
                lock_guard<mutex> lock(queueMutex);
                runningProcesses.erase(assignedCore);
                finishedProcesses.push_back(*currentProcess);
            }
            delete currentProcess;
        }
        else {
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
}