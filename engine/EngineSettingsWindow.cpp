#include "EngineSettingsWindow.h"

// include jleEditor to include EditorSettings
#include "jleEditor.h"

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

jle::EngineSettingsWindow::EngineSettingsWindow(const std::string& window_name, std::shared_ptr<jleEditorSettings> es) :
	iEditorImGuiWindow{ window_name }, editorSettings{ es }
{
}

void jle::EngineSettingsWindow::Update(jleGameEngine& ge)
{
	if (!isOpened)
	{
		return;
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(window_name.c_str(), &isOpened, flags);

	// TODO implement GUI to edit settings

	ImGui::End();
}
