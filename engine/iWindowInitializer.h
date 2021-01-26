#pragma once

using InterfaceWindowPtr = void*;

class iWindowInitializer
{
public:
	virtual ~iWindowInitializer() {};

	virtual InterfaceWindowPtr InitWindow(int width, int height, const char* title) = 0;
};