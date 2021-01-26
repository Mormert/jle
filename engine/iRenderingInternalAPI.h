#pragma once

#include "iRenderingAPI.h"

class iRenderingInternalAPI : public iRenderingAPI
{
public:
	virtual ~iRenderingInternalAPI() {};
	virtual void SetViewportDimensions(int x, int y, unsigned int width, unsigned int height) = 0; // glviewport...

	virtual void Render() = 0;
};