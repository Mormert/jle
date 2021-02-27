#pragma once

#include "iWindowInternalAPI.h"

namespace jle
{
	class iWindowLinkable
	{
	public:
		virtual ~iWindowLinkable() {}
		virtual void LinkWindow(std::shared_ptr<iWindowInternalAPI> windowInternal) = 0;
	};
}