// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleRendering.h"
#include "jleWindowAPIInterface.h"
#include "jleWindowInitializerInterface.h"

#include <memory>
#include <utility>

class jleWindowInternalAPIInterface : public jleWindowAPIInterface {
public:
    ~jleWindowInternalAPIInterface() override = default;
    ;

    virtual void InitWindow(
        jleWindowInitializerInterface& windowInitializer,
        std::shared_ptr<jleRendering> internalRenderingAPI) = 0;

    virtual void UpdateWindow() = 0;

    virtual bool WindowShouldClose() = 0;

    virtual float GetTime() = 0;

    virtual bool GetMouseClick(int button) = 0;

    virtual bool GetKey(char key) = 0;

    virtual bool GetKeyPressed(char key) = 0;

    virtual bool GetKeyReleased(char key) = 0;

    virtual float GetScrollX() = 0;

    virtual float GetScrollY() = 0;

    virtual std::pair<int, int> GetCursor() = 0;
};
