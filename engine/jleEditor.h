#pragma once

#include "jleGameEngine.h"

#include "EditorBackgroundImage.h"
#include "iEditorImGuiWindow.h"

#include <vector>
#include <memory>

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

		void AddImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window);

		std::vector<std::shared_ptr<iEditorImGuiWindow>> ImGuiWindows;

		std::unique_ptr<EditorBackgroundImage> background_image;
	};

}


