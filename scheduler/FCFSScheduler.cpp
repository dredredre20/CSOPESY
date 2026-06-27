#include "FCFSScheduler.hpp"
#include <thread>
#include <chrono>
#include <mutex>
#include <iostream>

using namespace std;

void FCFSScheduler::runCycle(int coreId) {
    while (running) {
        std::shared_ptr<Process> currentProcess = nullptr;

        {
            lock_guard<mutex> lock(queueMutex);
            // Only pick a process for this core if it is idle
            if (!processQueues[coreId].empty() &&
                runningProcesses.find(coreId) == runningProcesses.end())
            {
                currentProcess = processQueues[coreId].front();
                processQueues[coreId].erase(processQueues[coreId].begin());
            }
        }

        if (currentProcess) {
            currentProcess->setCPUCoreID(coreId);

            {
                lock_guard<mutex> lock(queueMutex);
                runningProcesses[coreId] = currentProcess;
            }

            while (!currentProcess->isFinished()) {
                currentProcess->executeCurrentCommand(coreId);
                currentProcess->moveToNextLine();
                this_thread::sleep_for(chrono::milliseconds(500));
            }

            {
                lock_guard<mutex> lock(queueMutex);
                runningProcesses.erase(coreId);
                finishedProcesses.push_back(currentProcess);
            }
        } else {
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
}