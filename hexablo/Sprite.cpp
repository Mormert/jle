#include "Sprite.h"

#include "Renderer2D.h"

Sprite::Sprite(Texture &texture, int x, int y, int width, int height)
	: x{x}, y{y}, width{width}, height{height}, texture{ texture }
{
}

void Sprite::DrawSprite(int worldX, int worldY, float depth)
{
	Renderer2D::GetMainRenderer().RenderQuadTexture(worldX, worldY, depth, texture, x, y, width, height);
}