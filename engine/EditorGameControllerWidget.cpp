#include "EditorGameControllerWidget.h"

#include "3rdparty/ImGui/imgui.h"
#include "3rdparty/ImGui/imgui_impl_glfw.h"
#include "3rdparty/ImGui/imgui_impl_opengl3.h"

#include <plog/Log.h>

namespace jle
{
	EditorGameControllerWidget::EditorGameControllerWidget(const std::string& window_name) :
		iEditorImGuiWindow{window_name}
	{
        mPlayGameIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath("EditorResources/icons/play.png");
        mRestartGameIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath("EditorResources/icons/replay.png");
        mPauseGameIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath("EditorResources/icons/pause.png");
        mNextFrameIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath("EditorResources/icons/next_frame.png");
    }

	void EditorGameControllerWidget::Update(jleGameEngine& ge)
	{

        const ImVec2 iconSize{ImGui::GetWindowHeight() - 3,ImGui::GetWindowHeight() - 3};

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
}