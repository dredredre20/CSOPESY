#include <GLFW/glfw3.h>
#include <cstdio>
#include <ctime>
#include <string>
#include <chrono>

#include "desktop.hpp"
#include "imgui.h"

using namespace std;

void Desktop::drawClock() {
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    struct tm timeinfo;
    localtime_s(&timeinfo, &time);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%I:%M:%S %p |  %a, %b %d %Y", &timeinfo);

    ImGui::SetWindowFontScale(1.5f); // increase font size
    ImVec2 textSize = ImGui::CalcTextSize(buffer);
    ImVec2 pos = ImVec2(ImGui::GetIO().DisplaySize.x - textSize.x - 20, 20);

    ImGui::SetCursorPos(pos);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", buffer);

    ImGui::SetWindowFontScale(1.0f); // reset for other elements
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
    drawClock();

    ImGui::End();
}



