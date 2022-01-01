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

        std::shared_ptr<iTexture> mPlayGameIcon;
        std::shared_ptr<iTexture> mRestartGameIcon;
        std::shared_ptr<iTexture> mPauseGameIcon;
        std::shared_ptr<iTexture> mNextFrameIcon;
	};
}


