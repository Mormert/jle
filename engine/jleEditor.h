#pragma once

#include "jlePathDefines.h"

#include "jleGameEngine.h"
#include "jleEditorSettings.h"
#include "EditorBackgroundImage.h"
#include "iEditorImGuiWindow.h"

#include "Image.h"

#include <vector>
#include <memory>

namespace jle
{
	class jleEditor : public jleGameEngine
	{
	public:

		jleEditor(std::shared_ptr<jleGameSettings>, std::shared_ptr<jleEditorSettings>);

		void StartEditor();

		virtual void Start() override;
		virtual void Render() override;

	private:
		
		void InitImgui();
		void SetImguiTheme();

		void AddImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window);

        void MainEditorWindowResized(int w, int h);

		std::vector<std::shared_ptr<iEditorImGuiWindow>> ImGuiWindows;

		std::unique_ptr<Image> background_image;
		std::unique_ptr<EditorBackgroundImage> editor_background_image;

		std::shared_ptr<jleEditorSettings> editor_settings;
	};

}


