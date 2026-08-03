#include "Process.hpp"
#include "../SymbolTable/SymbolTable.hpp"
#include "../Commands/DeclareCommand.hpp"
#include "../Commands/PrintCommand.hpp"
#include "../Commands/AddCommand.hpp"
#include "../Commands/SubtractCommand.hpp"
#include "../Commands/ForCommand.hpp"
#include "../Commands/SleepCommand.hpp"
#include "../Commands/ReadCommand.hpp"
#include "../Commands/WriteCommand.hpp"
#include "../ConsoleUI/ProcessConsole.hpp"
#include "../Memory/IMemoryAllocator.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

// Constructor
Process::Process(int p_id, std::string name, size_t memRequired) {
    this->p_id = p_id;
    this->name = name;
    this->memoryRequirement = memRequired;
    this->commandCounter = 0;
    this->currentState = ProcessState::READY;
    this->sleeping = false;
}

void Process::generateInstructions(int numInstructions) {
    symbolTable.declareVar("x");
    symbolTable.declareVar("y");
    symbolTable.declareVar("z");

	uintptr_t addrX = symbolTable.getAddress("x");
    uintptr_t addrY = symbolTable.getAddress("y");
    uintptr_t addrZ = symbolTable.getAddress("z");

    static const std::vector<std::string> vars = { "x", "y", "z" };

    for (int i = 0; i < numInstructions; ++i) {
        int roll = i % 9; // cycles through 9 slots

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

        case 7:
			// READ command - reads from shared memory address into variable "x"
            commandList.push_back(std::make_shared<ReadCommand>(
                std::string("x"), addrX
            ));

            break;

        case 8:
			// WRITE command - writes the value of x to shared memory address
            commandList.push_back(std::make_shared<WriteCommand>(
                addrX, std::string("x") 
            ));
     
            break;
        }
    }
}

// Add instructions to commandList
void Process::loadInstructions(const std::vector<std::shared_ptr<ICommand>>& instructions) {
    commandList.insert(commandList.end(), instructions.begin(), instructions.end());
}

// Helper function to parse instructions from a string
void Process::parseInstructions(const std::string& instructions) {
    std::istringstream stream(instructions);
    std::string token;

    while (std::getline(stream, token, ';')) {
		std::istringstream lineStream(token);
		std::string commandType;
		lineStream >> commandType;
		if (commandType == "DECLARE") {
			std::string varName;
			uint16_t value;
			lineStream >> varName >> value;
			commandList.push_back(std::make_shared<DeclareCommand>(varName, value));
		}
		else if (commandType == "PRINT") {
			std::string message;
			std::getline(lineStream, message);
			commandList.push_back(std::make_shared<PrintCommand>(this->p_id, this->name, message));
		}
		else if (commandType == "ADD") {
			std::string resultVar, operandA, operandB;
			lineStream >> resultVar >> operandA >> operandB;
			commandList.push_back(std::make_shared<AddCommand>(resultVar, operandA, operandB));
		}
		else if (commandType == "SUBTRACT") {
			std::string resultVar, operandA, operandB;
			lineStream >> resultVar >> operandA >> operandB;
			commandList.push_back(std::make_shared<SubtractCommand>(resultVar, operandA, operandB));
		}
		else if (commandType == "SLEEP") {
			uint8_t ticks;
			lineStream >> ticks;
			commandList.push_back(std::make_shared<SleepCommand>(ticks));
		}
        else if (commandType == "READ") {
			std::string varName;
			uintptr_t address;
            lineStream >> varName >> address;
            commandList.push_back(std::make_shared<ReadCommand>(varName, address));
        }
        else if (commandType == "WRITE") {
            std::string varName;
            uintptr_t address;
            lineStream >> address >> varName;
            commandList.push_back(std::make_shared<WriteCommand>(address, varName));
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
    if (!symbolTable.hasVar(varName))
        symbolTable.declareVar(varName);

    uintptr_t addr = symbolTable.getAddress(varName);

    if (memoryAllocator && allocatedMemoryBlock)
        memoryAllocator->writeMemory(allocatedMemoryBlock, addr, value);
}

uint16_t Process::getVariable(const std::string& varName) const {
    if (!symbolTable.hasVar(varName))
        throw std::runtime_error("Variable '" + varName + "' not declared.");

    uintptr_t addr = symbolTable.getAddress(varName);

    if (memoryAllocator && allocatedMemoryBlock)
        return memoryAllocator->readMemory(allocatedMemoryBlock, addr);

    return 0; // not yet allocated
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

// process is marked as terminated due to a memory access violation
void Process::setMemoryViolation(const std::string& timestamp, uintptr_t address) {
    memoryViolationOccurred = true;
    violationTimestamp = timestamp;
    violationAddress = address;
    currentState = ProcessState::TERMINATED_VIOLATION;
}