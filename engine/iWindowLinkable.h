#pragma once

#include "iWindowInternalAPI.h"

class iWindowLinkable
{
public:
	virtual ~iWindowLinkable(){}
	virtual void LinkWindow(std::shared_ptr<iWindowInternalAPI> windowInternal) = 0;
};