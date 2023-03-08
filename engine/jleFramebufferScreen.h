// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleFrameBufferInterface.h"

// A framebuffer that renders to the screen. Thus, it does not contain a depth buffer.
class jleFramebufferScreen : public jleFramebufferInterface
{
public:
    jleFramebufferScreen(unsigned int width, unsigned int height);

    ~jleFramebufferScreen() override;

    void resize(unsigned int width, unsigned int height) override;
};
