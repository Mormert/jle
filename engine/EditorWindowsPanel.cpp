#include "EditorWindowsPanel.h"

#include "GameEditorWindow.h"

#include "3rdparty/ImGui/imgui.h"
#include "3rdparty/ImGui/imgui_impl_glfw.h"
#include "3rdparty/ImGui/imgui_impl_opengl3.h"

namespace jle
{
	EditorWindowsPanel::EditorWindowsPanel(const std::string& window_name) :
		iEditorImGuiWindow{ window_name }, mGameController{"Game Controller"}
	{
	}

	void EditorWindowsPanel::Update(jleGameEngine& ge)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Manage Windows"))
			{
				for (auto window : windows)
				{
					if (window->GetOpened())
					{
						if (ImGui::MenuItem(("Close " + window->GetWindowName()).c_str()))
						{
							window->CloseWindow();
						}						
					}
					else
					{
						if (ImGui::MenuItem(("Open " + window->GetWindowName()).c_str()))
						{
							window->OpenWindow();
						}
					}
				}

                ImGui::Checkbox("Game Fullscreen", &GameEditorWindow::IsFullscreen);

				ImGui::EndMenu();
			}


            auto windowWidth = ImGui::GetWindowSize().x;
            ImGui::SetCursorPosX((windowWidth) * 0.5f);

            mGameController.Update(ge);

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize("FPS: XXXX, DT: XXXXXXXX, Time: XXXXX").x
                                 - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
			ImGui::Text("FPS: %4d, DT: %4f, Time: %4f", ge.status->GetFPS(), ge.status->GetDeltaFrameTime(), ge.status->GetCurrentFrameTime());

			ImGui::EndMainMenuBar();
		}
	}

	void EditorWindowsPanel::AddWindow(std::shared_ptr<iEditorImGuiWindow> window)
	{
		windows.push_back(window);
	}
}