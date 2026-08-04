#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Scheduler.hpp"
#include "../Memory/MemoryManager.hpp"
#include <random>
#include <cmath>
#include <optional>

using namespace std;

void Scheduler::initialize(const Config& cfg) {
    this->config = cfg;
    processQueues.resize(cfg.numCPU);
}

void Scheduler::addProcess(std::shared_ptr<Process> process) {
    lock_guard<mutex> lock(queueMutex);
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

void Scheduler::screenLs() {
    lock_guard<mutex> lock(queueMutex);

    string border = "================================";
    string timestamp = getTimestamp();

    int coresUsed = static_cast<int>(runningProcesses.size());
    int coresAvailable = config.numCPU - coresUsed;
    double cpuUtilization = (static_cast<double>(coresUsed) / config.numCPU) * 100.0;

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

void Scheduler::processSMI() {
    {
        lock_guard<mutex> lock(queueMutex);

        int coresUsed = static_cast<int>(runningProcesses.size());
        double cpuUtilization = (static_cast<double>(coresUsed) / config.numCPU) * 100.0;

        auto snap = MemoryManager::getInstance()->getMemorySnapshot();
        long memUtil = std::lround((static_cast<double>(snap.allocatedSize) / snap.capacitySize) * 100.0);

        std::ostringstream oss;
        oss << "\n--------------------------------------------------\n";
        oss << "| PROCESS-SMI V01.00 Driver Version: 01.00 |\n";
        oss << "--------------------------------------------------\n";
        oss << "CPU-Util: " << fixed << setprecision(2) << cpuUtilization << "%\n";
        oss << "Memory Usage: " << snap.allocatedSize << "MiB / " << snap.capacitySize << "MiB\n";
        oss << "Memory Util: " << memUtil << "%\n\n";
        oss << "==================================================\n";
        oss << "Running processes and memory usage:\n";
        oss << "--------------------------------------------------\n";

        for (const auto& [name, bytes] : snap.processResidentBytes) {
            if (bytes > 0)
                oss << name << " " << bytes << "MiB\n";
        }
        oss << "--------------------------------------------------\n";

        cout << oss.str();
    }
}

void Scheduler::vmstat() {
    {
        lock_guard<mutex> lock(queueMutex);

        auto snap = MemoryManager::getInstance()->getMemorySnapshot();

        std::ostringstream oss;
        oss << std::endl;
        oss << snap.capacitySize << "\t"/*  */ << "total memory\n";
        oss << snap.allocatedSize << "\t"/*  */ << "used memory\n";
        oss << snap.capacitySize - snap.allocatedSize << "\t"/*  */ << "free memory\n";
        oss << this->idleCPUTicks << "\t"/*  */ << "idle cpu ticks\n";
        oss << this->activeCPUTicks << "\t"/*  */ << "active cpu ticks\n";
        oss << this->idleCPUTicks + this->activeCPUTicks << "\t"/*  */ << "total cpu ticks\n";
        oss << snap.numPagedIn << "\t"/*  */ << "num paged in\n";
        oss << snap.numPagedOut << "\t"/*  */ << "num paged out\n";

        cout << oss.str();
    }
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
    double cpuUtilization = (static_cast<double>(coresUsed) / config.numCPU) * 100.0;

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