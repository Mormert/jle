#include "jleCore.h"

#include "EngineStatus.h"

#include "KeyboardInputInternal.h"
#include "MouseInputInternal.h"

#include "Window_GLFW_OpenGL.h"
#include "RenderingAPI_OpenGL.h"
#include "WindowInitializer_GLFW_OpenGL.h"
#include "QuadRendering_OpenGL.h"
#include "FrameBuffer_OpenGL.h"
#include "FullscreenRendering_OpenGL.h"

#include "RenderingFactory_OpenGL.h"
#include "WindowFactory_GLFW.h"

#include <iostream>

namespace jle
{

	jleCore* jleCore::core{ nullptr };

	struct CoreStatus_Internal;
	struct jleCore::jleCoreInternalImpl
	{
		std::shared_ptr<iRenderingInternalAPI> rendering_internal;
		std::shared_ptr<iWindowInternalAPI> window_internal;
		std::shared_ptr<CoreStatus_Internal> status_internal;

		CoreSettings cs;
	};

	struct CoreStatus_Internal : CoreStatus
	{
	public:
		virtual int GetFPS() override
		{
			return fps;
		}

		virtual float GetDeltaFrameTime() override
		{
			return deltaTime;
		}

		virtual float GetCurrentFrameTime() override
		{
			return currentFrame;
		}

		virtual float GetLastFrameTime() override
		{
			return lastFrame;
		}

		void Refresh()
		{
			currentFrame = jleCore::core->coreImpl->window_internal->GetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			fps = static_cast<int>(1.0 / deltaTime);
		}
	private:
		int fps = 0;
		float deltaTime = 0;
		float currentFrame = 0;
		float lastFrame = 0;
	};

	std::unique_ptr<iRenderingFactory> CreateRenderingFactory(EngineInternalAPIs& eia)
	{
		switch (eia.renderingAPI)
		{
		case EngineInternalAPIs::RenderingAPI::OPENGL_33:
			return std::make_unique<RenderingFactory_OpenGL>();
		default:
			std::cerr << "Setting rendering API failed!\n";
			exit(1);
			break;
		}
	}

	std::unique_ptr<iWindowFactory> CreateWindowFactory(EngineInternalAPIs& eia)
	{
		switch (eia.windowingAPI)
		{
		case EngineInternalAPIs::WindowAPI::GLFW:
			return std::make_unique<WindowFactory_GLFW>();
		default:
			std::cerr << "Setting window API failed!\n";
			exit(1);
			break;
		}
	}



	jleCore::jleCore(CoreSettings cs) : 
		renderingFactory { CreateRenderingFactory(cs.engineAPIs) },
		windowFactory { CreateWindowFactory(cs.engineAPIs) },
		window {windowFactory->CreateWindow()},
		rendering {renderingFactory->CreateRenderingAPI()},
		input{ std::make_shared<InputAPI>( std::make_shared<KeyboardInputInternal>(std::static_pointer_cast<iWindowInternalAPI>(window)),
											std::make_shared<MouseInputInternal>(std::static_pointer_cast<iWindowInternalAPI>(window)))},
		window_initializer{windowFactory->CreateWindowInitializer()},
		status {std::make_shared<CoreStatus_Internal>()}

	{
		coreImpl = std::make_unique<jleCoreInternalImpl>();
		coreImpl->rendering_internal = std::static_pointer_cast<iRenderingInternalAPI>(rendering);
		coreImpl->window_internal = std::static_pointer_cast<iWindowInternalAPI>(window);
		coreImpl->status_internal = std::static_pointer_cast<CoreStatus_Internal>(status);

		coreImpl->window_internal->SetWindowSettings(cs.windowSettings);

		coreImpl->cs = cs;
	}

	jleCore::~jleCore() = default;

	void jleCore::Run()
	{
		if (core != nullptr)
		{
			std::cerr << "Error: Multiple instances of jleCore\n";
			exit(1);
		}
		core = this;

		coreImpl->window_internal->InitWindow(*window_initializer, coreImpl->rendering_internal);
		coreImpl->rendering_internal->Setup(renderingFactory->CreateQuadRendering());

		running = true;
		Start(coreImpl->cs);
		Loop();
	}

	void jleCore::Loop()
	{
		while (running)
		{
			coreImpl->status_internal->Refresh();

			Render();

			Update(status->GetDeltaFrameTime());

			((iWindowInternalAPI*)window.get())->UpdateWindow();

			running = !((iWindowInternalAPI*)window.get())->WindowShouldClose();
		}
	}
}