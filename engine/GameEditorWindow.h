#pragma once

#include "iEditorImGuiWindow.h"

#include "jleGameEngine.h"

namespace jle
{
	class GameEditorWindow : public iEditorImGuiWindow
	{
	public:
		GameEditorWindow(const std::string& window_name);
		virtual void Update(jleGameEngine& ge) override;

	private:
		unsigned int lastGameWindowWidth = 0, lastGameWindowHeight = 0;
		bool wasFocused = false;

    public:
        static inline bool IsFullscreen = false;
	};

}