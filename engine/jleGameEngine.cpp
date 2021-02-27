#include "jleGameEngine.h"

#include "iRenderingInternalAPI.h"

namespace jle
{
	jleGameEngine::jleGameEngine(CoreSettings cs) : jleCore{ cs }
	{
	}

	void jleGameEngine::Start(CoreSettings cs)
	{
		framebuffer_main = renderingFactory->CreateFramebuffer(cs.windowSettings.windowWidth, cs.windowSettings.windowHeight);
		fullscreen_renderer = renderingFactory->CreateFullscreenRendering();
	}

	void jleGameEngine::Update(float dt)
	{
	}

	void jleGameEngine::Render()
	{
		((iRenderingInternalAPI*)rendering.get())->Render(*framebuffer_main.get(), window->GetWindowWidth(), window->GetWindowHeight());
		fullscreen_renderer->RenderFramebufferFullscreen(*framebuffer_main.get(), window->GetWindowWidth(), window->GetWindowHeight());
	}
}
