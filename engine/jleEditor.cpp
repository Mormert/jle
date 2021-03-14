#include "jleEditor.h"

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

#include "Window_GLFW_OpenGL.h"
#include <GLFW/glfw3.h>

namespace jle
{
	void jleEditor::StartEditor()
	{
	}

	void jleEditor::Start()
	{

		ImGui::DebugCheckVersionAndDataLayout(IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx));
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(std::static_pointer_cast<Window_GLFW_OpenGL>(window)->GetGLFWWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		jleGameEngine::Start();

	}

	void jleEditor::Render()
	{
		jleGameEngine::Render();


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

