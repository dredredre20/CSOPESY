#include "ProcessConsole.hpp"
#include "ConsoleManager.hpp"
#include <chrono>
#include "../Scheduler/Scheduler.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>



ProcessConsole::ProcessConsole(std::shared_ptr<Process> process, std::string name)
    : AConsole(PROCESS_CONSOLE) {}

void ProcessConsole::onEnabled(){
    this->display();
}

void ProcessConsole::process(){
    std::cout << "\nroot:\\>";

    // Read user input
    std::string command;
    std::getline(std::cin, command);

    if (command == "process-smi") {
        this->printProcessInfo();
    } 
    
    else if (command == "exit") {
        ConsoleManager::getInstance()->switchConsole(MAIN_MENU_CONSOLE);
    }
}

void ProcessConsole::display(){
    // clear screen
    std::cout << "\033[2J\033[3J\033[1;1H";

    this -> printProcessInfo();
}


void ProcessConsole::printProcessInfo() {
    if (activeProcess) {
        std::string timestamp = getTimestamp();
        std::ostringstream oss;
        oss << timestamp << " Core:" << activeProcess->getCPUCoreID();
        std::string formattedLog = oss.str();

        // Append then print all logs
        this->logs.push_back(formattedLog);

        std::cout << "\nProcess Name: " << activeProcess->getName() << std::endl;
        std::cout << "ID: " << activeProcess->getPID() << std::endl;
        std::cout << "\nCurrent Instruction line: " << activeProcess->getCommandCounter() << std::endl;
        std::cout << "Lines of code: " << activeProcess->getLinesOfCode() << "\n\n";

        std::cout << "Logs:\n";
        for (const auto& log : this->logs) {
            std::cout << log << "\n";
        }
    }

    if (activeProcess && activeProcess->isFinished()) {
        std::cout << "\nFinished!\n";
    }
}

void ProcessConsole::setProcess(std::shared_ptr<Process> process) {
    this->activeProcess = process;
    this->refreshed = false;
    this->logs.clear();
}

std::string ProcessConsole::getTimestamp() {
	auto now = std::chrono::system_clock::now();
	time_t t = std::chrono::system_clock::to_time_t(now);
	tm tm = *std::localtime(&t);

	std::ostringstream oss;
	oss << std::put_time(&tm, "(%m/%d/%Y %I:%M:%S%p)");
	return oss.str();
}