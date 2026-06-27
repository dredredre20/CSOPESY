#pragma once

class Process;

class ICommand {
public:
    enum CommandType {
        PRINT,
        DECLARE,
        ADD,
        SUBTRACT,
        SLEEP,
        FOR
    };

    virtual ~ICommand() = default;
    virtual CommandType getCommandType() const = 0;
    virtual void execute(int coreId, Process& process) = 0;
};