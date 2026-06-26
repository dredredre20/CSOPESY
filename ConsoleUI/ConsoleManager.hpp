#pragma once
#include "AConsole.hpp"
#include <unordered_map>
#include "BaseScreen.hpp"
#include "MainMenuConsole.hpp"
#include "ProcessConsole.hpp"

const std::string MAIN_MENU_CONSOLE = "MAIN_MENU_CONSOLE";
const std::string PROCESS_CONSOLE = "PROCESS_CONSOLE";


class ConsoleManager{
    public:
        typedef std::unordered_map<std::string, std::shared_ptr<AConsole>> ConsoleTable;

        static ConsoleManager* getInstance();
        static void initialize();
        static void destroy();

        void drawConsole() const;
        void process() const;
        void switchConsole(std::string consoleName);
        bool hasScreen(std::string consoleName) const;

        
        void registerScreen(std::shared_ptr<AConsole> screenRef);
        void switchToScreen(std::string screenName);
        void unregisterScreen(std::string screenName);

        void returnToPreviousConsole();
        void exitApplication();
        bool isRunning() const;



    private:
        ConsoleManager();
        ~ConsoleManager() = default;
        ConsoleManager(const ConsoleManager&) = delete; // copy constructor is private
        ConsoleManager& operator=(const ConsoleManager&) = delete; // assignment operator is private
        static ConsoleManager* sharedInstance;
        
        ConsoleTable consoleTable;
        std::shared_ptr<AConsole> currentConsole;
        std::shared_ptr<AConsole> previousConsole;

        bool running = true;
};

