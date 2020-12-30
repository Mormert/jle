#include "Viewport.h"

namespace jle
{

	Viewport* Viewport::mainViewport{ nullptr };

	Viewport::Viewport(unsigned int viewportWidth, unsigned int viewportHeight,
		unsigned int windowWidth, unsigned int windowHeight, bool setToMain)
		: viewportWidth{viewportWidth}, viewportHeight{viewportHeight}, xPos{0}, yPos{0}, windowWidth{ windowWidth }, windowHeight{ windowHeight }
		{ if (setToMain) { mainViewport = this; } }

	void Viewport::SetWorldPosition(int worldX, int worldY)
	{
		this->xPos = worldX;
		this->yPos = worldY;
	}

	void Viewport::SetViewportDimensions(unsigned int width, unsigned int height)
	{
		this->viewportWidth = width;
		this->viewportHeight = height;
	}

	int Viewport::GetWorldPositionX()
	{
		return xPos;
	}
	int Viewport::GetWorldPositionY()
	{
		return yPos;
	}

	unsigned int Viewport::GetViewportWidth()
	{
		return viewportWidth;
	}
	unsigned int Viewport::GetViewportHeight()
	{
		return viewportHeight;
	}

	Viewport& Viewport::GetMainViewport()
	{
		return *mainViewport;
	}

	void Viewport::SetWindowDimensions(unsigned int width, unsigned int height)
	{
		this->windowWidth = width;
		this->windowHeight = height;
	}
}