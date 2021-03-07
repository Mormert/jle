#include "jleGameEngine.h"

#include "iRenderingInternalAPI.h"

#include <iostream>

#include "Window_GLFW_OpenGL.h"
#include <GLFW/glfw3.h>

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

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
		if (fa == FIXED_AXIS::height)
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

		return std::make_pair(windowWidth, windowHeight);
	}

	void jleGameEngine::Start()
	{
		auto dims = GetFramebufferDimensions(fixed_axis, gameDimsPixels, core_settings->windowSettings.windowWidth, core_settings->windowSettings.windowHeight);
		framebuffer_main = renderingFactory->CreateFramebuffer(dims.first, dims.second);

		fullscreen_renderer = renderingFactory->CreateFullscreenRendering();

		window->AddWindowResizeCallback(std::bind(&jleGameEngine::FramebufferResizeEvent, this, std::placeholders::_1, std::placeholders::_2));

		framebuffer_main->ResizeFramebuffer(200, 200);

		ImGui::DebugCheckVersionAndDataLayout(IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx));
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(std::static_pointer_cast<Window_GLFW_OpenGL>(window)->GetGLFWWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
	}

	void jleGameEngine::FramebufferResizeEvent(unsigned int width, unsigned int height)
	{
		auto dims = GetFramebufferDimensions(fixed_axis, gameDimsPixels, width, height);
		framebuffer_main->ResizeFramebuffer(dims.first, dims.second);
	}

	void jleGameEngine::Update(float dt)
	{
	}

	void jleGameEngine::Render()
	{
		((iRenderingInternalAPI*)rendering.get())->Render(*framebuffer_main.get());
		fullscreen_renderer->RenderFramebufferFullscreen(*framebuffer_main.get(), window->GetWindowWidth(), window->GetWindowHeight());

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("OpenGL Texture Text");
		ImGui::Text("pointer = %p", (intptr_t)framebuffer_main->GetTexture());
		ImGui::Text("size = %d x %d", framebuffer_main->GetWidth(), framebuffer_main->GetHeight());
		ImGui::Image((void*)(intptr_t)framebuffer_main->GetTexture(), ImVec2(framebuffer_main->GetWidth() * 4, framebuffer_main->GetHeight() * 4));
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
