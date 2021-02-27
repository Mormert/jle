#include "jleCore.h"

#include "EngineStatus.h"

#include "KeyboardInputInternal.h"
#include "MouseInputInternal.h"

#include "GLFWWindowOpenGL.h"
#include "OpenGLRenderingAPI.h"
#include "GLFWOpenGL33WindowInitializer.h"
#include "OpenGLQuadRendering.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLFullscreenRendering.h"

#include "OpenGLRenderingFactory.h"
#include "GLFWWindowFactory.h"

#include <iostream>

namespace jle
{

	jleCore* jleCore::core { nullptr };

	std::unique_ptr<iRenderingFactory> CreateRenderingFactory(EngineInternalAPIs& eia)
	{
		switch (eia.renderingAPI)
		{
		case EngineInternalAPIs::RenderingAPI::OPENGL_33:
			return std::make_unique<OpenGLRenderingFactory>();
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
			return std::make_unique<GLFWWindowFactory>();
		default:
			std::cerr << "Setting window API failed!\n";
			exit(1);
			break;
		}
	}

	struct jleCore::jleCoreInternalImpl
	{
		std::shared_ptr<iRenderingInternalAPI> rendering_internal;
		std::shared_ptr<iWindowInternalAPI> window_internal;

		CoreSettings cs;
	};

	jleCore::jleCore(CoreSettings cs) : 
		renderingFactory { CreateRenderingFactory(cs.engineAPIs) },
		windowFactory { CreateWindowFactory(cs.engineAPIs) },
		window {windowFactory->CreateWindow()},
		rendering {renderingFactory->CreateRenderingAPI()},
		input{ std::make_shared<InputAPI>( std::make_shared<KeyboardInputInternal>(std::static_pointer_cast<iWindowInternalAPI>(window)),
											std::make_shared<MouseInputInternal>(std::static_pointer_cast<iWindowInternalAPI>(window)))},
		window_initializer{windowFactory->CreateWindowInitializer()}

	{
		coreImpl = std::make_unique<jleCoreInternalImpl>();
		coreImpl->rendering_internal = std::static_pointer_cast<iRenderingInternalAPI>(rendering);
		coreImpl->window_internal = std::static_pointer_cast<iWindowInternalAPI>(window);

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
			EngineStatus::UpdateEngineStatus();

			Render();

			Update(static_cast<float>(EngineStatus::deltaTime));

			((iWindowInternalAPI*)window.get())->UpdateWindow();

			running = !((iWindowInternalAPI*)window.get())->WindowShouldClose();
		}
	}
}