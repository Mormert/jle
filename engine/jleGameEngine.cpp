#include "jleGameEngine.h"

#include "iRenderingInternalAPI.h"

#include <iostream>

namespace jle
{
	jleGameEngine::jleGameEngine(std::shared_ptr<GameSettings> gs) : jleCore{ gs }
	{
		SetGameDimsPixels(gs->framebufferSettings.fixedAxis, gs->framebufferSettings.fixedAxisPixels);
	}

	void jleGameEngine::SetGameDimsPixels(FIXED_AXIS fa, unsigned int pixels)
	{
		fixed_axis = fa;
		gameDimsPixels = pixels;
	}

	std::pair<unsigned int, unsigned int> GetFramebufferDimensions(FIXED_AXIS fa, unsigned int pixels_along_axis, unsigned int windowWidth, unsigned int windowHeight)
	{
		if (fa == FIXED_AXIS::same_as_window)
		{
			return std::make_pair(windowWidth, windowHeight);
		}
		else if (fa == FIXED_AXIS::height)
		{
			float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

			unsigned int w = static_cast<unsigned int>(pixels_along_axis * aspect);
			return std::make_pair(w, pixels_along_axis);
		}
		else if (fa == FIXED_AXIS::width)
		{
			float aspect = static_cast<float>(windowHeight) / static_cast<float>(windowWidth);

			unsigned int h = static_cast<unsigned int>(pixels_along_axis * aspect);
			return std::make_pair(pixels_along_axis, h);
		}
	}

	void jleGameEngine::Start()
	{
		auto dims = GetFramebufferDimensions(fixed_axis, gameDimsPixels, core_settings->windowSettings.windowWidth, core_settings->windowSettings.windowHeight);
		std::cout << dims.first << " " << dims.second << '\n';
		framebuffer_main = renderingFactory->CreateFramebuffer(dims.first, dims.second);

		fullscreen_renderer = renderingFactory->CreateFullscreenRendering();

		window->AddWindowResizeCallback(std::bind(&jleGameEngine::FramebufferResizeEvent, this, std::placeholders::_1, std::placeholders::_2));
	}

	void jleGameEngine::FramebufferResizeEvent(unsigned int width, unsigned int height)
	{
		auto dims = GetFramebufferDimensions(fixed_axis, gameDimsPixels, width, height);
		std::cout << dims.first << " " << dims.second << '\n';
		framebuffer_main->ResizeFramebuffer(dims.first, dims.second);
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
