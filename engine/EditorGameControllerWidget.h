#pragma once

#include "iEditorImGuiWindow.h"

namespace jle
{
	class EditorGameControllerWidget : public iEditorImGuiWindow
	{
	public:
		EditorGameControllerWidget(const std::string& window_name);
		virtual void Update(jleGameEngine& ge) override;

        std::shared_ptr<iTexture> mPlayGameIcon;
        std::shared_ptr<iTexture> mRestartGameIcon;
        std::shared_ptr<iTexture> mPauseGameIcon;
        std::shared_ptr<iTexture> mNextFrameIcon;
	};
}


