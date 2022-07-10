// Copyright (c) 2022. Johan Lind

#pragma once

#include <memory>
#include <sstream>
#include <utility>
#include <vector>
#include <climits>

#include "3rdparty/ImGui/imgui.h"
#include "3rdparty/ImGui/imgui_stdlib.h"
#include "3rdparty/ImGui/imgui_impl_glfw.h"
#include "3rdparty/ImGui/imgui_impl_opengl3.h"

#include "3rdparty/json.hpp"
#include <plog/Log.h>

class EditorJsonToImgui {
public:

    void DrawAndGetInput() {
        mRootNode.RecursiveDraw();
    }

    void JsonToImgui(nlohmann::json &j, const std::string &objectName) {
        PLOG_VERBOSE << j.dump(4);
        mRootNode.mNodes.clear();
        mRootNode.mName = objectName;
        mRootNode.RecursivelyConstructTree(j);
    }

    nlohmann::json ImGuiToJson() {
        nlohmann::json j;
        mRootNode.ConstructJson(j);
        PLOG_VERBOSE << j.dump(4);
        return j;
    }

private:

    struct _iNode {
        std::string mName = "root";
        std::vector<std::shared_ptr<_iNode>> mNodes;

        virtual ~_iNode() = default;

        void ConstructChildObject(const std::string &objName, const nlohmann::json &jsonObject) {
            if (jsonObject.is_object()) {
                auto oNode = std::make_shared<_iNode>();
                oNode->mName = objName;
                mNodes.push_back(oNode);

                // Use recursion if child is a json object
                oNode->RecursivelyConstructTree(jsonObject);
            } else if (jsonObject.is_array()) {
                auto arrayNode = std::make_shared<_NodeArray>();
                arrayNode->mName = objName;
                mNodes.push_back(arrayNode);

                int arrayIndex = 0;
                auto &&json_array = jsonObject.get<nlohmann::json::array_t>();

                if (!json_array.empty()) {
                    // store the value type as the first element
                    // it could however contain different types in the array!
                    arrayNode->value_type = json_array[0].type();
                }

                for (auto &&json_element_in_array: json_array) {
                    auto arrayChild = std::make_shared<_iNode>();
                    arrayChild->RecursivelyConstructTree(json_element_in_array);
                    arrayChild->mName = std::to_string(arrayIndex++);
                    arrayNode->mNodes.push_back(arrayChild);
                }
            } else if (jsonObject.is_number_float()) {
                auto jvalue = jsonObject.get<nlohmann::json::number_float_t>();
                auto node = std::make_shared<_NodeFloat>(static_cast<float_t>(jvalue));
                node->mName = objName;
                mNodes.push_back(node);
            } else if (jsonObject.is_number_integer()) {
                auto jvalue = jsonObject.get<nlohmann::json::number_integer_t>();
                auto node = std::make_shared<_NodeInt>(static_cast<int32_t>(jvalue));
                node->mName = objName;
                mNodes.push_back(node);
            } else if (jsonObject.is_string()) {
                auto jvalue = jsonObject.get<nlohmann::json::string_t>();
                auto node = std::make_shared<_NodeString>(jvalue);
                node->mName = objName;
                mNodes.push_back(node);
            } else if (jsonObject.is_boolean()) {
                auto jvalue = jsonObject.get<nlohmann::json::boolean_t>();
                auto node = std::make_shared<_NodeBool>(jvalue);
                node->mName = objName;
                mNodes.push_back(node);
            }
        }

        // Takes a json object, and depending on what the json object is (float, int, or another json object),
        // use recursion to construct nodes of the json object children, or
        void RecursivelyConstructTree(const nlohmann::json &j) {
            if (j.is_object()) // root needs to be json object
            {
                auto objHead = j.get<nlohmann::json::object_t>();
                for (auto &objChildren: objHead) {
                    ConstructChildObject(objChildren.first, objChildren.second);
                }
            }
        }

        // Uses recursion to draw the nodes with ImGui
        // It's a ImGui::TreeNode for json objects, and ImGui::DragInt for ints, etc.
        virtual void RecursiveDraw() {
            if (ImGui::TreeNode(mName.c_str())) {
                for (auto &nodeChildren: mNodes) {
                    nodeChildren->RecursiveDraw();
                }

                ImGui::TreePop();
            }
        }

        // Recursively goes through all children and constructs a json object
        virtual void ConstructJson(nlohmann::json &j_out) {
            for (auto &childNode: mNodes) {
                nlohmann::json j_inner;
                childNode->ConstructJson(j_inner);
                j_out[childNode->mName] = j_inner;
            }
        }
    };

