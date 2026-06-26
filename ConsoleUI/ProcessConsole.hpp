#pragma once
#include "AConsole.hpp"
#include "../Process/Process.hpp"

class ProcessConsole : public AConsole{
    private:
        void printProcessInfo() const;
        std::shared_ptr<Process> attachedProcess;
        bool refreshed = false;

    public:
        ProcessConsole(std::shared_ptr<Process> process, std::string process_name);

        void onEnabled() override;
        void display() override;
        void process() override;
};