#pragma once

#include "iEditorImGuiWindow.h"

namespace jle
{
	class EditorSceneObjectsWindow : public iEditorImGuiWindow
	{
	public:
		EditorSceneObjectsWindow(const std::string& window_name);
		virtual void Update(jleGameEngine& ge) override;
	};
}