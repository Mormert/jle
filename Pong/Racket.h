#pragma once

#include "jleRenderingAPIInterface.h"
#include "jleTextureInterface.h"

class Racket
{
public:
	Racket(int x, int y, std::shared_ptr<iTexture> texture);
	void SetY(int y);
	int GetY(){ return y; }
	void Update();

private:
	TexturedQuad racketTexturedQuad;
	const int x;
	int y;
};