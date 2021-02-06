#pragma once

#include "Texture.h"

enum class RenderingMethod { Dynamic, Static };

struct Quad
{
	float x, y;
	float depth;
};

struct TexturedQuad : public Quad
{
	jle::graphics::Texture& texture;

	// Coordinates on the texture
	int textureX, textureY;

	// Dimensions of quad on the texture
	unsigned int width, height;
};

struct ColoredQuad : public Quad
{
	unsigned char r, g, b, a;
};

class iQuadRendering
{
public:
	virtual ~iQuadRendering(){}

	virtual void SendTexturedQuad(TexturedQuad& texturedQuad, RenderingMethod renderingMethod) = 0;
	virtual void SendColoredQuad(ColoredQuad& coloredQuad, RenderingMethod renderingMethod) = 0;
};