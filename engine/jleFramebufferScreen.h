// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleFrameBufferInterface.h"

// A framebuffer that renders to the screen. Thus, it does NOT contain a depth buffer.
// Do not use this for rendering 3D objects directly to the framebuffer!
class jleFramebufferScreen : public jleFramebufferInterface
{
public:
    jleFramebufferScreen(unsigned int width, unsigned int height);

    ~jleFramebufferScreen() override;

    void resize(unsigned int width, unsigned int height) override;
};
