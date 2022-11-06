// Copyright (c) 2022. Johan Lind

#pragma once

#include "jle3DRenderer.h"
#include "jleCamera.h"
#include "jleFrameBuffer.h"
#include "jleQuadRendering.h"
#include "jleTextRendering.h"
#include <memory>

class jleFramebuffer;

class jleRendering {
public:
    ~jleRendering() = default;

    void setup();

    void viewportDimensions(int x,
                            int y,
                            unsigned int width,
                            unsigned int height);

    void render(jleFramebuffer &framebufferOut, const jleCamera &camera);

    void clearBuffersForNextFrame();

    jleQuadRendering &quads();
    jle3DRenderer &rendering3d();
    jleTextRendering &texts();

private:
    std::unique_ptr<jleQuadRendering> _quads;
    std::unique_ptr<jleTextRendering> _texts;
    std::unique_ptr<jle3DRenderer> _3dRenderer;
};
