#include "GameEditorWindow.h"

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

#include "GLState.h"
#include <glad/glad.h>

namespace jle
{

	void GameEditorWindow::Update(jleGameEngine& ge)
	{

		ImGui::Begin("Game Window");



		constexpr int negYOffset = 6;
		constexpr int negXOffset = 6;
		if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == lastGameWindowWidth && ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == lastGameWindowHeight))
		{
			lastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
			lastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

			auto dims = ge.GetFramebufferDimensions(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
			ge.framebuffer_main->ResizeFramebuffer(dims.first, dims.second);
		}

		//ImGui::Text("%f , %f", ImGui::GetCursorStartPos().x, ImGui::GetCursorStartPos().y);
		//ImGui::Text("%f , %f", ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);


		//std::cout << ImGui::GetWindowHeight() << '\n';

		//ImGui::Text("pointer = %p", (intptr_t)framebuffer_main->GetTexture());
		//ImGui::Text("size = %d x %d", framebuffer_main->GetWidth(), framebuffer_main->GetHeight());

		// Get the texture from the framebuffer
		glBindTexture(GL_TEXTURE_2D, (unsigned int)ge.framebuffer_main->GetTexture());
		glStaticState.globalActiveTexture = (unsigned int)ge.framebuffer_main->GetTexture();
		ImGui::Image((void*)(intptr_t)ge.framebuffer_main->GetTexture(), ImVec2(lastGameWindowWidth, lastGameWindowHeight), ImVec2(0, 1), ImVec2(1, 0));

		if (ImGui::IsWindowFocused() != wasFocused)
		{
			wasFocused = ImGui::IsWindowFocused();
			ge.input->SetInputEnabled(wasFocused);
		}

		ImGui::End();
	}

}

