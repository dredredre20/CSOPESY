#include "AWindow.hpp"

AWindow::AWindow(const std::string& name) : windowName(name), isVisible(false) {}

bool AWindow::beginWindow() {
    if (!isVisible){ 
        return false;
    }
    return ImGui::Begin(windowName.c_str(), &isVisible);
}

void AWindow::endWindow() {
    ImGui::End();
}

void AWindow::show() { 
    isVisible = true; 
}

void AWindow::hide() { 
    isVisible = false; 
}

bool AWindow::isShown() { 
    return isVisible; 
}