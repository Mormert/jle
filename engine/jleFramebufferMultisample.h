// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFrameBufferInterface.h"

// A framebuffer that supports anti-aliasing (MSAA)
class jleFramebufferMultisample : public jleFramebufferInterface
{
public:
    jleFramebufferMultisample(unsigned int width, unsigned int height, unsigned int msaaSamples);

    ~jleFramebufferMultisample() override;

    void resize(unsigned int width, unsigned int height) override;

    void setSamples(unsigned int samples);

private:
    unsigned int _colorRenderbuffer{};
    unsigned int _depthRenderbuffer{};

    unsigned int _msaaSamples{4};
};
