// the actual implementation of a console
#pragma once
#include "../Process/Process.hpp"
#include "AConsole.hpp"

class BaseScreen : public AConsole{
    private:
        void printProcessInfo() const;
        std::shared_ptr<Process> attachedProcess;
        bool refreshed = false;

    
    BaseScreen(std::shared_ptr<Process> process, std::string process_name);

    void onEnabled() override;
    void process() override;
    void display() override;
};

