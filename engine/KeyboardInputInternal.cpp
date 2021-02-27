#include "KeyboardInputInternal.h"

namespace jle
{
	KeyboardInputInternal::KeyboardInputInternal(std::shared_ptr<iWindowInternalAPI> windowInternal)
	{
		this->windowInternal = windowInternal;
	}

	void KeyboardInputInternal::LinkWindow(std::shared_ptr<iWindowInternalAPI> windowInternal)
	{
		this->windowInternal = windowInternal;
	}


	bool KeyboardInputInternal::GetKeyPressed(char key)
	{
		return false;
	}

	bool KeyboardInputInternal::GetKeyReleased(char key)
	{
		return false;
	}

	bool KeyboardInputInternal::GetKeyDown(char key)
	{
		return windowInternal->GetKey(key);
	}
}