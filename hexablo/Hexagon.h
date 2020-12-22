#pragma once

#include "GameObject.h"
#include "Sprite.h"

namespace gameobjects
{
	class Hexagon : public GameObject
	{
		Hexagon(int worldX, int worldY);

	private:
		int worldX, worldY;
		Sprite sprite;
	};
}