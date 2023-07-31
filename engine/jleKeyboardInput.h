// Copyright (c) 2023. Johan Lind

#pragma once

#include <memory>

class jleWindow;

class jleKeyboardInput {
public:
    explicit jleKeyboardInput(std::shared_ptr<jleWindow> window);

    void linkWindow(std::shared_ptr<jleWindow> window);

    bool keyPressed(int key);

    bool keyReleased(int key);

    bool keyDown(int key, bool ignoreInputEnabled = false);

    void isEnabled(bool value);

private:
    std::shared_ptr<jleWindow> windowInternal;

    bool _isEnabled = true;
};
