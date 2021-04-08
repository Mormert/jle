#pragma once

#include "iEditorImGuiWindow.h"

namespace jle
{
	class EditorGameControllerWindow : public iEditorImGuiWindow
	{
	public:
		EditorGameControllerWindow(const std::string& window_name);
		virtual void Update(jleGameEngine& ge) override;

		void StartGame();
		void StopGame();
		void PauseGame();
	};
}


