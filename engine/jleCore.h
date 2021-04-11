#pragma once

#include "no_copy_no_move.h"

#include <memory>

//#include "Window.h"
#include "iWindowAPI.h"
//#include "Camera2D.h"
//#include "DebugRenderer.h"
#include "InputAPI.h"
#include "iRenderingAPI.h"
#include "iFramebuffer.h"
#include "iTextureCreator.h"
#include "iRenderingFactory.h"
#include "iWindowFactory.h"
#include "iWindowInitializer.h"

namespace jle
{

	struct CoreStatus
	{
		virtual ~CoreStatus() {}
		virtual int GetFPS() = 0;
		virtual float GetDeltaFrameTime() = 0;
		virtual float GetCurrentFrameTime() = 0;
		virtual float GetLastFrameTime() = 0;
	};


	struct EngineInternalAPIs
	{
		enum class RenderingAPI{ OPENGL_33 } renderingAPI = RenderingAPI::OPENGL_33;
		enum class WindowAPI{ GLFW } windowingAPI = WindowAPI::GLFW;
	};

	struct CoreSettings
	{
		WindowSettings windowSettings;
		EngineInternalAPIs engineAPIs;

		virtual ~CoreSettings() {}
	};

	// Core part of the jle engine
	class jleCore
	{
	public:
		NO_COPY_NO_MOVE(jleCore)

		jleCore(std::shared_ptr<CoreSettings> cs);
		virtual ~jleCore();

		void Run();

		const std::unique_ptr<iRenderingFactory> renderingFactory;
		const std::unique_ptr<iWindowFactory> windowFactory;

		// Singleton
		static jleCore* core;

		// Entry point for a user to access the windowing API
		const std::shared_ptr<iWindowAPI> window;

		// Entry point for a user to access the input API
		const std::shared_ptr<InputAPI> input;

		// Entry point for a user to do fundamental rendering
		const std::shared_ptr<iRenderingAPI> rendering;

		// Entry point for a user to create textures of different kinds
		const std::shared_ptr<iTextureCreator> texture_creator;

		// Entry point for a user to get core status
		const std::shared_ptr<CoreStatus> status;

	private:
		void Loop();
		bool running{ false };

		const std::unique_ptr<iWindowInitializer> window_initializer;

		// Internal impl data
		struct jleCoreInternalImpl;
		std::unique_ptr<jleCoreInternalImpl> coreImpl;

		friend struct CoreStatus_Internal;

	protected:
		virtual void Start() {}
		virtual void Update(float dt) {}
		virtual void Render() {}
		virtual void Exiting() {}

		std::shared_ptr<CoreSettings> core_settings;
	};
}