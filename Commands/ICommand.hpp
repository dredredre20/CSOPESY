#pragma once

class ICommand {
public:
    enum CommandType {
        PRINT
    };

    ICommand(int p_id, CommandType commandType);
    virtual ~ICommand() = default;

    CommandType getCommandType() const;

    // CoreId is passed in so a command knows which core executed it
    // (needed for logging "Core:X" in the print command).
    virtual void execute(int coreId) = 0;

protected:
    int p_id;
    CommandType commandType;
};