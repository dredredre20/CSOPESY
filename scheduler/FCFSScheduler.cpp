#pragma once
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>
#include <algorithm>

#include "FCFSScheduler.h"
#include "../process/process.h"

std::atomic<bool> running(true);

// a template for an FCFS scheduler

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
        // while (running) {}

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
        // core
        // return core of all processes
        return 1;
    }

    void FCFSScheduler::stopScheduler() {
        std::cout << "Stopping scheduler!" << std::endl;
        running = false;
    }
