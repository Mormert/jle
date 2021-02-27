#pragma once

#include "iWindowAPI.h"
#include "iWindowInitializer.h"
#include "iRenderingInternalAPI.h"

#include <utility>
#include <memory>

namespace jle
{
	class iWindowInternalAPI : public iWindowAPI
	{
	public:
		virtual ~iWindowInternalAPI() {};

		virtual void InitWindow(iWindowInitializer& windowInitializer, std::shared_ptr<iRenderingInternalAPI> internalRenderingAPI) = 0;

		virtual void UpdateWindow() = 0;

		virtual bool WindowShouldClose() = 0;

		virtual float GetTime() = 0;
		virtual bool GetKey(char key) = 0;
		virtual float GetScrollX() = 0;
		virtual float GetScrollY() = 0;
		virtual std::pair<int, int> GetCursor() = 0;

	};
}

