#include "RenderingFactory_OpenGL.h"


#include "FrameBuffer_OpenGL.h"
#include "QuadRendering_OpenGL.h"
#include "TextRendering_OpenGL.h"
#include "RenderingAPI_OpenGL.h"
#include "FullscreenRendering_OpenGL.h"

#include <memory>

namespace jle
{
    std::unique_ptr<iRenderingAPI> RenderingFactory_OpenGL::CreateRenderingAPI() const
    {
        return std::make_unique<RenderingAPI_OpenGL>();
    }

    std::unique_ptr<iQuadRendering> RenderingFactory_OpenGL::CreateQuadRendering() const
    {
        return std::make_unique<QuadRendering_OpenGL>();
    }

    std::unique_ptr<iTextRendering> RenderingFactory_OpenGL::CreateTextRendering() const
    {
        return std::make_unique<TextRendering_OpenGL>();
    }

    std::unique_ptr<iFramebuffer> RenderingFactory_OpenGL::CreateFramebuffer(unsigned int width, unsigned int height) const
    {
        return std::make_unique<Framebuffer_OpenGL>(width, height);
    }

    std::unique_ptr<iFullscreenRendering> RenderingFactory_OpenGL::CreateFullscreenRendering() const
    {
        return std::make_unique<FullscreenRendering_OpenGL>();
    }
}