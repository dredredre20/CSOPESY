#pragma once

#include <vector>
#include <string>
#include <functional>
#include "imgui.h"

struct TaskbarIcon {
    std::string name;
    GLuint texture;
    ImVec4 color = ImVec4(1, 1, 1, 1); // text color for non-image buttons
    std::function<void()> onClick;
};

class Taskbar {
public:
    void initialize();
    void draw();

private:
    std::vector<TaskbarIcon> taskbarIcons;
};