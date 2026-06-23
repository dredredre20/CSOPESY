#include "ICommand.hpp"

ICommand::ICommand(int p_id, CommandType commandType) {
    this->p_id = p_id;
    this->commandType = commandType;
}

ICommand::CommandType ICommand::getCommandType() const {
    return this->commandType;
}