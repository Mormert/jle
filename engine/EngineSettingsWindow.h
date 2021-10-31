#pragma once

#include "iEditorImGuiWindow.h"
#include "EditorJsonToImgui.h"

#include <memory>

namespace jle
{

	struct jleEditorSettings;

	class EngineSettingsWindow : public iEditorImGuiWindow
	{
	public:
		EngineSettingsWindow(const std::string& window_name, std::shared_ptr<jleGameSettings> gs, std::shared_ptr<jleEditorSettings> es);
		virtual void Update(jleGameEngine& ge) override;

	private:
		std::shared_ptr<jleGameSettings> gameSettings;
		std::shared_ptr<jleEditorSettings> editorSettings;
		EditorJsonToImgui mJsonToImgui;
	};

}