#include "EditorWindowsPanel.h"

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

namespace jle
{
	EditorWindowsPanel::EditorWindowsPanel(const std::string& window_name) :
		iEditorImGuiWindow{ window_name }
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
				ImGui::EndMenu();
			}

			ImGui::Text("FPS: %4d, DT: %4f, Time: %4f", ge.status->GetFPS(), ge.status->GetDeltaFrameTime(), ge.status->GetCurrentFrameTime());

			ImGui::EndMainMenuBar();
		}
	}

	void EditorWindowsPanel::AddWindow(std::shared_ptr<iEditorImGuiWindow> window)
	{
		windows.push_back(window);
	}
}