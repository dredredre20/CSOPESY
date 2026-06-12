#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "AWindow.hpp"

class UIManager {
private:
    std::unordered_map<std::string, std::shared_ptr<AWindow>> windows;
    UIManager() = default; // Private constructor for Singleton

public:
    static UIManager& getInstance();

    void registerWindow(const std::string& name, std::shared_ptr<AWindow> window);
    void showWindow(const std::string& name);
    void hideWindow(const std::string& name);
    void updateAllWindows();
    void renderAllWindows();
};