    struct _NodeArray : _iNode {
        _NodeArray() = default;

        nlohmann::detail::value_t value_type;

        void RecursiveDraw() override {
            if (ImGui::TreeNode((mName + " [" + std::to_string(mNodes.size()) + "]").c_str())) {
                const auto endsWith = [](const std::string &mainStr, const std::string &toMatch) {
                    if (mainStr.size() >= toMatch.size() &&
                        mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0) {
                        return true;
                    } else {
                        return false;
                    }
                };

                if (endsWith(mName, "_STRVEC")) {
                    ImGui::SameLine();
                    if (ImGui::Button("+")) {
                        auto newNode = std::make_shared<_iNode>();
                        newNode->mName = std::to_string(mNodes.size());

                        auto stringNode = std::make_shared<_NodeString>("");
                        stringNode->mName = "mString";

                        newNode->mNodes.push_back(stringNode);
                        mNodes.push_back(newNode);
                    }
                } else if (endsWith(mName, "_INTVEC")) {
                    ImGui::SameLine();
                    if (ImGui::Button("+")) {
                        auto newNode = std::make_shared<_iNode>();
                        newNode->mName = std::to_string(mNodes.size());

                        auto intNode = std::make_shared<_NodeInt>(0);
                        intNode->mName = "mInt";

                        newNode->mNodes.push_back(intNode);
                        mNodes.push_back(newNode);
                    }
                } else if (endsWith(mName, "_FLOATVEC")) {
                    ImGui::SameLine();
                    if (ImGui::Button("+")) {
                        auto newNode = std::make_shared<_iNode>();
                        newNode->mName = std::to_string(mNodes.size());

                        auto floatNode = std::make_shared<_NodeFloat>(0.f);
                        floatNode->mName = "mFloat";

                        newNode->mNodes.push_back(floatNode);
                        mNodes.push_back(newNode);
                    }
                }

                std::vector<int> nodesToRemove;
                int index = 0;
                for (auto &&nodeChildren: mNodes) {

                    nodeChildren->RecursiveDraw();
                    ImGui::SameLine();
                    ImGui::PushID(index);
                    if (ImGui::Button("-")) {
                        nodesToRemove.push_back(index);
                    }
                    ImGui::PopID();
                    index++;
                }

                if (!nodesToRemove.empty()) {
                    for (auto &&node: nodesToRemove) {
                        mNodes.erase(mNodes.begin() + node);
                    }
                }


                ImGui::TreePop();
            }
        }

        void ConstructJson(nlohmann::json &j_out) override {
            j_out = nlohmann::json::array();
            for (auto &childNode: mNodes) {
                nlohmann::json j_inner;
                childNode->ConstructJson(j_inner);
                j_out.push_back(j_inner);
            }
        }
    };

    struct _NodeFloat : _iNode {
        _NodeFloat(float f) : value{f} {}

        float value;

        void RecursiveDraw() override {
            ImGui::PushItemWidth(100);
            ImGui::DragFloat(mName.c_str(), &value, 0.02f, -FLT_MAX, FLT_MAX, "%.2f");
        }

        void ConstructJson(nlohmann::json &j_out) override {
            j_out = value;
        }
    };

    struct _NodeInt : _iNode {
        _NodeInt(int i) : value{i} {}

        int value;

        void RecursiveDraw() override {
            ImGui::PushItemWidth(100);
            ImGui::DragInt(mName.c_str(), &value, 0.02f, INT_MIN, INT_MAX, "%.2f");
        }

        void ConstructJson(nlohmann::json &j_out) override {
            j_out = value;
        }
    };

    struct _NodeBool : _iNode {
        _NodeBool(bool b) : value{b} {}

        bool value;

        void RecursiveDraw() override {
            ImGui::PushItemWidth(100);
            ImGui::Checkbox(mName.c_str(), &value);
        }

        void ConstructJson(nlohmann::json &j_out) override {
            j_out = value;
        }
    };

    struct _NodeString : _iNode {
        _NodeString(std::string s) : str{std::move(s)} {}

        std::string str;

        void RecursiveDraw() override {
            ImGui::PushItemWidth(250);
            ImGui::InputText(mName.c_str(), &str);
        }

        void ConstructJson(nlohmann::json &j_out) override {
            // TODO: Watch out for buffer overflow here
            j_out = str.data();
        }
    };

    _iNode mRootNode;

};
