#include "Windows.hpp"
#include "imgui.h"


FileBrowserWindow::FileBrowserWindow() : AWindow("File Browser") {}

void FileBrowserWindow::update() {} 

void FileBrowserWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(600.0f, 450.0f), ImGuiCond_FirstUseEver);

    if (beginWindow()) {
        ImGui::Text("Directory Explorer: /root/user/desktop");
        ImGui::Separator();
        
        if (ImGui::Selectable("Documents")) {}
        if (ImGui::Selectable("Downloads")) {}
        if (ImGui::Selectable("assets")) {}
        if (ImGui::Selectable("csopesy_os.exe")) {}
    }
    endWindow();
}

InitWindow::InitWindow() : AWindow("Initialize") {}

void InitWindow::update() {}

void InitWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(400.0f, 350.0f), ImGuiCond_FirstUseEver);

    if (beginWindow()) {;
        ImGui::Text("System Settings Configurations");
        ImGui::Separator();
        
        static float volume = 0.8f;
        ImGui::SliderFloat("Master Volume", &volume, 0.0f, 1.0f);
        
        static bool darkMode = true;
        ImGui::Checkbox("Enable Dark Mode", &darkMode);
    }
    endWindow();
}


TaskManagerWindow::TaskManagerWindow() : AWindow("Task Manager") {}

void TaskManagerWindow::update() {}


// Component 3 - a window that closely resembles the Windows task manager
void TaskManagerWindow::draw() {
    ImGui::SetNextWindowSize(ImVec2(800.0f, 550.0f), ImGuiCond_FirstUseEver);

    if (beginWindow()){
        // Implementation
    }

    endWindow();
}