#pragma once

#include "iEditorImGuiWindow.h"
#include "EditorJsonToImgui.h"

namespace jle
{
	class EditorSceneObjectsWindow : public iEditorImGuiWindow
	{
	public:
		EditorSceneObjectsWindow(const std::string& window_name);
		virtual void Update(jleGameEngine& ge) override;

	private:
		EditorJsonToImgui mJsonToImgui;
	};
}