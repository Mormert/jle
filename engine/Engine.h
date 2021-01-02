#pragma once

#include <memory>

#include "Window.h"
#include "Camera2D.h"
#include "ImGuiRenderer.h"
#include "Renderer2D.h"

namespace jle
{
	struct EngineSettings
	{
		const char* WindowTitle = "Game";
		int windowWidth = 500, windowHeight = 500;
		unsigned int viewportWidth = 100, viewportHeight = 100;

		bool startFpsMode = false;
	};

	// Abstract class a game inherits from
	class Engine
	{
	public:
		Engine(EngineSettings es);
		virtual ~Engine(){}
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

	protected:

		// Entry point for a game to access the public Window methods
		Window window;
		// Entry point for a game to specify the properties of the 2D camera
		Camera2D camera;
		//ImGuiRenderer imGuiRenderer;
		// Entry point for a game to render graphics to the game world and to the UI
		Renderer2D renderer;
	};
}