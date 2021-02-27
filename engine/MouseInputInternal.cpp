#include "MouseInputInternal.h"

namespace jle
{
	MouseInputInternal::MouseInputInternal(std::shared_ptr<iWindowInternalAPI> windowInternal)
	{
		this->windowInternal = windowInternal;
	}

	void MouseInputInternal::LinkWindow(std::shared_ptr<iWindowInternalAPI> windowInternal)
	{
		this->windowInternal = windowInternal;
	}

	int MouseInputInternal::GetMouseX()
	{
		return windowInternal->GetCursor().first;
	}

	int MouseInputInternal::GetMouseY()
	{
		return windowInternal->GetCursor().second;
	}

	float MouseInputInternal::GetMouseXDelta()
	{
		return 0;
	}

	float MouseInputInternal::GetMouseYDelta()
	{
		return 0;
	}

	float MouseInputInternal::GetScrollX()
	{
		return 0;
	}

	float MouseInputInternal::GetScrollY()
	{
		return 0;
	}
}