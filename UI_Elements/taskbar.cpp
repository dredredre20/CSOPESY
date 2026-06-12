#define IMGUI_DEFINE_MATH_OPERATORS
#define STB_IMAGE_IMPLEMENTATION

#include <GLFW/glfw3.h>
#include <cstdio>
#include <ctime>
#include <string>
#include <chrono>
#include  "taskbar.hpp"
#include "imgui.h"
#include "UIConfig.hpp"
#include "../LoadTexture/loadTexture.hpp"
#include <stdio.h>
#include "UIManager.hpp" 


// initialize buttons with icons, colors, and click actions
void Taskbar::initialize(GLFWwindow* window) {
    GLuint filesIcon = loadTexture("assets/folder.png");
    GLuint taskManagerIcon = loadTexture("assets/task_manager.png");

    taskbarIcons = {
        { "Files",  filesIcon, ImVec4(1,1,1,1), []() { 
            UIManager::getInstance().showWindow("File Browser");
        }},
        { "INIT",   0, ImVec4(0.4f, 0.8f, 1.0f, 1), []() { 
            UIManager::getInstance().showWindow("Initialize");
        } },
        { "START",  0, ImVec4(0.4f, 0.9f, 0.4f, 1), []() { /* start action */ } },
        { "STOP",   0, ImVec4(1.0f, 0.3f, 0.3f, 1.0f), []() { /* stop action */ } },

        { "TASKMANAGER",   taskManagerIcon, ImVec4(0.9f, 0.4f, 0.4f, 1), []() { 
            UIManager::getInstance().showWindow("Task Manager");
        } },
        { "SRCH",   0, ImVec4(0.7f, 0.4f, 0.9f, 1), []() { /* search action */ } },
        { "VOL",   0, ImVec4(0.7f, 0.4f, 0.9f, 1), []() { /* search action */ } },
        { "NET",   0, ImVec4(0.7f, 0.4f, 0.9f, 1), []() { /* search action */ } },

        { "PWR",   0, ImVec4(1.0f, 0.3f, 0.3f, 1.0f), [window]() {
            glfwSetWindowShouldClose(window, GLFW_TRUE); 
        } }
    };
}

void Taskbar::draw() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float taskbarHeight = 60.0f * UIConfig::getScaleFactor();

    // Position at bottom of screen
    ImGui::SetNextWindowPos(ImVec2(0, displaySize.y - taskbarHeight));
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, taskbarHeight));

    ImGui::Begin("Taskbar", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar);

    int iconCount = (int)taskbarIcons.size();
    float spacing = 10.0f;

    // set size for buttons
    float maxButtonWidth = 80.0f * UIConfig::getScaleFactor();

    // dynamic resizing accounting for the display size
    float totalSpacing = spacing * (iconCount + 1);
    float dynamicWidth = (displaySize.x - totalSpacing) / iconCount;
    float buttonWidth = (dynamicWidth > maxButtonWidth) ? maxButtonWidth : dynamicWidth;
    float buttonHeight = taskbarHeight - 10.0f;

    ImVec2 btnSize(buttonWidth, buttonHeight);
    ImGui::SetWindowFontScale(1.5f);
    // Render icons
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, 0));

    // left corner buttons
    for (int i = 0; i < iconCount - 3; i++) {
        auto& icon = taskbarIcons[i];
        if (icon.texture) {
            if (ImGui::ImageButton(icon.name.c_str(), (ImTextureID)(intptr_t)icon.texture, btnSize)) {
                icon.onClick();
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", icon.name.c_str());
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, icon.color);
            if (ImGui::Button(icon.name.c_str(), btnSize)) {
                icon.onClick();
            }
            ImGui::PopStyleColor();
        }
        ImGui::SameLine();
    }

    // right corner buttons calculator
    float rightGroupWidth = (buttonWidth * 3) + (spacing * 3);
    float rightStartPosX = displaySize.x - rightGroupWidth;

    // force imgui to go to specified coordinate
    ImGui::SetCursorPosX(rightStartPosX);

    for (int i = iconCount - 3; i < iconCount; i++) {
        auto& icon = taskbarIcons[i];
        if (icon.texture) {
            if (ImGui::ImageButton(icon.name.c_str(), (ImTextureID)(intptr_t)icon.texture, btnSize)) {
                icon.onClick();
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", icon.name.c_str());
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, icon.color);
            if (ImGui::Button(icon.name.c_str(), btnSize)) {
                icon.onClick();
            }
            ImGui::PopStyleColor();
        }
        if (i < iconCount - 1) {
            ImGui::SameLine();
        }
    }

    ImGui::PopStyleVar();
    ImGui::SetWindowFontScale(1.0f);
    ImGui::End();
}
