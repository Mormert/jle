#include "GameEditorWindow.h"

#include "3rdparty/ImGui/imgui.h"
#include "3rdparty/ImGui/imgui_impl_glfw.h"
#include "3rdparty/ImGui/imgui_impl_opengl3.h"

#include "GLState.h"
#include <glad/glad.h>

namespace jle
{
	GameEditorWindow::GameEditorWindow(const std::string& window_name) :
		iEditorImGuiWindow{ window_name }
	{
	}

	void GameEditorWindow::Update(jleGameEngine& ge)
	{
		if (!isOpened)
		{
			return;
		}

        ImGuiWindowFlags flags;

        if(IsFullscreen)
        {
            flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
        }
        else
        {
            flags = ImGuiWindowFlags_NoCollapse;
        }

        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin(window_name.c_str(), &isOpened, flags);

		constexpr float negYOffset = 6;
		constexpr float negXOffset = 6;

        if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset == lastGameWindowWidth && ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset == lastGameWindowHeight))
		{
			lastGameWindowWidth = ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
			lastGameWindowHeight = ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y - negYOffset;

			auto dims = ge.GetFramebufferDimensions(static_cast<unsigned int>(ImGui::GetWindowWidth()), static_cast<unsigned int>(ImGui::GetWindowHeight()));
			ge.framebuffer_main->ResizeFramebuffer(dims.first, dims.second);
		}

		// Get the texture from the framebuffer
		glBindTexture(GL_TEXTURE_2D, (unsigned int)ge.framebuffer_main->GetTexture());
		jle::GLState::globalActiveTexture = (unsigned int)ge.framebuffer_main->GetTexture();
		ImGui::Image((void*)(intptr_t)ge.framebuffer_main->GetTexture(), ImVec2(lastGameWindowWidth, lastGameWindowHeight), ImVec2(0, 1), ImVec2(1, 0));

		if (ImGui::IsWindowFocused() != wasFocused)
		{
			wasFocused = ImGui::IsWindowFocused();
			ge.input->SetInputEnabled(wasFocused);
		}

		ImGui::End();
	}

}

