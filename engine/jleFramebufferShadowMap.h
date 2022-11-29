// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFrameBufferInterface.h"

// A framebuffer that light sources render to for shadow mapping
class jleFramebufferShadowMap : public jleFramebufferInterface
{
public:
    jleFramebufferShadowMap(unsigned int width, unsigned int height);

    ~jleFramebufferShadowMap() override;

    void resize(unsigned int width, unsigned int height) override;
};
