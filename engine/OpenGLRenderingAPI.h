#pragma once

#include "iRenderingInternalAPI.h"
#include "iWindowInitializer.h"

class OpenGLRenderingAPI : public iRenderingInternalAPI
{
public:
	~OpenGLRenderingAPI(){}

	void SetViewportDimensions(int x, int y, unsigned int width, unsigned int height) override;
	void Render() override;
};

