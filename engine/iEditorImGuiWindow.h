#pragma once

#include <string>

#include "jleGameEngine.h"

#include <vector>
#include <memory>

namespace jle
{
	class iEditorImGuiWindow
	{
	public:

		iEditorImGuiWindow(const std::string& window_name) : window_name{window_name} {}
		virtual ~iEditorImGuiWindow() {};

		virtual void Update(jleGameEngine& ge) = 0;

		virtual void OpenWindow()  { isOpened = true; }
		virtual void CloseWindow() { isOpened = false; }
		bool GetOpened() { return isOpened; }

		const std::string& GetWindowName() { return window_name; }

	protected:
		std::string window_name;
		bool isOpened = true;
	};
}