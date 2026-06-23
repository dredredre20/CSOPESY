#pragma once

class ICommand {
public:
    enum CommandType {
        PRINT
    };

    ICommand(int p_id, CommandType commandType);
    virtual ~ICommand() = default;

    CommandType getCommandType() const;

    virtual void execute(int coreId) = 0;

protected:
    int p_id;
    CommandType commandType;
};