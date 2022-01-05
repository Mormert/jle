#pragma once

#include "iEditorImGuiWindow.h"
#include "EditorGameControllerWidget.h"

namespace jle
{
	class EditorWindowsPanel : public iEditorImGuiWindow
	{
	public:
		EditorWindowsPanel(const std::string& window_name);
		virtual void Update(jleGameEngine& ge) override;

		void AddWindow(std::shared_ptr<iEditorImGuiWindow> window);

	private:
		std::vector<std::shared_ptr<iEditorImGuiWindow>> windows;

        EditorGameControllerWidget mGameController;
	};
}