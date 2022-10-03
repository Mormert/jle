// Copyright (c) 2022. Johan Lind

#pragma once

#include <memory>

class jleWindow;

class jleKeyboardInput {
public:
    explicit jleKeyboardInput(
        std::shared_ptr<jleWindow> windowInternal);

    void LinkWindow(std::shared_ptr<jleWindow> windowInternal);

    bool GetKeyPressed(char key);

    bool GetKeyReleased(char key);

    bool GetKeyDown(char key, bool ignoreInputEnabled = false);

    void SetIsEnabled(bool value);

private:
    std::shared_ptr<jleWindow> windowInternal;

    bool _isEnabled = true;
};
