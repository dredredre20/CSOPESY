#include "process.h"
#include "../Commands/PrintCommand.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

// Constructor
Process::Process(int p_id, std::string name) {
    this->p_id = p_id;
    this->name = name;
    this->commandCounter = 0;
    this->currentState = ProcessState::READY;
    this->creationTimestamp = makeTimestamp();
}

std::string Process::makeTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%m/%d/%Y %I:%M:%S%p");
    return oss.str();
}

// Add a command to the process
void Process::addCommand(ICommand::CommandType commandType) {
    if (commandType == ICommand::PRINT) {
        std::string toPrint = "Hello world from " + this->name + "!";
        const std::shared_ptr<ICommand> print =
            std::make_shared<PrintCommand>(this->p_id, this->name, toPrint);
        this->commandList.push_back(print);
    }

    // Extend for future command implementations
}

// Execute the command pointed to by commandCounter
void Process::executeCurrentCommand(int coreId) {
    if (this->commandCounter < static_cast<int>(this->commandList.size())) {
        this->commandList[this->commandCounter]->execute(coreId);
    } else {
        std::cerr << "Error: Process " << this->p_id
                   << " attempted to execute past its command list bounds.\n";
    }
}

// Move instruction pointer to the next command line
void Process::moveToNextLine() {
    if (!isFinished()) {
        this->commandCounter++;

        if (isFinished()) {
            this->currentState = ProcessState::FINISHED;
        }
    }
}

bool Process::isFinished() const {
    return this->commandCounter == static_cast<int>(this->commandList.size());
}

int Process::getRemainingTime() const {
    return static_cast<int>(this->commandList.size()) - this->commandCounter;
}

int Process::getCommandCounter() const {
    return this->commandCounter;
}

int Process::getLinesOfCode() const {
    return static_cast<int>(this->commandList.size());
}

int Process::getPID() const {
    return this->p_id;
}

int Process::getCPUCoreID() const {
    return this->cpuCoreID;
}

void Process::setCPUCoreID(int coreId) {
    this->cpuCoreID = coreId;
    this->currentState = ProcessState::RUNNING;
}

Process::ProcessState Process::getState() const {
    return this->currentState;
}

void Process::setState(ProcessState state) {
    this->currentState = state;
}

std::string Process::getName() const {
    return this->name;
}

std::string Process::getCreationTimestamp() const {
    return this->creationTimestamp;
}
