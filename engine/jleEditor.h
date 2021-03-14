#pragma once

#include "jleGameEngine.h"

namespace jle
{
	class jleEditor : public jleGameEngine
	{
		using jleGameEngine::jleGameEngine;

	public:
		void StartEditor();

		virtual void Start() override;
		virtual void Render() override;
	};

}


