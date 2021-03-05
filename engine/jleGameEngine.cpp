#include "jleGameEngine.h"

#include "iRenderingInternalAPI.h"

#include <iostream>

namespace jle
{
	jleGameEngine::jleGameEngine(CoreSettings cs) : jleCore{ cs }
	{
	}

	void jleGameEngine::Start(CoreSettings cs)
	{
		framebuffer_main = renderingFactory->CreateFramebuffer(cs.windowSettings.windowWidth, cs.windowSettings.windowHeight);
		fullscreen_renderer = renderingFactory->CreateFullscreenRendering();

		window->SetWindowResizeCallback(std::bind(&jleGameEngine::FramebufferResizeEvent, this, std::placeholders::_1, std::placeholders::_2));
	}

	void jleGameEngine::FramebufferResizeEvent(unsigned int width, unsigned int height)
	{
		std::cout << "Window resize callback\n";
		framebuffer_main->ResizeFramebuffer(width, height);
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
