#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include "3rdparty/ImGui/imgui.h"
#include "3rdparty/ImGui/imgui_impl_glfw.h"
#include "3rdparty/ImGui/imgui_impl_opengl3.h"

#include "3rdparty/json.hpp"
#include <plog/Log.h>

class EditorJsonToImgui
{
public:

	void DrawAndGetInput()
	{
		mRootNode.RecursiveDraw();
	}

	void JsonToImgui(nlohmann::json& j, const std::string& objectName)
	{
		PLOG_VERBOSE << j.dump(4);
		mRootNode.mNodes.clear();
		mRootNode.mName = objectName;
		mRootNode.RecursivelyConstructTree(j);
	}

	nlohmann::json ImGuiToJson()
	{
		nlohmann::json j;
		mRootNode.ConstructJson(j);
		PLOG_VERBOSE << j.dump(4);
		return j;
	}

private:

	struct _iNode
	{
		std::string mName = "root";
		std::vector<std::shared_ptr<_iNode>> mNodes;

		virtual ~_iNode(){}

		// Takes a json object, and depending on what the json object is (float, int, or another json object),
		// use recursion to construct nodes of the json object children, or 
		void RecursivelyConstructTree(nlohmann::json& j) {
			if (j.is_object()) // root needs to be json object
			{
				auto objHead = j.get<nlohmann::json::object_t>();
				for (auto& objChildren : objHead)
				{
					if (objChildren.second.is_object())
					{
						auto oNode = std::make_shared<_iNode>();
						oNode->mName = objChildren.first.c_str();
						mNodes.push_back(oNode);

						// Use recursion if child is a json object
						oNode->RecursivelyConstructTree(objChildren.second);
					}
					else if(objChildren.second.is_array())
					{
						auto arrayNode = std::make_shared<_NodeArray>();
						arrayNode->mName = objChildren.first.c_str();
						mNodes.push_back(arrayNode);
						
						int arrayIndex = 0;
						auto&& json_array = objChildren.second.get<nlohmann::json::array_t>();
						for(auto&& json_element_in_array : json_array)
						{
							auto arrayChild = std::make_shared<_iNode>();
							arrayChild->RecursivelyConstructTree(json_element_in_array);
							arrayChild->mName = std::to_string(arrayIndex++);
							arrayNode->mNodes.push_back(arrayChild);
						}
					}
					else if (objChildren.second.is_number_float())
					{
						auto jvalue = objChildren.second.get<nlohmann::json::number_float_t>();
						auto node = std::make_shared<_NodeFloat>(static_cast<float_t>(jvalue));
						node->mName = objChildren.first.c_str();
						mNodes.push_back(node);
					}
					else if (objChildren.second.is_number_integer())
					{
						auto jvalue = objChildren.second.get<nlohmann::json::number_integer_t>();
						auto node = std::make_shared<_NodeInt>(static_cast<int32_t>(jvalue));
						node->mName = objChildren.first.c_str();
						mNodes.push_back(node);
					}
					else if (objChildren.second.is_string())
					{
						auto jvalue = objChildren.second.get<nlohmann::json::string_t>();
						auto node = std::make_shared<_NodeString>(jvalue);
						node->mName = objChildren.first.c_str();
						mNodes.push_back(node);
					}
					else if (objChildren.second.is_boolean())
					{
						auto jvalue = objChildren.second.get<nlohmann::json::boolean_t>();
						auto node = std::make_shared<_NodeBool>(jvalue);
						node->mName = objChildren.first.c_str();
						mNodes.push_back(node);
					}
				}
			}
		}

		// Uses recursion to draw the nodes with ImGui
		// It's a ImGui::TreeNode for json objects, and ImGui::DragInt for ints, etc.
		virtual void RecursiveDraw()
		{
			if (ImGui::TreeNode(mName.c_str()))
			{
				for (auto& nodeChildren : mNodes)
				{
					nodeChildren->RecursiveDraw();
				}

				ImGui::TreePop();
			}
		}

		// Recursively goes through all children and constructs a json object
		virtual void ConstructJson(nlohmann::json& j_out)
		{
			for (auto& childNode : mNodes)
			{
				nlohmann::json j_inner;
				childNode->ConstructJson(j_inner);
				j_out[childNode->mName] = j_inner;
			}
		}
	};
	
	struct _NodeArray : _iNode
    {
    	_NodeArray() = default;
		
    	virtual void RecursiveDraw() override
    	{
    		if (ImGui::TreeNode((mName + " [" + std::to_string(mNodes.size()) + "]").c_str()))
    		{
    			for (auto&& nodeChildren : mNodes)
    			{
    				nodeChildren->RecursiveDraw();
    			}
    			ImGui::TreePop();
    		}
    	}

    	virtual void ConstructJson(nlohmann::json& j_out) override
    	{
    		j_out = nlohmann::json::array();
    		for (auto& childNode : mNodes)
    		{
    			nlohmann::json j_inner;
    			childNode->ConstructJson(j_inner);
    			j_out.push_back(j_inner);
    		}
    	}
    };
	
	struct _NodeFloat : _iNode
	{
		_NodeFloat(float f) : value{ f } {}
		float value;

		virtual void RecursiveDraw() override
		{
			ImGui::PushItemWidth(100);
			ImGui::DragFloat(mName.c_str(), &value, 0.02f, -FLT_MAX, FLT_MAX, "%.2f");
		}

		virtual void ConstructJson(nlohmann::json& j_out)
		{
			j_out = value;
		}
	};

	struct _NodeInt : _iNode
	{
		_NodeInt(int i) : value{ i } {}
		int value;

		virtual void RecursiveDraw() override
		{
			ImGui::PushItemWidth(100);
			ImGui::DragInt(mName.c_str(), &value, 0.02f, INT_MIN, INT_MAX, "%.2f");
		}

		virtual void ConstructJson(nlohmann::json& j_out)
		{
			j_out = value;
		}
	};

	struct _NodeBool : _iNode
	{
		_NodeBool(bool b) : value{ b } {}
		bool value;

		virtual void RecursiveDraw() override
		{
			ImGui::PushItemWidth(100);
			ImGui::Checkbox(mName.c_str(), &value);
		}

		virtual void ConstructJson(nlohmann::json& j_out)
		{
			j_out = value;
		}
	};

	struct _NodeString : _iNode
	{
		// TODO: Use ImGui callback on buffer resize for the string instead of a hard-coded cap with reserve().
		_NodeString(const std::string& s) : str{ s } { str.reserve(1024); }
		std::string str;

		virtual void RecursiveDraw() override
		{
			ImGui::PushItemWidth(250);
			ImGui::InputText(mName.c_str(), (char*)str.c_str(), str.capacity() + 1);
		}

		virtual void ConstructJson(nlohmann::json& j_out)
		{
			// TODO: Watch out for buffer overflow here
			j_out = str.data();
		}
	};

	_iNode mRootNode;

};