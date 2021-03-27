#pragma once

#include <string>

#include "jleGameEngine.h"

namespace jle
{
	class iEditorImGuiWindow
	{
	public:

		virtual ~iEditorImGuiWindow() {};

		virtual void Update(jleGameEngine& ge) = 0;
	};
}