#include "Viewport.h"

#include <iostream>

namespace jle
{
	Viewport::Viewport(unsigned int viewportWidth, unsigned int viewportHeight,
		unsigned int windowWidth, unsigned int windowHeight)
		:	viewportWidth{viewportWidth}, viewportHeight{viewportHeight},
			windowWidth{ windowWidth }, windowHeight{ windowHeight },
			xPos{ 0 }, yPos{ 0 } {}

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

	void Viewport::SetViewportHeightKeepAspect(unsigned int height)
	{
		float ratio = float(windowWidth) / float(windowHeight);

		viewportWidth = static_cast<int>(height * ratio);
		viewportHeight = height;
	}

	void Viewport::SetViewportWidthKeepAspect(unsigned int width)
	{
		float ratio = float(windowHeight) / float(windowWidth);

		viewportHeight = static_cast<int>(width * ratio);
		viewportWidth = width;
	}

	int Viewport::GetWorldPositionX() const noexcept
	{
		return xPos;
	}
	int Viewport::GetWorldPositionY() const noexcept
	{
		return yPos;
	}

	unsigned int Viewport::GetViewportWidth() const noexcept
	{
		return viewportWidth;
	}
	unsigned int Viewport::GetViewportHeight() const noexcept
	{
		return viewportHeight;
	}

	void Viewport::SetWindowDimensions(unsigned int width, unsigned int height)
	{
		this->windowWidth = width;
		this->windowHeight = height;
	}
}