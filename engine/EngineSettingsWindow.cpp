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
	nlohmann::json j_gs = *gs;
	mJsonToImguiGS.JsonToImgui(j_gs);

	nlohmann::json j_es = *es;
	mJsonToImguiES.JsonToImgui(j_es);
}

void jle::EngineSettingsWindow::Update(jleGameEngine& ge)
{
	if (!isOpened)
	{
		return;
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(window_name.c_str(), &isOpened, flags);

	mJsonToImguiGS.DrawAndGetInput();
	mJsonToImguiES.DrawAndGetInput();

	if (ImGui::Button("Save Settings"))
	{
		auto jsonGS = mJsonToImguiGS.ImGuiToJson();
		cfg::SaveEngineConfig(cfg::GameSettingsName, jsonGS);

		auto jsonES = mJsonToImguiES.ImGuiToJson();
		cfg::SaveEngineConfig(cfg::EngineSettingsName, jsonES);
	}

	ImGui::End();
}
