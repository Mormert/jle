// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleKeyboardInputInterface.h"
#include "jleWindowInternalAPIInterface.h"
#include "jleWindowLinkableInterface.h"

class jleKeyboardInputInternal : public jleKeyboardInputInterface,
                                 public jleWindowLinkableInterface {
public:
    explicit jleKeyboardInputInternal(
        std::shared_ptr<jleWindowInternalAPIInterface> windowInternal);

    void LinkWindow(
        std::shared_ptr<jleWindowInternalAPIInterface> windowInternal) override;

    bool GetKeyPressed(char key) override;

    bool GetKeyReleased(char key) override;

    bool GetKeyDown(char key, bool ignoreInputEnabled = false) override;

private:
    std::shared_ptr<jleWindowInternalAPIInterface> windowInternal;
};
