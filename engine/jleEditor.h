#pragma once

#include "jleGameEngine.h"

namespace jle
{
	class jleEditor : public jleGameEngine
	{
	public:

		jleEditor(std::shared_ptr<GameSettings>);

		void StartEditor();

		virtual void Start() override;
		virtual void Render() override;

	private:

		
		void InitImgui();
		void SetImguiTheme();

		unsigned int lastGameWindowWidth = 0, lastGameWindowHeight = 0;
	};

}


