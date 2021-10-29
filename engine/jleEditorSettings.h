#pragma once

#include <string>

namespace jle
{
	struct jleEditorSettings : jleGameSettings
	{
		std::string editorBackgroundImage{ JLE_ENGINE_PATH + "/EditorResources/jle_default_bg.jpg" };
	};
}