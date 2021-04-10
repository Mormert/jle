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

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;

		if (ImGui::Begin(window_name.c_str(), &isOpened, flags))
		{
			if (ImGui::Button("Start Game", { 100, 25 }))
			{
				ge.StartGame();
			}

			if (ImGui::Button("Kill Game", { 100, 25 }))
			{
				ge.KillGame();
			}

			ImGui::SameLine();
			
			if (!ge.IsGameHalted())
			{
				if (ImGui::Button("Halt Game", { 100, 25 }))
				{
					ge.HaltGame();
				}
			}
			else
			{
				if (ImGui::Button("Unhalt Game", { 100, 25 }))
				{
					ge.UnhaltGame();
				}
				ImGui::SameLine();
				if (ImGui::Button("Next Frame", { 100, 25 }))
				{
					ge.ExecuteNextFrame();
				}
			}
		}

		ImGui::End();
	}

	inline void EditorGameControllerWindow::StartGame()
	{
		PLOG_INFO << "Starting the game.";
	}

	inline void EditorGameControllerWindow::StopGame()
	{
		PLOG_INFO << "Stopping the game.";
	}

	inline void EditorGameControllerWindow::PauseGame()
	{
		PLOG_INFO << "Pausing the game.";
	}
}