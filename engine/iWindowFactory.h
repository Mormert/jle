#pragma once

#include "iWindowAPI.h"

#include "iWindowInitializer.h"

#include <memory>

class iWindowFactory
{
public:
	virtual ~iWindowFactory(){}

	virtual std::unique_ptr<iWindowAPI> CreateWindow() = 0;
	virtual std::unique_ptr<iWindowInitializer> CreateWindowInitializer() = 0;
};