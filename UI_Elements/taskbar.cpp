#define IMGUI_DEFINE_MATH_OPERATORS

#include <GLFW/glfw3.h>
#include <cstdio>
#include <ctime>
#include <string>
#include <chrono>

#include  "taskbar.hpp"
#include "imgui.h"
#include "UIConfig.hpp"

void Taskbar::initialize() {
    taskbarIcons = {
        //{ "Files",  loadTexture("folder.png"), ImVec4(1,1,1,1), []() { /* open file browser */ } },
        { "INIT",   nullptr, ImVec4(0.4f, 0.8f, 1.0f, 1), []() { /* init action */ } },
        { "START",  nullptr, ImVec4(0.4f, 0.9f, 0.4f, 1), []() { /* start action */ } },
        { "STOP",   nullptr, ImVec4(0.9f, 0.4f, 0.4f, 1), []() { /* stop action */ } },
       // { "Graph",  loadTexture("graph.png"), ImVec4(1,1,1,1), []() { /* toggle graph */ } },
        { "SRCH",   nullptr, ImVec4(0.7f, 0.4f, 0.9f, 1), []() { /* search action */ } },
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

    // Render icons
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));
    // initialize();
    for (auto& icon : taskbarIcons) {
        if (icon.texture) {
            if (ImGui::ImageButton(
                icon.name.c_str(),                          // unique string ID
                (ImTextureID)(intptr_t)icon.texture,        // texture ID
                ImVec2(40, 40) * UIConfig::getScaleFactor() // size
            )) {
                icon.onClick();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", icon.name.c_str());
            }
        }
        ImGui::SameLine();
    }
    

    ImGui::PopStyleVar();

    // System tray (right side)
    drawSystemTray();

    ImGui::End();
}

void Taskbar::drawSystemTray() {
    ImGui::SameLine(ImGui::GetWindowWidth() - 200);

    
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "VOL");
    ImGui::SameLine();

    ImGui::TextColored(ImVec4(1, 1, 1, 1), "NET");
    ImGui::SameLine();

    ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1), "PWR");
   
}