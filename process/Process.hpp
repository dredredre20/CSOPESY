#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <cstddef>
#include "../Commands/ICommand.hpp"
#include "../Commands/AddCommand.hpp"
#include "../SymbolTable/SymbolTable.hpp"

class Process {

public:
    enum ProcessState {
        READY,
        RUNNING,
        WAITING,
        FINISHED,
        TERMINATED_VIOLATION // shut down early due to memory access violation
    };

    Process(int p_id, std::string name);

    void generateInstructions(int numInstructions);

    // Executes the command currently pointed to by commandCounter on the given core.
    void executeCurrentCommand(int coreId);
    void moveToNextLine();

    bool isFinished() const;
    int getRemainingTime() const;
    int getCommandCounter() const;
    int getLinesOfCode() const;
    int getPID() const;
    int getCPUCoreID() const;
    void setCPUCoreID(int coreId);
    ProcessState getState() const;
    void setState(ProcessState state);

    // For sleep command
    void setSleeping(bool state) { sleeping = state; }
    bool isSleeping() const { return sleeping; }
    void wake() { sleeping = false; }
    void setSleepTicks(uint8_t ticks) { sleepTicksRemaining = ticks; }
    uint8_t getSleepTicks() const { return sleepTicksRemaining;  }

    // For accessing symbol table
    void setVariable(const std::string& varName, uint16_t value);
    uint16_t getVariable(const std::string& varName) const;
    bool hasVariable(const std::string& varName) const;

    std::string getName() const;
    std::string getCreationTimestamp() const;

    void setMemoryRequirement(size_t requirement) { memoryRequirement = requirement; }
    size_t getMemoryRequirement() const { return memoryRequirement; }
    void setMemoryAllocatedBlock(void* block) { allocatedMemoryBlock = block; }
    void* getMemoryAllocatedBlock() const { return allocatedMemoryBlock; }
    bool isMemoryAllocated() const { return allocatedMemoryBlock != nullptr; }
    void clearMemoryAllocation() { allocatedMemoryBlock = nullptr; }

    void addLog(const std::string& entry) { logs.push_back(entry); }
    const std::vector<std::string>& getLogs() const { return logs; }

    void setMemoryViolation(const std::string& timestamp, uintptr_t address);
    bool hasMemoryViolation() const { return memoryViolationOccurred; }
    std::string getViolationTimestamp() const { return violationTimestamp; }
    uintptr_t getViolationAddress() const { return violationAddress; }

private:
    int p_id;
    std::string name;
    std::string creationTimestamp;

    typedef std::vector<std::shared_ptr<ICommand>> CommandList;
    CommandList commandList;

    int commandCounter;
    int cpuCoreID = -1;
    ProcessState currentState;

    bool sleeping = false;
    uint8_t sleepTicksRemaining = 0;

    size_t memoryRequirement = 0;
    void* allocatedMemoryBlock = nullptr;

    SymbolTable symbolTable;
    std::vector<std::string> logs;

    bool memoryViolationOccurred = false;
    std::string violationTimestamp;
    uintptr_t violationAddress = 0;
};
