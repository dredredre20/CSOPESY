#pragma once
#include "AConsole.hpp"
#include "../Process/Process.hpp"

class ProcessConsole : public AConsole{
    public:
    
        ProcessConsole(std::shared_ptr<Process> process, std::string name);
        
        void onEnabled() override;
        void display() override;
        void process() override;
        static std::string getTimestamp(); 
        
    private:
        std::shared_ptr<Process> activeProcess;
        void printProcessInfo();
        bool refreshed = false;
        std::vector<std::string> logs;
};