#include "jleGameEngine.h"

#include "iRenderingInternalAPI.h"

jle::jleGameEngine::jleGameEngine(CoreSettings cs) : jleCore{ cs }
{
}

void jle::jleGameEngine::Start(CoreSettings cs)
{
	framebuffer_main = renderingFactory->CreateFramebuffer(cs.windowSettings.windowWidth, cs.windowSettings.windowHeight);
	fullscreen_renderer = renderingFactory->CreateFullscreenRendering();
}

void jle::jleGameEngine::Update(float dt)
{
}

void jle::jleGameEngine::Render()
{
	((iRenderingInternalAPI*)rendering.get())->Render(*framebuffer_main.get(), window->GetWindowWidth(), window->GetWindowHeight());
	fullscreen_renderer->RenderFramebufferFullscreen(*framebuffer_main.get(), window->GetWindowWidth(), window->GetWindowHeight());
}
