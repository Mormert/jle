#include "ImGuiRenderer.h"

#include "InputState.h"
#include "EngineStatus.h"

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

#include <iostream>

namespace jle
{

	void RenderDebugInfo();

	ImGuiRenderer::ImGuiRenderer(GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330 core");
		// Setup Dear ImGui style
		ImGui::StyleColorsClassic();
	}

	ImGuiRenderer::~ImGuiRenderer()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiRenderer::Render()
	{

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		if (InputState::state.showDebugInfo)
		{
			RenderDebugInfo();
		}


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	}

	void RenderDebugInfo()
	{

		constexpr ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove;
		if (ImGui::Begin("Debug info", nullptr, window_flags))
		{
			ImGui::Text("FPS : %d (%f ms)", EngineStatus::GetFps(), EngineStatus::GetDeltaTime());
			ImGui::Text("Last Pressed  : %c (%d)", InputState::debugState.lastPressedButton, InputState::debugState.lastPressedButton);
			ImGui::Text("Last Released : %c (%d)", InputState::debugState.lastReleasedButton, InputState::debugState.lastReleasedButton);
		}
		ImGui::End();

	}
}

