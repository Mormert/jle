#pragma once

#include "JLE_ENGINE_CONFIG.h"

#include "jleGameEngine.h"

#include "EditorBackgroundImage.h"
#include "iEditorImGuiWindow.h"

#include "Image.h"

#include <vector>
#include <memory>

namespace jle
{
	struct EditorSettings : GameSettings
	{
		std::string editorBackgroundImage{ JLE_ENGINE_PATH + "/EditorResources/jle_default_bg.jpg" };
	};

	class jleEditor : public jleGameEngine
	{
	public:

		jleEditor(std::shared_ptr<EditorSettings>);

		void StartEditor();

		virtual void Start() override;
		virtual void Render() override;

	private:
		
		void InitImgui();
		void SetImguiTheme();

		void AddImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window);

		std::vector<std::shared_ptr<iEditorImGuiWindow>> ImGuiWindows;

		std::unique_ptr<Image> background_image;
		std::unique_ptr<EditorBackgroundImage> editor_background_image;
	};

}


