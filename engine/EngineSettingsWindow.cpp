#include "EngineSettingsWindow.h"
#include "jleConfigUtils.h"

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"


jle::EngineSettingsWindow::EngineSettingsWindow(const std::string& window_name,
	std::shared_ptr<jleGameSettings> gs,
	std::shared_ptr<jleEditorSettings> es) :
	iEditorImGuiWindow{ window_name }, editorSettings{ es }
{
	nlohmann::json j = *gs;
	mJsonToImgui.JsonToImgui(j);
}

void jle::EngineSettingsWindow::Update(jleGameEngine& ge)
{
	if (!isOpened)
	{
		return;
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(window_name.c_str(), &isOpened, flags);

	mJsonToImgui.DrawAndGetInput();

	if (ImGui::Button("Save Settings"))
	{
		auto json = mJsonToImgui.ImGuiToJson();
		cfg::SaveEngineConfig(cfg::GameSettingsName, json);
	}

	ImGui::End();
}
