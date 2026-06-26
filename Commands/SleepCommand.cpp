#include "SleepCommand.hpp"
#include "../Process/Process.hpp"
#include "../Process/ProcessResolver.hpp"
#include <string>
#include <chrono>
#include <thread>

SleepCommand::SleepCommand(int p_id, std::string processName, uint8_t X, ProcessResolver& resolver)
    : ICommand(p_id, ADD), resolver(resolver) {
    this->processName = processName;
}

void SleepCommand::execute(int coreId) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}