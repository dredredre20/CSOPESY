#pragma once
#include "AConsole.hpp"
#include "../Config.hpp"
#include "../Scheduler/Scheduler.hpp"
#include "../process/Process.hpp"
#include <string>

class MainMenuConsole : public AConsole{
    private:
        void printMenu() const;
        void handleInitializeCommand();
        void handleExitCommand();
        void handleScreenSCommand(const std::string &input); 
        void handleScreenRCommand(const std::string &input);
        void handleScreenCCommand(const std::string &input);
        void handleSchedulerStartCommand();
        void handleSchedulerStopCommand();
        void handleReportUtilCommand();
        void handleScreenLsCommand();
        void handleProcessSMICommand();
        void handleVmStatCommand();
        
        bool initialized = false;
        Config config;
        std::unique_ptr<Scheduler> scheduler;

    public:
        MainMenuConsole();
        void onEnabled() override;
        void display() override;
        void process() override;
};