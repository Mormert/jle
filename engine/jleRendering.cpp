// Copyright (c) 2023. Johan Lind

/*



#include "jleIncludeGL.h"
#include "jleGLError.h"

#include "jleFramebufferMultisample.h"
#include "jleProfiler.h"
#include "jleQuadRendering.h"
#include "jleTextRendering.h"
#include <plog/Log.h>

void
jleRendering::viewportDimensions(int x, int y, unsigned int width, unsigned int height)
{
    glViewport(x, y, static_cast<int>(width), static_cast<int>(height));
}

void
jleRendering::render(jleFramebufferInterface &framebufferOut, const jleCamera &camera)
{
    JLE_SCOPE_PROFILE_CPU(jleRendering_render)

    framebufferOut.bind();

    const auto backgroundColor = camera.getBackgroundColor();
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glCheckError("Render");

    _3dRenderer->render(framebufferOut, camera);

    glCheckError("3D Render Queue");
}

void
jleRendering::renderMSAA(jleFramebufferInterface &framebufferOut,
                         jleFramebufferMultisample &msaaIn,
                         const jleCamera &camera)
{

    JLE_SCOPE_PROFILE_CPU(jleRendering_renderMSAA)

    render(msaaIn, camera);

    glCheckError("Render");

    msaaIn.blitToOther(framebufferOut);

    glCheckError("MSAA Blit");
}

void
jleRendering::setup()
{
    LOG_VERBOSE << "Creating text rendering";
    this->_texts = std::make_unique<jleTextRendering>();
    LOG_VERBOSE << "Creating quad rendering";
    this->_quads = std::make_unique<jleQuadRendering>();
    LOG_VERBOSE << "Creating 3D rendering";
    this->_3dRenderer = std::make_unique<jle3DRenderer>();
}

void
jleRendering::clearBuffersForNextFrame()
{
    _quads->clearBuffersForNextFrame();
    _texts->clearBuffersForNextFrame();
}

jleQuadRendering &
jleRendering::quads()
{
    return *_quads;
}

jleTextRendering &
jleRendering::texts()
{
    return *_texts;
}
jle3DRenderer &
jleRendering::rendering3d()
{
    return *_3dRenderer;
}

*/