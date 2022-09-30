// Copyright (c) 2022. Johan Lind

#include "jleEngineStatus.h"

#include "GLFW/glfw3.h"

int jleEngineStatus::fps{0};
double jleEngineStatus::deltaTime{0.0f};
double jleEngineStatus::currentFrame{0.0f};
double jleEngineStatus::lastFrame{0.0f};

int jleEngineStatus::GetFps() { return fps; }

double jleEngineStatus::GetDeltaTime() { return deltaTime; }

double jleEngineStatus::GetTime() { return currentFrame; }

void jleEngineStatus::UpdateEngineStatus() {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    fps = static_cast<int>(1.0 / deltaTime);
}
