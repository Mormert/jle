// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCamera.h"
#include "jleFramebufferInterface.h"
#include "jleRenderingFactoryInterface.h"

class jleRenderingInternalAPIInterface : public jleRenderingAPIInterface {
public:
    ~jleRenderingInternalAPIInterface() override = default;
    ;

    virtual void SetViewportDimensions(
        int x,
        int y,
        unsigned int width,
        unsigned int height) = 0; // glviewport...

    virtual void Setup(const jleRenderingFactoryInterface& renderFactory) = 0;

    virtual void Render(jleFramebufferInterface& framebufferOut, jleCamera& camera) = 0;

    virtual void ClearBuffersForNextFrame() = 0;
};
