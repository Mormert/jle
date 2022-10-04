// Copyright (c) 2022. Johan Lind

#pragma once

#include <memory>

class jleWindow;

class jleKeyboardInput {
public:
    explicit jleKeyboardInput(std::shared_ptr<jleWindow> windowInternal);

    void linkWindow(std::shared_ptr<jleWindow> windowInternal);

    bool keyPressed(char key);

    bool keyReleased(char key);

    bool keyDown(char key, bool ignoreInputEnabled = false);

    void isEnabled(bool value);

private:
    std::shared_ptr<jleWindow> windowInternal;

    bool _isEnabled = true;
};
