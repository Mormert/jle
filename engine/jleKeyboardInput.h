// Copyright (c) 2022. Johan Lind

#pragma once

#include <memory>

class jleWindow_GLFW_OpenGL;

class jleKeyboardInput {
public:
    explicit jleKeyboardInput(
        std::shared_ptr<jleWindow_GLFW_OpenGL> windowInternal);

    void LinkWindow(std::shared_ptr<jleWindow_GLFW_OpenGL> windowInternal);

    bool GetKeyPressed(char key);

    bool GetKeyReleased(char key);

    bool GetKeyDown(char key, bool ignoreInputEnabled = false);

    void SetIsEnabled(bool value);

private:
    std::shared_ptr<jleWindow_GLFW_OpenGL> windowInternal;

    bool _isEnabled = true;
};
