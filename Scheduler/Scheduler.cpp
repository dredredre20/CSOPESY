#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Scheduler.hpp"
#include "../Memory/FlatMemoryAllocator.hpp"

using namespace std;

void Scheduler::initialize(const Config& cfg) {
    this->config = cfg;
    processQueues.resize(cfg.numCPU);

    if (cfg.maxOverallMem > 0 && cfg.memPerProc > 0) {
        memoryAllocator = std::make_unique<FlatMemoryAllocator>(cfg.maxOverallMem, cfg.memPerProc);
    } else {
        memoryAllocator.reset();
    }
}

void Scheduler::addProcess(std::shared_ptr<Process> process) {
    lock_guard<mutex> lock(queueMutex);
    if (process) {
        process->setMemoryRequirement(config.memPerProc > 0 ? static_cast<size_t>(config.memPerProc) : 0);
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

    void* block = memoryAllocator->allocate(process->getMemoryRequirement());
    if (block == nullptr) {
        // std::cout << "Process " << process->getName() << " cannot be admitted because memory is full. It will be requeued.\n";
        return false;
    }

    process->setMemoryAllocatedBlock(block);
    return true;
}

void Scheduler::releaseProcessMemory(const std::shared_ptr<Process>& process) {
    if (!process || !process->isMemoryAllocated() || !memoryAllocator) {
        return;
    }

    memoryAllocator->deallocate(process->getMemoryAllocatedBlock());
    process->clearMemoryAllocation();
}

void Scheduler::writeQuantumReport() {
    lock_guard<mutex> lock(queueMutex);

    std::ostringstream filename;
    filename << "memory_stamp_" << std::setw(3) << std::setfill('0') << quantumReportCounter++ << ".txt";

    std::ofstream report(filename.str());
    if (!report.is_open()) {
        std::cerr << "Unable to write memory report: " << filename.str() << "\n";
        return;
    }

    const size_t totalMemory = memoryAllocator ? memoryAllocator->getCapacity() : 0;
    size_t allocatedMemory = 0;

    struct MemoryBlockView {
        size_t start = 0;
        size_t size = 0;
        std::string name;
    };

    std::vector<MemoryBlockView> blocks;

    auto addAllocatedProcess = [&](const std::shared_ptr<Process>& process) {
        if (!process || !process->isMemoryAllocated() || !memoryAllocator) {
            return;
        }

        size_t start = memoryAllocator->getAllocationStart(process->getMemoryAllocatedBlock());
        if (start == std::numeric_limits<size_t>::max()) {
            return;
        }

        allocatedMemory += process->getMemoryRequirement();
        blocks.push_back({start, process->getMemoryRequirement(), process->getName()});
    };

    for (const auto& [core, process] : runningProcesses) {
        addAllocatedProcess(process);
    }

    for (const auto& queue : processQueues) {
        for (const auto& process : queue) {
            addAllocatedProcess(process);
        }
    }

    std::sort(blocks.begin(), blocks.end(), [](const MemoryBlockView& a, const MemoryBlockView& b) {
        return a.start > b.start;
    });

    report << "Timestamp: " << getTimestamp() << "\n";
    report << "\nTotal external fragmentation in KB: " << (totalMemory > allocatedMemory ? totalMemory - allocatedMemory : 0)
           << "\n";
    report << "----end---- = " << totalMemory << "\n\n";

    for (const auto& block : blocks) {
        const size_t upperLimit = block.start + block.size;
        const size_t lowerLimit = block.start;

        report << upperLimit << "\n";
        report << block.name << "\n";
        report << lowerLimit << "\n\n";
    }

    report << "----start----- = 0\n";

    report.close();
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

    // Search the RQ
	for (auto& queue : processQueues) {
        for (auto& process : queue) {
            if (process->getName() == name) {
                return process; 
            }
        }
    }

    // Search the running processes
    for (const auto& [core, process] : runningProcesses) {
            if (process->getName() == name) return process;
            
    }    return nullptr;
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