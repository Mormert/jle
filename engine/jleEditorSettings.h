#pragma once

#include "JLE_ENGINE_CONFIG.h"
#include <string>

namespace jle
{
	struct jleEditorSettings
	{
		std::string editorBackgroundImage{ JLE_ENGINE_PATH + "/EditorResources/jle_default_bg.jpg" };
	};
}