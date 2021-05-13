#pragma once

#include "iRenderingAPI.h"
#include "iTexture.h"

class Racket
{
public:
	Racket(int x, int y, std::shared_ptr<jle::iTexture> texture);
	void SetY(int y);
	int GetY(){ return y; }
	void Update();

private:
	TexturedQuad racketTexturedQuad;
	const int x;
	int y;
};