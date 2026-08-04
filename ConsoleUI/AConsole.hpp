#pragma once 
#include <string>

class AConsole{
    public:
        AConsole(std::string name) : name(name) {} 
        
        ~AConsole() = default;

        std::string getName() { return name; } 

        virtual void onEnabled() = 0; 
        virtual void display() = 0; 
        virtual void process() = 0; 

        std::string name;
        friend class ConsoleManager;
};