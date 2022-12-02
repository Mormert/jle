// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFrameBufferInterface.h"

// A framebuffer that light sources render to for shadow mapping
class jleFramebufferShadowCubeMap : public jleFramebufferInterface
{
public:
    jleFramebufferShadowCubeMap(unsigned int width, unsigned int height);

    ~jleFramebufferShadowCubeMap() override;

    void resize(unsigned int width, unsigned int height) override;

    void setRenderFace(unsigned int index);
};
