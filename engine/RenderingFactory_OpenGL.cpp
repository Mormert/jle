#include "RenderingFactory_OpenGL.h"


#include "FrameBuffer_OpenGL.h"
#include "QuadRendering_OpenGL.h"
#include "RenderingAPI_OpenGL.h"
#include "FullscreenRendering_OpenGL.h"

#include <memory>

std::unique_ptr<iRenderingAPI> RenderingFactory_OpenGL::CreateRenderingAPI()
{
    return std::make_unique<RenderingAPI_OpenGL>();
}

std::unique_ptr<iQuadRendering> RenderingFactory_OpenGL::CreateQuadRendering()
{
    return std::make_unique<QuadRendering_OpenGL>();
}

std::unique_ptr<iFramebuffer> RenderingFactory_OpenGL::CreateFramebuffer(unsigned int width, unsigned int height)
{
    return std::make_unique<Framebuffer_OpenGL>(width, height);
}

std::unique_ptr<iFullscreenRendering> RenderingFactory_OpenGL::CreateFullscreenRendering()
{
    return std::make_unique<FullscreenRendering_OpenGL>();
}
