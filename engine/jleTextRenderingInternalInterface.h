// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleCamera.h"
#include "jleFramebufferInterface.h"
#include "jleTextRenderingInterface.h"

class jleTextRenderingInternalInterface : public jleTextRenderingInterface {
public:
    ~jleTextRenderingInternalInterface() override = default;

    // Renders to an output framebuffer
    virtual void Render(jleFramebufferInterface& framebufferOut,
                        const jleCamera& camera) = 0;

    virtual void ClearBuffersForNextFrame() = 0;
};
