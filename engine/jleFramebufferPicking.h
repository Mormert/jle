// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFrameBufferInterface.h"

class jleFramebufferPicking : public jleFramebufferInterface
{
public:
    jleFramebufferPicking(unsigned int width, unsigned int height);

    ~jleFramebufferPicking() override;

    void resize(unsigned int width, unsigned int height) override;

private:
    unsigned int _depthRenderbuffer{};
};
