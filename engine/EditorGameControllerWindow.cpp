#include "EditorGameControllerWindow.h"

#include "3rdparty/ImGui/imgui.h"
#include "3rdparty/ImGui/imgui_impl_glfw.h"
#include "3rdparty/ImGui/imgui_impl_opengl3.h"

#include <plog/Log.h>

namespace jle
{
	EditorGameControllerWindow::EditorGameControllerWindow(const std::string& window_name) :
		iEditorImGuiWindow{window_name}
	{
        mPlayGameIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath("EditorResources/icons/play.png");
        mRestartGameIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath("EditorResources/icons/replay.png");
        mPauseGameIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath("EditorResources/icons/pause.png");
        mNextFrameIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath("EditorResources/icons/next_frame.png");
    }

	void EditorGameControllerWindow::Update(jleGameEngine& ge)
	{
		if (!isOpened)
		{
			return;
		}

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;

        const ImVec2 iconSize {52, 52};

		if (ImGui::Begin(window_name.c_str(), &isOpened, flags))
		{
			if (ge.IsGameKilled())
			{
                if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(mPlayGameIcon->GetTextureID()), iconSize))
				{
                    LOG_VERBOSE << "Starting the game.";
					ge.StartGame();
				}
			}
			else
			{
                if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(mRestartGameIcon->GetTextureID()), iconSize))
                {
                    LOG_VERBOSE << "Restarting the game.";
                    ge.RestartGame();
				}
			}

			ImGui::SameLine();


			if (!ge.IsGameHalted())
			{
                if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(mPauseGameIcon->GetTextureID()), iconSize))
                {
                    LOG_VERBOSE << "Pausing the game.";
                    ge.HaltGame();
				}
			}
			else
			{
                if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(mPlayGameIcon->GetTextureID()), iconSize))
                {
                    LOG_VERBOSE << "Continue playing the game.";
                    ge.UnhaltGame();
				}
				ImGui::SameLine();
                if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(mNextFrameIcon->GetTextureID()), iconSize))
                {
                    LOG_VERBOSE << "Going one frame forward.";
                    if (!ge.IsGameKilled())
					{
						ge.ExecuteNextFrame();
					}
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