#pragma once
#include "AConsole.hpp"
#include "../Process/Process.hpp"

class ProcessConsole : public AConsole{
    private:
        std::shared_ptr<Process> activeProcess;
        void printProcessInfo() const;
        bool refreshed = false;

    public:
        ProcessConsole(std::shared_ptr<Process> process, std::string name);
        
        void onEnabled() override;
        void display() override;
        void process() override;
};