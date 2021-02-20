#include "OpenGLRenderingFactory.h"


#include "OpenGLFrameBuffer.h"
#include "OpenGLQuadRendering.h"
#include "OpenGLRenderingAPI.h"
#include "OpenGLFullscreenRendering.h"

#include <memory>

std::unique_ptr<iRenderingAPI> OpenGLRenderingFactory::CreateRenderingAPI()
{
    return std::make_unique<OpenGLRenderingAPI>();
}

std::unique_ptr<iQuadRendering> OpenGLRenderingFactory::CreateQuadRendering()
{
    return std::make_unique<OpenGLQuadRendering>();
}

std::unique_ptr<iFramebuffer> OpenGLRenderingFactory::CreateFramebuffer(unsigned int width, unsigned int height)
{
    return std::make_unique<OpenGLFrameBuffer>(width, height);
}

std::unique_ptr<iFullscreenRendering> OpenGLRenderingFactory::CreateFullscreenRendering()
{
    return std::make_unique<OpenGLFullscreenRendering>();
}
