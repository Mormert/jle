#pragma once

#include <memory>
#include <sstream>

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

#include "3rdparty/json.hpp"


class EditorJsonToImgui
{
public:
	void DrawAndGetInput()
	{
		for (int i = 0; i < mUsedTextFields; i++)
		{
			ImGui::InputTextMultiline(mTextfields[i][0], mTextfields[i][1], 1024);
		}
	}

	void JsonToImgui(nlohmann::json& j)
	{
		if (j.is_object())
		{
			auto obj = j.get<nlohmann::json::object_t>();
			mUsedTextFields = 0;
			for (auto& kvp : obj)
			{
				strcpy_s(mTextfields[mUsedTextFields][0], kvp.first.c_str());

				std::ostringstream oss;
				oss << kvp.second.dump(4);
				strcpy_s(mTextfields[mUsedTextFields][1], oss.str().c_str());
				mUsedTextFields++;
			}
		}
	}

	nlohmann::json ImGuiToJson()
	{
		nlohmann::json j;
		for (int i = 0; i < mUsedTextFields; i++)
		{
			j[mTextfields[i][0]] = nlohmann::json::parse(mTextfields[i][1]);
		}
		return j;
	}

private:
	char mTextfields[128][2][1024];
	int mUsedTextFields = 0;
};