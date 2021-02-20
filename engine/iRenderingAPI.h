#pragma once

#include "iQuadRendering.h"

#include <memory>

struct RenderingSettings
{
	bool renderFullscreen = true;
};

class iRenderingAPI
{
public:
	virtual ~iRenderingAPI() {};

	std::unique_ptr<iQuadRendering> quads;
};