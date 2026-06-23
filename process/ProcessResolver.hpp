#pragma once

class Process;

class ProcessResolver {
    public:
        virtual ~ProcessResolver() = default;
        virtual Process* getProcessOnCore(int coreId) = 0; 
};