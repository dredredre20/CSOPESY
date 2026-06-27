#pragma once
#include "ICommand.hpp"
#include <cstdint>

class SleepCommand : public ICommand {
public:
    // ticks: number of CPU ticks to sleep (uint8, so 0-255)
    explicit SleepCommand(uint8_t ticks) : ticks(ticks) {}

    void execute(int coreID, Process& process) override;
    CommandType getCommandType() const override { return SLEEP; }

private:
    uint8_t ticks;
};