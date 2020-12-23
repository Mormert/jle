#pragma once

#include <memory>

#include "Window.h"
#include "ImGuiRenderer.h"
#include "Renderer2D.h"

namespace jle
{
	struct EngineSettings
	{
		const char* WindowTitle = "Game";
		int windowWidth = 500, windowHeight = 500;

		bool startFpsMode = false;
	};

	// Abstract class a game inherits from
	class Engine
	{
	public:
		Engine(EngineSettings engineSettings);
		void Run();

		virtual void Start() = 0;
		virtual void Update(float dt) = 0;

		Engine(const Engine& e) = delete;
		Engine& operator=(const Engine& e) = delete;
		Engine(Engine&& e) = delete;
		Engine& operator=(Engine&& e) = delete;

	private:

		void Loop();
		void CollectInput();

		bool running{ false };

		internals::Window window;
		ImGuiRenderer imGuiRenderer;

	protected:
		Renderer2D renderer;
	};
}