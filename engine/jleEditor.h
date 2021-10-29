#pragma once

#include "JLE_ENGINE_CONFIG.h"

#include "jleGameEngine.h"
#include "jleEditorSettings.h"
#include "EditorBackgroundImage.h"
#include "iEditorImGuiWindow.h"

#include "Image.h"

#include <vector>
#include <memory>

namespace jle
{

#pragma region jleFramebufferSettings to/from json 
	using json = nlohmann::json;
	inline void to_json(nlohmann::json& j, const jleEditorSettings& es)
	{
		j = (jleGameSettings&)es;
		j["editor_background_image"] = es.editorBackgroundImage;
	}

	inline void from_json(const nlohmann::json& j, jleEditorSettings& es)
	{
		from_json(j, (jleGameSettings&)es);
		j.at("editor_background_image").get_to(es.editorBackgroundImage);
	}
#pragma endregion

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

		std::vector<std::shared_ptr<iEditorImGuiWindow>> ImGuiWindows;

		std::unique_ptr<Image> background_image;
		std::unique_ptr<EditorBackgroundImage> editor_background_image;

		std::shared_ptr<jleEditorSettings> editor_settings;
	};

}


