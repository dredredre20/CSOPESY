#include "RRScheduler.hpp"
#include "../Process/Process.hpp"
#include "../Memory/MemoryManager.hpp"
#include <thread>
#include <chrono>

using namespace std;

void RRScheduler::runCycle(int coreId) {
    while (running) {
        std::shared_ptr<Process> currentProcess = nullptr;

        {
            lock_guard<mutex> lock(queueMutex);
            // Only pick a process for this core if it is idle
            if (runningProcesses.find(coreId) == runningProcesses.end()) {
                size_t attempts = processQueues[coreId].size();
                bool memoryKnownFull = false;

                for (size_t i = 0; i < attempts; ++i) {
                    auto candidate = processQueues[coreId].front();
                    processQueues[coreId].erase(processQueues[coreId].begin());

                    if (candidate->isMemoryAllocated()) {
                        currentProcess = candidate;
                        break;
                    } 
                    
                    if (memoryKnownFull) {
                        processQueues[coreId].push_back(candidate);
                        continue;
                    }

                    if (MemoryManager::getInstance()->tryAdmitProcess(candidate)) {
                        currentProcess = candidate;
                        break;
                    } else {
                        memoryKnownFull = true;
                        processQueues[coreId].push_back(candidate);
                    }
                }
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
                        currentProcess->moveToNextLine();
                    }
                    this->idleCPUTicks++;
                    this_thread::sleep_for(chrono::milliseconds(config.delayPerExec));
                    continue;
                }
                
                currentProcess->executeCurrentCommand(coreId);
                currentProcess->moveToNextLine();
                this->activeCPUTicks++;
                this_thread::sleep_for(chrono::milliseconds(config.delayPerExec));
            }


            {
                lock_guard<mutex> lock(queueMutex);
                runningProcesses.erase(coreId);

                if (currentProcess->isFinished()) {
                    MemoryManager::getInstance()->releaseProcessMemory(currentProcess);
                    finishedProcesses.push_back(currentProcess);
                } else {
                    processQueues[coreId].push_back(currentProcess);
                }
            }
        } else {
            // idle tick
            this->idleCPUTicks++;

            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}