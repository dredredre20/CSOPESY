#include "Process.hpp"
#include "../SymbolTable/SymbolTable.hpp"
#include "../Commands/DeclareCommand.hpp"
#include "../Commands/PrintCommand.hpp"
#include "../Commands/AddCommand.hpp"
#include "../Commands/SubtractCommand.hpp"
#include "../Commands/ForCommand.hpp"
#include "../Commands/SleepCommand.hpp"
#include "../ConsoleUI/ProcessConsole.hpp"
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
    this->sleeping = false;
}

void Process::generateInstructions(int numInstructions) {
    symbolTable.setVar("x", 0);
    symbolTable.setVar("y", 0);
    symbolTable.setVar("z", 0);

    static const std::vector<std::string> vars = { "x", "y", "z" };

    for (int i = 0; i < numInstructions; ++i) {
        int roll = i % 7; // cycles through 7 slots

        switch (roll) {
        case 0:
            // ADD(x, x, 1)
            commandList.push_back(std::make_shared<AddCommand>(
                "x", std::string("x"), uint16_t(1)));
            break;

        case 1:
            // SUB(y, y, 0)
            commandList.push_back(std::make_shared<SubtractCommand>(
                "y", std::string("x"), uint16_t(1)));
            break;

        case 2:
            // DECLARE
            commandList.push_back(std::make_shared<DeclareCommand>(
                vars[i % 3], static_cast<uint16_t>(i + 1)));
            break;

        case 3:
            // PRINT with variable
            commandList.push_back(std::make_shared<PrintCommand>(
                this->p_id, this->name,
                "Value of x: ", std::string("x")));
            break;

        case 4:
            // SLEEP for 2 ticks
            commandList.push_back(std::make_shared<SleepCommand>(uint8_t(2)));
            break;

        case 5:
            // FOR: ADD(z, z, 1) x3 iterations
            commandList.push_back(std::make_shared<ForCommand>(
                std::vector<std::shared_ptr<ICommand>>{
                std::make_shared<AddCommand>("z", std::string("z"), uint16_t(1)),
                    std::make_shared<PrintCommand>(
                        this->p_id, this->name,
                        "z inside for: ", std::string("z"))
            },
                3, // iterations
                1 // depth
            ));
            break;

        case 6:
            // PRINT without variable — plain message
            commandList.push_back(std::make_shared<PrintCommand>(
                this->p_id, this->name,
                "Hello world from " + this->name + "!"));
            break;
        }
    }
}

// Execute the command pointed to by commandCounter
void Process::executeCurrentCommand(int coreId) {
    if (isFinished()) {
        std::cerr << "Error: Process " << this->p_id
            << " attempted to execute past its command list bounds.\n";
        return;
    }
    if (!sleeping) {
        commandList[commandCounter]->execute(coreId, *this);
    }
}

// Move instruction pointer to the next command line
void Process::moveToNextLine() {
    if (isFinished()) return;

    this->commandCounter++;
    if (isFinished()) {
        this->currentState = ProcessState::FINISHED;
    }
}

void Process::setVariable(const std::string& varName, uint16_t value) {
    symbolTable.setVar(varName, value);
}

uint16_t Process::getVariable(const std::string& varName) const {
    return symbolTable.getVar(varName);
}

bool Process::hasVariable(const std::string& varName) const {
    return symbolTable.hasVar(varName);
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
