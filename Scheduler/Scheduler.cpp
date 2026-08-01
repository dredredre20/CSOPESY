#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Scheduler.hpp"
#include <random>
#include <cmath>
#include "../Memory/DemandPagingAllocator.hpp"

using namespace std;

// Picks a random memory size for a process
// within the min/max bounds from config.txt.
size_t Scheduler::randomPowerofTwoMemSize() const {

    int minExp = static_cast<int>(std::log2(config.minMemPerProc));
    int maxExp = static_cast<int>(std::log2(config.maxMemPerProc));

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(minExp, maxExp);

    int exponent = dist(rng);
    return static_cast<size_t>(1) << exponent; // 2^exponent
}

void Scheduler::initialize(const Config& cfg) {
    this->config = cfg;
    processQueues.resize(cfg.numCPU);

    if (cfg.maxOverallMem > 0 && cfg.memPerFrame > 0) {
        memoryAllocator = std::make_unique<DemandPagingAllocator>(cfg.maxOverallMem, cfg.memPerFrame);
    } else {
        memoryAllocator.reset();
    }
}

void Scheduler::addProcess(std::shared_ptr<Process> process) {
    lock_guard<mutex> lock(queueMutex);
    if (process) {
        size_t memSize = randomPowerofTwoMemSize();
        process->setMemoryRequirement(memSize);
    }
    processQueues[nextCore].push_back(process);
    nextCore = (nextCore + 1) % config.numCPU;
}

void Scheduler::start() {
    running = true;
    for (int i = 0; i < config.numCPU; ++i) {
        cpuThreads.emplace_back(&Scheduler::runCycle, this, i);
    }
}

void Scheduler::stop() {
    running = false;
    for (auto& t : cpuThreads)
        if (t.joinable()) t.join();
    cpuThreads.clear();
}

string Scheduler::getTimestamp() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm tm = *std::localtime(&t);

    ostringstream oss;
    oss << put_time(&tm, "(%m/%d/%Y %I:%M:%S%p)");
    return oss.str();
}

// Tries to give a process its memory allocation (virtual address space
// registration
bool Scheduler::tryAdmitProcess(const std::shared_ptr<Process>& process) {
    if (!process || !memoryAllocator) {
        return true;
    }

    if (process->isMemoryAllocated()) {
        return true; 
    }

    if (process->getMemoryRequirement() <= 0) {
        return true;
    }

    void* block = nullptr;
    if (auto* dpAllocator = dynamic_cast<DemandPagingAllocator*>(memoryAllocator.get())) {
        block = dpAllocator->allocate(process);
    } 

    if (block == nullptr) {
        return false;
    }

    process->setMemoryAllocatedBlock(block);
    memoryResidentProcesses.push_back(process);
    return true;
}

void Scheduler::releaseProcessMemory(const std::shared_ptr<Process>& process) {
    if (!process || !process->isMemoryAllocated() || !memoryAllocator) {
        return;
    }

    memoryAllocator->deallocate(process->getMemoryAllocatedBlock());
    process->clearMemoryAllocation();

    memoryResidentProcesses.erase(
        std::remove_if(memoryResidentProcesses.begin(), memoryResidentProcesses.end(),
            [&](const std::shared_ptr<Process>& p) { return p == process; }),
        memoryResidentProcesses.end());
}


