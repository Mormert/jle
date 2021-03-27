#include "jleGameEngine.h"

#include "iRenderingInternalAPI.h"

#include <iostream>

namespace jle
{

	jleGameEngine::jleGameEngine(std::shared_ptr<GameSettings> gs) : jleCore{ gs }
	{
		SetGameDimsPixels(gs->framebufferSettings.fixedAxis, gs->framebufferSettings.fixedAxisPixels);
	}

	void jleGameEngine::SetGame(std::unique_ptr<jleGame> game)
	{
		this->game = std::move(game);
	}

	void jleGameEngine::SetGameDimsPixels(FIXED_AXIS fa, unsigned int pixels)
	{
		fixed_axis = fa;
		gameDimsPixels = pixels;
	}

	std::pair<unsigned int, unsigned int> jleGameEngine::GetFramebufferDimensions(unsigned int windowWidth, unsigned int windowHeight)
	{
		if (fixed_axis == FIXED_AXIS::height)
		{
			float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

			unsigned int w = static_cast<unsigned int>(gameDimsPixels * aspect);
			return std::make_pair(w, gameDimsPixels);
		}
		else if (fixed_axis == FIXED_AXIS::width)
		{
			float aspect = static_cast<float>(windowHeight) / static_cast<float>(windowWidth);

			unsigned int h = static_cast<unsigned int>(gameDimsPixels * aspect);
			return std::make_pair(gameDimsPixels, h);
		}

		return std::make_pair(windowWidth, windowHeight);
	}

	void jleGameEngine::Start()
	{
		auto dims = GetFramebufferDimensions(core_settings->windowSettings.windowWidth, core_settings->windowSettings.windowHeight);
		framebuffer_main = renderingFactory->CreateFramebuffer(dims.first, dims.second);

		fullscreen_renderer = renderingFactory->CreateFullscreenRendering();

		window->AddWindowResizeCallback(std::bind(&jleGameEngine::FramebufferResizeEvent, this, std::placeholders::_1, std::placeholders::_2));

		//framebuffer_main->ResizeFramebuffer(200, 200);



		game->Start();
	}

	void jleGameEngine::FramebufferResizeEvent(unsigned int width, unsigned int height)
	{
		auto dims = GetFramebufferDimensions(width, height);
		framebuffer_main->ResizeFramebuffer(dims.first, dims.second);
	}

	void jleGameEngine::Update(float dt)
	{
		game->Update(dt);
	}

	void jleGameEngine::Render()
	{
		((iRenderingInternalAPI*)rendering.get())->Render(*framebuffer_main.get());
		fullscreen_renderer->RenderFramebufferFullscreen(*framebuffer_main.get(), window->GetWindowWidth(), window->GetWindowHeight());

	}
}
