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
    this->creationTimestamp = makeTimestamp();
    this->sleeping = false;
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
    else if (commandType == ICommand::DECLARE) {
        // Randomize variable name from a pool and initial uint16 value
        std::string varName = randomVarName();
        uint16_t initVal = static_cast<uint16_t>(rand() % std::numeric_limits<uint16_t>::max());
        const std::shared_ptr<ICommand> declare =
            std::make_shared<DeclareCommand>(varName, initVal);
        this->commandList.push_back(declare);
    }

    else if (commandType == ICommand::ADD) {
        // result = varA + varB (all from symbol table, auto-declared if missing)
        std::string result = randomVarName();
        AddCommand::Operand a = randomOperand();
        AddCommand::Operand b = randomOperand();
        const std::shared_ptr<ICommand> add =
            std::make_shared<AddCommand>(result, a, b);
        this->commandList.push_back(add);
    }

    else if (commandType == ICommand::SUBTRACT) {
        std::string result = randomVarName();
        SubtractCommand::Operand a = randomOperand();
        SubtractCommand::Operand b = randomOperand();
        const std::shared_ptr<ICommand> subtract =
            std::make_shared<SubtractCommand>(result, a, b);
        this->commandList.push_back(subtract);
    }
   
    else if (commandType == ICommand::SLEEP) {
        // Random sleep between 1-255 ticks (uint8)
        uint8_t ticks = static_cast<uint8_t>((rand() % 255) + 1);
        const std::shared_ptr<ICommand> sleep =
            std::make_shared<SleepCommand>(ticks);
        this->commandList.push_back(sleep);
    }

    else if (commandType == ICommand::FOR) {
        // Build a randomized FOR body
        const std::shared_ptr<ICommand> forCmd = buildForCommand(1);
        this->commandList.push_back(forCmd);
    }
}

void Process::generateInstructions(int numInstructions) {
    symbolTable.setVar("x", 0);
    symbolTable.setVar("y", 0);
    symbolTable.setVar("z", 0);

    static const std::vector<std::string> vars = { "x", "y", "z" };

    for (int i = 0; i < numInstructions; ++i) {
        int roll = i % 6;  // cycles through 6 command types

        switch (roll) {
        case 0:
            // ADD(x, x, 1)
            commandList.push_back(std::make_shared<AddCommand>(
                "x", std::string("x"), uint16_t(1)));
            break;

        case 1:
            // SUB(y, y, 0) — effectively a no-op subtract, just to exercise the command
            commandList.push_back(std::make_shared<SubtractCommand>(
                "y", std::string("y"), uint16_t(0)));
            break;

        case 2:
            // DECLARE z with a cycling value
            commandList.push_back(std::make_shared<DeclareCommand>(
                vars[i % 3], static_cast<uint16_t>(i * 3)));
            break;

        case 3:
            // PRINT with variable value
            commandList.push_back(std::make_shared<PrintCommand>(
                this->p_id, this->name,
                "Hello world from " + this->name + "! x=",
                std::string("x")));   // prints x's current value
            break;

        case 4:
            // ADD(z, x, y)
            commandList.push_back(std::make_shared<AddCommand>(
                "z", std::string("x"), std::string("y")));
            break;

        case 5:
            // PRINT without variable
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

    if (sleeping) {
        wake();
        return;
    }

    this->commandCounter++;
    if (isFinished()) {
        this->currentState = ProcessState::FINISHED;
    }
}

// Picks a random variable name from a fixed pool
std::string Process::randomVarName() const {
    static const std::vector<std::string> pool = { "x", "y", "z", "a", "b", "c" };
    return pool[rand() % pool.size()];
}

// Randomly returns either a variable name or a uint16 constant
AddCommand::Operand Process::randomOperand() const {
    if (rand() % 2 == 0) {
        return randomVarName();
    }
    else {
        return static_cast<uint16_t>(rand() % std::numeric_limits<uint16_t>::max());
    }
}

// Recursively builds a FOR command up to MAX_DEPTH
std::shared_ptr<ICommand> Process::buildForCommand(int depth) {
    std::vector<std::shared_ptr<ICommand>> body;

    int bodySize = (rand() % 4) + 2;
    for (int i = 0; i < bodySize; ++i) {
        // Randomly pick a non-FOR command for the body
        int roll = rand() % (depth < ForCommand::MAX_DEPTH ? 5 : 4);
        switch (roll) {
        case 0: {
            std::string toPrint = "Hello world from " + this->name + "!";
            body.push_back(std::make_shared<PrintCommand>(
                this->p_id, this->name, toPrint));
            break;
        }
        case 1: {
            std::string varName = randomVarName();
            uint16_t initVal = static_cast<uint16_t>(rand() % std::numeric_limits<uint16_t>::max());
            body.push_back(std::make_shared<DeclareCommand>(varName, initVal));
            break;
        }
        case 2: {
            body.push_back(std::make_shared<AddCommand>(
                randomVarName(), randomOperand(), randomOperand()));
            break;
        }
        case 3: {
            body.push_back(std::make_shared<SubtractCommand>(
                randomVarName(), randomOperand(), randomOperand()));
            break;
        }
        case 4: {
            // increment depth
            body.push_back(buildForCommand(depth + 1));
            break;
        }
        }
    }

    // Random iterations between 1-10
    int iterations = (rand() % 10) + 1;
    return std::make_shared<ForCommand>(std::move(body), iterations, depth);
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
