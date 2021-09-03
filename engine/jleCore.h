#pragma once

#include "no_copy_no_move.h"
#include "iWindowAPI.h"
#include "InputAPI.h"
#include "iRenderingAPI.h"
#include "iFramebuffer.h"
#include "iTextureCreator.h"
#include "iRenderingFactory.h"
#include "iWindowFactory.h"
#include "iWindowInitializer.h"

#include "3rdparty/json.hpp"

#include <memory>

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

#pragma region EngineInternalAPIs to/from json 
	using json = nlohmann::json;
	inline void to_json(nlohmann::json& j, const EngineInternalAPIs& ei)
	{
		j = json{ {"rendering_api",ei.renderingAPI}, {"windowing_api", ei.windowingAPI } };
	}

	inline void from_json(const nlohmann::json& j, EngineInternalAPIs& ei)
	{
		j.at("rendering_api").get_to(ei.renderingAPI);
		j.at("windowing_api").get_to(ei.windowingAPI);
	}
#pragma endregion

#pragma region CoreSettings to/from json 
	using json = nlohmann::json;
	inline void to_json(nlohmann::json& j, const CoreSettings& cs)
	{
		j = json{ {"window_settings", cs.windowSettings}, {"engine_abstractions", cs.engineAPIs} };
	}

	inline void from_json(const nlohmann::json& j, CoreSettings& cs)
	{
		j.at("window_settings").get_to(cs.windowSettings);
		j.at("engine_abstractions").get_to(cs.engineAPIs);
	}
#pragma endregion

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