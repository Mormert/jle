#pragma once

#include "iInput.h"

class iMouseInput : public iInput
{
public:
	virtual ~iMouseInput() {}

	// Gets the X mouse position on the Window
	virtual int GetMouseX() = 0;
	// Gets the Y mouse position on the Window
	virtual int GetMouseY() = 0;

	// Gets the X mouse delta for 1 frame
	virtual float GetMouseXDelta() = 0;
	// Gets the Y mouse delta for 1 frame
	virtual float GetMouseYDelta() = 0;

	// Returns a float != 0 if scroll detected, value depend on scroll
	virtual float GetScrollX() = 0;
	// Returns a float != 0 if scroll detected, value depend on scroll
	virtual float GetScrollY() = 0;
};