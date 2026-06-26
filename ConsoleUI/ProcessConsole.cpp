#include "ProcessConsole.hpp"
#include "ConsoleManager.hpp"
#include <chrono>
#include "../Scheduler/Scheduler.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>



ProcessConsole::ProcessConsole(std::shared_ptr<Process> process, std::string name)
    : AConsole(name), activeProcess(process) {}

void ProcessConsole::onEnabled(){
    this->display();
}

void ProcessConsole::process(){
    if (this->refreshed == false){
        this -> refreshed = true;
        this -> printProcessInfo();
    }

    std::cout << "root:\\>";

    // Read user input
    std::string command;
    std::getline(std::cin, command);

    if (command == "process-smi") {
        this->printProcessInfo();
    } 
    
    else if (command == "exit") {
        ConsoleManager::getInstance()->returnToPreviousConsole();
    }
}

void ProcessConsole::display(){
    // clear screen
    std::cout << "\033[2J\033[1;1H";
}


void ProcessConsole::printProcessInfo() {
    if (activeProcess) {
        std::string timestamp = getTimestamp();
        std::ostringstream oss;
        oss << timestamp << " Core:" << activeProcess->getCPUCoreID();
        std::string formattedLog = oss.str();

        std::cout << "Process Name: " << this->name << std::endl;
        std::cout << "ID: " << activeProcess->getPID() << std::endl;
        std::cout << "Logs: \n" << formattedLog << std::endl;
        std::cout << "\nCurrent Instruction line: " << activeProcess ->getCommandCounter() << std::endl;
        std::cout << "Lines of code: " << activeProcess -> getLinesOfCode();
        std::cout << "\n\n";

        // append log
        this->logs.push_back(formattedLog);
    }

    if (activeProcess -> getCommandCounter() == activeProcess ->getLinesOfCode() ){
        std::cout << "\nFinished!\n";
    }


}

std::string ProcessConsole::getTimestamp() {
	auto now = std::chrono::system_clock::now();
	time_t t = std::chrono::system_clock::to_time_t(now);
	tm tm = *std::localtime(&t);

	std::ostringstream oss;
	oss << std::put_time(&tm, "(%m/%d/%Y %I:%M:%S%p)");
	return oss.str();
}