// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleWindowInternalAPIInterface.h"
#include "jleWindowLinkableInterface.h"

class jleKeyboardInputInternal : public jleWindowLinkableInterface {
public:
    explicit jleKeyboardInputInternal(
        std::shared_ptr<jleWindowInternalAPIInterface> windowInternal);

    void LinkWindow(
        std::shared_ptr<jleWindowInternalAPIInterface> windowInternal) override;

    bool GetKeyPressed(char key);

    bool GetKeyReleased(char key);

    bool GetKeyDown(char key, bool ignoreInputEnabled = false);

    void SetIsEnabled(bool value);

private:
    std::shared_ptr<jleWindowInternalAPIInterface> windowInternal;

    bool _isEnabled = true;
};
