#include "HexMap.h"


#include <iostream>

Hexagon::Hexagon(int worldX, int worldY, const jle::graphics::Sprite& sprite)
	: sprite{ sprite }, worldX{worldX}, worldY{worldY} { }


void Hexagon::Render() const
{
	sprite.DrawSprite(worldX, worldY, 0.0f);
}

void HexMap::RenderHexagons()
{
	for (const auto& h : hexagons)
	{
		h.Render();
	}
}

jle::graphics::Texture myTexture8{ "GameAssets/HexagonDiabloConcept.png" };
jle::graphics::Sprite mySprite8(myTexture8, 8, 88, 15, 13);
jle::graphics::Sprite mySprite9(myTexture8, 24, 88, 15, 13);

void HexMap::GenerateHexagons()
{

	constexpr int sizeX = 14;
	constexpr int sizeY = 9;
	constexpr int size = 8;

	bool a = true;

	for (int r = -15; r < 15; r++)
	{
		for (int q = -15; q < 15; q++)
		{
			
			int x = q * sizeX;

			if (a)
			{
				x = q * sizeX + 7;
			}
			

			
			int y = r * sizeY;


			if (r * q % 3 == 0)
			{
				hexagons.push_back(Hexagon{ x, y, mySprite8 });
			}
			else
			{
				hexagons.push_back(Hexagon{ x, y, mySprite9 });
			}
			
			
		}
		a = !a;
	}

}