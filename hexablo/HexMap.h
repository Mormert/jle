#pragma once

#include <vector>

#include "Sprite.h"




class Hexagon
{
public:
	Hexagon(int worldX, int worldY, const jle::graphics::Sprite& sprite);

	void Render() const;

private:
	int worldX, worldY;
	jle::graphics::Sprite sprite;
};


class HexMap
{

public:
	void RenderHexagons();
	void GenerateHexagons();

private:
	std::vector<Hexagon> hexagons;
};

