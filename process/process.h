#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "../Commands/ICommand.hpp"

class Process {

public:
    enum ProcessState {
        READY,
        RUNNING,
        WAITING,
        FINISHED
    };

    Process(int p_id, std::string name);

    void addCommand(ICommand::CommandType commandType);

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

    std::string getName() const;
    std::string getCreationTimestamp() const;

private:
    int p_id;
    std::string name;
    std::string creationTimestamp;

    typedef std::vector<std::shared_ptr<ICommand>> CommandList;
    CommandList commandList;

    int commandCounter;
    int cpuCoreID = -1;
    ProcessState currentState;

    static std::string makeTimestamp();
};
