#pragma once

#include "iQuadRendering.h"

#include <memory>

class iRenderingAPI
{
public:
	virtual ~iRenderingAPI() {};

	std::unique_ptr<iQuadRendering> quads;
};