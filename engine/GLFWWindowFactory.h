#pragma once

#include "iWindowFactory.h"

#include <memory>

class GLFWWindowFactory : public iWindowFactory
{
public:
	std::unique_ptr<iWindowAPI> CreateWindow() override;

	virtual std::unique_ptr<iWindowInitializer> CreateWindowInitializer() override;
};

