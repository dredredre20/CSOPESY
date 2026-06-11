#pragma once

#include <vector>
#include <string>
#include <functional>
#include "imgui.h"

struct TaskbarIcon {
    std::string name;
    void* texture = nullptr;       // null if it's a text-style button
    ImVec4 color = ImVec4(1, 1, 1, 1); // text color for non-image buttons
    std::function<void()> onClick;
};

class Taskbar {
public:
    void initialize();
    void draw();
    void drawSystemTray();

private:
    std::vector<TaskbarIcon> taskbarIcons;
};