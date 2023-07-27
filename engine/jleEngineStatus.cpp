// Copyright (c) 2023. Johan Lind

#include "jleEngineStatus.h"

#include "jleIncludeGL.h"

int jleEngineStatus::_fps{0};
double jleEngineStatus::_deltaTime{0.0f};
double jleEngineStatus::_currentFrame{0.0f};
double jleEngineStatus::_lastFrame{0.0f};

int jleEngineStatus::fps() { return _fps; }

double jleEngineStatus::deltaTime() { return _deltaTime; }

double jleEngineStatus::time() { return _currentFrame; }

void jleEngineStatus::updateEngineStatus() {
    _currentFrame = glfwGetTime();
    _deltaTime = _currentFrame - _lastFrame;
    _lastFrame = _currentFrame;
    _fps = static_cast<int>(1.0 / _deltaTime);
}
