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
		float lastGameWindowWidth = 0.f, lastGameWindowHeight = 0.f;
		bool wasFocused = false;
	};
}