// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleFrameBufferInterface.h"

// The actual framebuffer that is created by GLFW for us
class jleFramebufferGameScreen : public jleFramebufferInterface
{
public:
    jleFramebufferGameScreen(unsigned int width, unsigned int height);

    ~jleFramebufferGameScreen() override;

    void resize(unsigned int width, unsigned int height) override;
};
