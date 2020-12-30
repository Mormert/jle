#pragma once

namespace jle
{
	class Viewport
	{
	public:
		Viewport(unsigned int width, unsigned int height,
			unsigned int windowWidth, unsigned int windowHeight, bool setToMain = true);

		void SetWorldPosition(int worldX, int worldY);
		void SetViewportDimensions(unsigned int width, unsigned int height);

		int GetWorldPositionX();
		int GetWorldPositionY();

		unsigned int GetViewportWidth();
		unsigned int GetViewportHeight();

		static Viewport &GetMainViewport();

	private:
		int xPos, yPos;
		unsigned int viewportWidth, viewportHeight, windowWidth, windowHeight;

		static Viewport* mainViewport;

		friend class Engine;
		void SetWindowDimensions(unsigned int width, unsigned int height);

	};
}