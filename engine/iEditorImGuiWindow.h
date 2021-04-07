#pragma once

#include <string>

#include "jleGameEngine.h"

namespace jle
{
	class iEditorImGuiWindow
	{
	public:

		iEditorImGuiWindow(const std::string& window_name) : window_name{window_name} {}
		virtual ~iEditorImGuiWindow() {};

		virtual void Update(jleGameEngine& ge) = 0;

	protected:
		std::string window_name;
	};
}