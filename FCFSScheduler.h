#pragma once
#include <vector>
#include "process.h"
#include <thread>
#include <chrono>
#include <fstream>

std::atomic<bool> running(true);

// a template for an FCFS scheduler
class FCFSScheduler {
private:
    int numCores;
    std::vector<std::vector<Process>> processQueues; // One queue for each core

public:
    FCFSScheduler(int cores) : numCores(cores), processQueues(cores) {}

    // Add a process to the scheduler
    void addProcess(const Process& process, int core = 0) {
        if (core >= 0 && core < numCores) {
            processQueues[core].push_back(process);
        } else {
            std::cerr << "Invalid core specified for process addition.\n";
        }
    }

    // Sort the process queues based on remaining instructions (FCFS)
    void sortProcessQueues() {
        for(auto& queue : processQueues) {
            std::sort(queue.begin(), queue.end(), [](const Process& a, const Process& b) {
                return a.getRemainingInstructions() > b.getRemainingInstructions();
            });
        }
    }

    // Run the scheduler
    void runScheduler(std::ofstream& outFile) {
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

    int screenLs() {
        // core
        // return core of all processes
        return 1;
    }

    void stopScheduler() {
        std::cout << "Stopping scheduler!" << std::endl;
        running = false;
    }
};