#pragma once

#include "iEditorImGuiWindow.h"

#include "jleGameEngine.h"

namespace jle
{
	class GameEditorWindow : public iEditorImGuiWindow
	{
	public:

		virtual void Update(jleGameEngine& ge) override;
	private:
		unsigned int lastGameWindowWidth = 0, lastGameWindowHeight = 0;
	};

}