void Scheduler::screenLs() {
    lock_guard<mutex> lock(queueMutex);

    string border = "================================";
    string timestamp = getTimestamp();

    int coresUsed = static_cast<int>(runningProcesses.size());
    int coresAvailable = config.numCPU - coresUsed;
    double cpuUtilization = (coresUsed / config.numCPU) * 100.0;

    cout << "\n" << border << "\n";
    cout << "SYSTEM REPORT " << timestamp << "\n";
    cout << border << "\n";
    cout << "Cores Available: " << coresAvailable << "\n";
    cout << "Cores Used: " << coresUsed << "\n";
    cout << "CPU Utilization: " << fixed << std::setprecision(2) << cpuUtilization << "%\n";
    cout << "\n";

    cout << "Running Processes:\n";
    if (runningProcesses.empty()) {
        cout << "  None\n";
    } else {
        for (const auto& [core, p] : runningProcesses) {
            cout  << "  " << p->getName()
                  << " " << getTimestamp()
                  << " Core:" << core
                  << "   " << p->getCommandCounter()
                  << "/" << p->getLinesOfCode() << "\n";
        }
    }

    cout << "\n";

    cout << "Finished Processes:\n";
    if (finishedProcesses.empty()) {
        cout << "  None\n";
    } else {
        for (const auto& p : finishedProcesses) {
            cout << "  " << p->getName()
                 << " (" << p->getCreationTimestamp() << ")"
                 << " Finished   "
                 << p->getLinesOfCode() << "/" << p->getLinesOfCode() << "\n";
        }
    }

    cout << border << "\n";
}

std::shared_ptr<Process> Scheduler::findProcessByName(const std::string& name) {
    lock_guard<mutex> lock(queueMutex);

    for (auto& queue : processQueues) {
        for (auto& process : queue) {
            if (process->getName() == name) {
                return process;
            }
        }
    }

    for (const auto& [core, process] : runningProcesses) {
        if (process->getName() == name) return process;
    }
    return nullptr;
}

void Scheduler::reportUtil() {
    lock_guard<mutex> lock(queueMutex);
    ofstream logFile("csopesy-log.txt", ios::app);

    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open csopesy-log.txt for writing.\n";
        return;
    }

    string border = "================================";
    string timestamp = getTimestamp();

    int coresUsed = static_cast<int>(runningProcesses.size());
    int coresAvailable = config.numCPU - coresUsed;
    double cpuUtilization = (coresUsed / config.numCPU) * 100.0;

    logFile << "\n" << border << "\n";
    logFile << "SYSTEM REPORT " << timestamp << "\n";
    logFile << border << "\n";
    logFile << "Cores Available: " << coresAvailable << "\n";
    logFile << "Cores Used: " << coresUsed << "\n";
    logFile << "CPU Utilization: " << fixed << std::setprecision(2) << cpuUtilization << "%\n";
    logFile << "\n";

    logFile << "Running Processes:\n";
    if (runningProcesses.empty()) {
        logFile << "  None\n";
    } else {
        for (const auto& [core, p] : runningProcesses) {
            logFile << "  " << p->getName()
                    << " " << getTimestamp()
                    << " Core:" << core
                    << "   " << p->getCommandCounter()
                    << "/" << p->getLinesOfCode() << "\n";
        }
    }

    logFile << "\n";

    logFile << "Finished Processes:\n";
    if (finishedProcesses.empty()) {
        logFile << "  None\n";
    } else {
        for (const auto& p : finishedProcesses) {
            logFile << "  " << p->getName()
                    << " (" << p->getCreationTimestamp() << ")"
                    << " Finished   "
                    << p->getLinesOfCode() << "/" << p->getLinesOfCode() << "\n";
        }
    }

    logFile << border << "\n";
    logFile.close();

    std::cout << "Report saved to csopesy-log.txt\n";
}

// For debugging
void Scheduler::visualizeHighLevelMemory() {
    if (memoryAllocator) {
        std::string memVisual;
        memVisual = memoryAllocator->visualizeHighLevelMemory();

        std::cout << memVisual << std::endl;
    } else {
        std::cout << "No memory allocator initialized.\n";
    }
}

// For debugging
void Scheduler::visualizeDetailedMemory() {
    if (memoryAllocator) {
        std::string memVisual;
        memVisual = memoryAllocator->visualizeDetailedMemory(this->activeCPUTicks, this->idleCPUTicks);

        std::cout << memVisual << std::endl;
    } else {
        std::cout << "No memory allocator initialized.\n";
    }
}