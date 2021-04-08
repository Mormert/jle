#include "EditorGameControllerWindow.h"

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

#include <plog/Log.h>

namespace jle
{
	EditorGameControllerWindow::EditorGameControllerWindow(const std::string& window_name) :
		iEditorImGuiWindow{window_name}
	{
	}

	void EditorGameControllerWindow::Update(jleGameEngine& ge)
	{
		if (!isOpened)
		{
			return;
		}

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;

		if (ImGui::Begin(window_name.c_str(), &isOpened, flags))
		{
			if (ImGui::Button("Start Game", { 100, 25 }))
			{
				StartGame();
			}

			ImGui::SameLine();
			if (ImGui::Button("Stop Game", { 100, 25 }))
			{
				StopGame();
			}

			ImGui::SameLine();
			if (ImGui::Button("Pause Game", { 100, 25 }))
			{
				PauseGame();
			}
		}

		ImGui::End();
	}

	void EditorGameControllerWindow::StartGame()
	{
		PLOG_INFO << "Starting the game.";
	}

	void EditorGameControllerWindow::StopGame()
	{
		PLOG_INFO << "Stopping the game.";
	}

	void EditorGameControllerWindow::PauseGame()
	{
		PLOG_INFO << "Pausing the game.";
	}
}