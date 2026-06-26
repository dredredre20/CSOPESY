#include "RRScheduler.hpp"
#include "../process/Process.hpp"
#include <thread>
#include <chrono>

using namespace std;

/*
Process* RRScheduler::getProcessOnCore(int coreId) {
    lock_guard<mutex> lock(queueMutex);
    auto it = runningProcesses.find(coreId);
    if (it != runningProcesses.end()) {
        return it->second;
    }
    return nullptr;
}
*/

void RRScheduler::runCycle() {
    while (running) {
        Process* currentProcess = nullptr;
        int assignedCore = -1;

        {
            lock_guard<mutex> lock(queueMutex);
            for (int core = 0; core < config.numCPU; ++core) {
                if (!processQueues[core].empty() && runningProcesses.find(core) == runningProcesses.end()) {
                    currentProcess = new Process(processQueues[core].front());
                    processQueues[core].erase(processQueues[core].begin());
                    assignedCore = core;
                    break;
                }
            }
        }

        if (currentProcess) {
            currentProcess->setCPUCoreID(assignedCore);

            {
                lock_guard<mutex> lock(queueMutex);
                runningProcesses[assignedCore] = currentProcess;
            }

            // Execute for up to timeSlice commands
            for (int tick = 0; tick < timeSlice && !currentProcess->isFinished(); ++tick) {
                currentProcess->executeCurrentCommand(assignedCore);
                currentProcess->moveToNextLine();
                this_thread::sleep_for(chrono::milliseconds(500));
            }

            {
                lock_guard<mutex> lock(queueMutex);
                runningProcesses.erase(assignedCore);

                // If finished, remove froma active processes
                if (currentProcess->isFinished()) {
                    finishedProcesses.push_back(*currentProcess);
                    delete currentProcess;
                }
                else {
                    // Requeue the process for the next time slice
                    processQueues[assignedCore].push_back(*currentProcess);
                    delete currentProcess;
                }
            }
        }
        else {
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
}