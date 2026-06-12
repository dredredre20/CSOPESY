#pragma once
#include <string>
#include "imgui.h"

class AWindow {
protected:
    bool isVisible;
    std::string windowName;

    bool beginWindow();
    void endWindow();

public:
    AWindow(const std::string& name);
    virtual ~AWindow() = default;

    void show();
    void hide();
    bool isShown();

    virtual void update() = 0;
    virtual void draw() = 0;
};