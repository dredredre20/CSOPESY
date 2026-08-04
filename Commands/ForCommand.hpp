#pragma once
#include "ICommand.hpp"
#include <vector>
#include <memory>

class ForCommand : public ICommand {
public:
    ForCommand(std::vector<std::shared_ptr<ICommand>> body,
        int iterations,
        int depth = 1)
        : body(std::move(body)), iterations(iterations), depth(depth) {
    }

    void execute(int coreID, Process& process) override;
    CommandType getCommandType() const override { return FOR; }

    static constexpr int MAX_DEPTH = 3;

private:
    std::vector<std::shared_ptr<ICommand>> body;
    int iterations;
    int depth;
};