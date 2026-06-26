#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include "Scheduler.hpp"

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
    cout << "Stopping scheduler...\n";
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
    std::string border = "--------------------------------";

    // Print current running processes
    cout << "\n";
    cout << border;
    cout << "\nRunning Processes:\n";
    if (runningProcesses.empty()) {
        cout << "\n";
    }
    else {
        for (const auto& [core, p] : runningProcesses) {
            cout << p->getName()
                << getTimestamp()
                << " Core:" << core
                << "   " << p->getCommandCounter()
                << "/" << p->getLinesOfCode() << "\n";
        }
    }

    // Print history log of finished processes
    cout << "\nFinished Processes:\n";
    if (finishedProcesses.empty()) {
        cout << "\n";
    }
	else{
		for (const auto& p : finishedProcesses) {
			cout << p->getName()
				<< " (" << p->getCreationTimestamp() << ")"
				<< " Finished   "
				<< p->getLinesOfCode() << "/" << p->getLinesOfCode() << "\n";
		}
	}
    cout << border;
    cout << "\n";
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
    int coresAvailable = config.numCPU;
    double cpuUtilization = (coresAvailable > 0)
        ? (static_cast<double>(coresUsed) / static_cast<double>(coresAvailable)) * 100.0
        : 0.0;
 
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