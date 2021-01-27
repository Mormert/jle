#pragma once

#include <memory>

class iRenderingAPI
{
public:
	virtual ~iRenderingAPI() {};

	std::unique_ptr<iRendering2DAPI> rendering2DAPI;
};