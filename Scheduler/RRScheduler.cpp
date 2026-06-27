#include "RRScheduler.hpp"
#include "../Process/Process.hpp"
#include <thread>
#include <chrono>

using namespace std;

void RRScheduler::runCycle(int coreId) {
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

            // Execute up to timeSlice commands
            for (int tick = 0; tick < timeSlice && !currentProcess->isFinished(); ++tick) {
                
                if (currentProcess->isSleeping()) {
                    uint8_t ticks = currentProcess->getSleepTicks();
                    if (ticks > 0) {
                        currentProcess->setSleepTicks(ticks - 1);
                    }
                    else {
                        currentProcess->wake();
                    }
                    break;
                }
                
                currentProcess->executeCurrentCommand(coreId);
                currentProcess->moveToNextLine();
                this_thread::sleep_for(chrono::milliseconds(config.delayPerExec));
            }

            {
                lock_guard<mutex> lock(queueMutex);
                runningProcesses.erase(coreId);

                if (currentProcess->isFinished()) {
                    finishedProcesses.push_back(currentProcess);
                } else {
                    // Requeue the shared_ptr — no copy/slice of the Process object
                    processQueues[coreId].push_back(currentProcess);
                }
            }
        } else {
            this_thread::sleep_for(chrono::seconds(2));
        }
    }
}