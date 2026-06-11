#include <GLFW/glfw3.h>
#include <cstdio>
#include <ctime>
#include <string>
#include <chrono>

#include "desktop.hpp"
#include "imgui.h"

using namespace std;

static bool createButton(string name, ImVec2 displaySize, GLFWwindow* window, int buttonIndex) {
    // scale with display size  
    float btnW = displaySize.x * 0.08f;   
    float btnH = displaySize.y * 0.05f;   
    ImVec2 buttonSize = ImVec2(btnW, btnH);

    // dynamic adjustment of buttons for proper display
    float totalOffset = 20.0f + buttonIndex * (buttonSize.x + 10.0f);

    ImGui::SetCursorPos(ImVec2(displaySize.x - buttonSize.x - totalOffset,
        displaySize.y - buttonSize.y - 20.0f));

    // render the button
    return ImGui::Button(name.c_str(), buttonSize);
}

static void initializeButtonCreation(ImVec2 displaySize, GLFWwindow* window) {
    if (createButton("PWR", displaySize, window, 0)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    
    if (createButton("NET", displaySize, window, 1)) {
        // placeholder for now
    }

    if (createButton("VOL", displaySize, window, 2)) {
        // placeholder for now
    }

    if (createButton("SRCH", displaySize, window, 5)) {
        // placeholder for now
    }
    if (createButton("IMG", displaySize, window, 6)) {
        // placeholder for now
    }
    if (createButton("STOP",  displaySize, window, 7)) {
        // placeholder for now
    }
    if (createButton("START", displaySize, window, 8)) {
        // placeholder for now
    }
    if (createButton("INIT", displaySize, window, 9)) {
        // placeholder for now
    }
    if (createButton("FOLDER", displaySize, window, 10)) {
        // placeholder for now
    }
}


void Desktop::renderDesktop(GLFWwindow* window)
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(displaySize);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;
    ImGui::Begin("Desktop", nullptr, flags);

    // initial wallpaper design (no image yet)
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 p0 = ImGui::GetWindowPos();
    ImVec2 p1 = ImVec2(p0.x + displaySize.x, p0.y + displaySize.y);

    ImU32 colTop = IM_COL32(30, 60, 110, 255);
    ImU32 colBottom = IM_COL32(10, 20, 40, 255);
    drawList->AddRectFilledMultiColor(p0, p1, colTop, colTop, colBottom, colBottom);

	// display date and time in the top-right corner
    const auto present_datetime = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(present_datetime); // convert precise time point to c-style
    ImVec2 datetimePos = ImVec2(displaySize.x - 280.0f, 10.0f);
    drawList->AddText(ImGui::GetFont(), 20.0f, datetimePos, IM_COL32(255, 255, 255, 255), ctime(&t));

    // Button Creation
    ImGui::SetWindowFontScale(1.5f);
    initializeButtonCreation(displaySize, window);
    ImGui::SetWindowFontScale(1.0f);


    ImGui::End();
}



