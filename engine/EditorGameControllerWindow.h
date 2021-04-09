#pragma once

#include "iEditorImGuiWindow.h"

namespace jle
{
	class EditorGameControllerWindow : public iEditorImGuiWindow
	{
	public:
		EditorGameControllerWindow(const std::string& window_name);
		virtual void Update(jleGameEngine& ge) override;

		inline void StartGame();
		inline void StopGame();
		inline void PauseGame();
	};
}


