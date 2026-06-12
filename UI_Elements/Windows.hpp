#pragma once
#include "AWindow.hpp"

class FileBrowserWindow : public AWindow {
public:
    FileBrowserWindow();
    void update() override;
    void draw() override;
};

class InitWindow : public AWindow {
public:
    InitWindow();
    void update() override;
    void draw() override;
};

class TaskManagerWindow : public AWindow {
public:
    TaskManagerWindow();
    void update() override;
    void draw() override;
};