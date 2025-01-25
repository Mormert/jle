/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleResourceIndexer.h"
#include "jleImGuiArchive.h"
#include "modules/scripting/jleLuaEnvironment.h"

#include <jleVectorSet.h>
#include <glm/gtc/quaternion.hpp>

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, jleTextureRefOrRGBA &value)
{
    ImGui::PushID(elementCount++);

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

    ImGui::BeginGroupPanel(std::string{name + std::string{" (Texture or solid color)"}}.c_str());

    draw_ui(ar, name, value.textureRef());

    if (!value.textureRef()) {
        ImGui::ColorEdit4("Solid Color RGBA", &value.color()[0]);
    } else {
        ImGui::Image((void *)(intptr_t)value.textureRef()->id(),
                     ImVec2{100.f * globalImguiScale, 100.f * globalImguiScale});
        ImGui::SameLine();
        if (ImGui::Button("Remove RGBA Texture")) {
            value.textureRef() = jleResourceRef<jleTexture>();
        }
    }

    ImGui::EndGroupPanel();

    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, jleTextureRefOrRGB &value)
{
    ImGui::PushID(elementCount++);

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

    ImGui::BeginGroupPanel(std::string{name + std::string{" (Texture or solid color)"}}.c_str());

    draw_ui(ar, name, value.textureRef());

    if (!value.textureRef()) {
        ImGui::ColorEdit3("Solid Color RGB", &value.color()[0]);
    } else {
        ImGui::Image((void *)(intptr_t)value.textureRef()->id(),
                     ImVec2{100.f * globalImguiScale, 100.f * globalImguiScale});
        ImGui::SameLine();
        if (ImGui::Button("Remove RGB Texture")) {
            value.textureRef() = jleResourceRef<jleTexture>();
        }
    }

    ImGui::EndGroupPanel();

    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, jleTextureRefOrAlpha &value)
{
    ImGui::PushID(elementCount++);

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

    ImGui::BeginGroupPanel(std::string{name + std::string{" (Texture or constant value)"}}.c_str());

    draw_ui(ar, name, value.textureRef());

    if (!value.textureRef()) {
        ImGui::SliderFloat("Alpha", &value.alpha(), 0.f, 1.f);
    } else {
        ImGui::Image((void *)(intptr_t)value.textureRef()->id(),
                     ImVec2{100.f * globalImguiScale, 100.f * globalImguiScale});
        ImGui::SameLine();
        if (ImGui::Button("Remove Alpha Texture")) {
            value.textureRef() = jleResourceRef<jleTexture>();
        }
    }

    ImGui::EndGroupPanel();

    ImGui::PopID();
}

bool
jleImGuiArchive::draw_ui_reference(const char *name,
                                   std::string &value,
                                   std::vector<std::string> fileExtensions)
{
    ImGui::PushID(elementCount++);

    std::vector<char> charData(value.begin(), value.end());
    charData.resize(1000);

    bool isEditedAndDeactivated = false;

    if (ImGui::InputText(jleImGuiHelpers::LeftLabelImGui(name).c_str(), &charData[0], charData.size())) {
        value = std::string(charData.data());
    }

    std::vector<const jleVectorSet<jlePath> *> indexedFileSets;

    bool existsSomeFiles = false;
    for (auto &extension : fileExtensions) {
        auto e = editorCtx.resourceIndexer.getIndexedFilesPtr(extension.c_str());
        if (!e->empty()) {
            existsSomeFiles = true;
        }
        indexedFileSets.push_back(e);
    }

    static std::set<int> isOpenSet;

    bool isOpen = isOpenSet.find(elementCount) != isOpenSet.end();

    if (existsSomeFiles) {
        bool isFocused = ImGui::IsItemFocused();
        isOpen |= ImGui::IsItemActive();

        if (isOpen) {
            static std::string lastValue = "non-empty";
            static std::vector<std::string> lastFileExtensions;
            static std::vector<std::string> searchResults;
            if (lastValue != value || lastFileExtensions != fileExtensions) {
                lastValue = value;
                lastFileExtensions = fileExtensions;
                searchResults.clear();

                // Perform search
                for (auto &files : indexedFileSets) {
                    for (auto &file : *files) {
                        if (strstr(file.getVirtualPath().str().c_str(), value.c_str()) != NULL) {
                            searchResults.push_back(file.getVirtualPath().str());
                        }
                    }
                }
            }

            ImGui::SetNextWindowPos({ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y});
            ImGui::SetNextWindowSize({ImGui::GetItemRectSize().x, 0});
            if (ImGui::Begin(std::string{"popup_" + std::string{name}}.c_str(),
                             &isOpen,
                             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_Tooltip)) {
                ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
                isFocused |= ImGui::IsWindowFocused();
                int ctr = 0;
                for (auto &result : searchResults) {
                    if (ImGui::Selectable(result.c_str()) ||
                        (ImGui::IsItemFocused() && ImGui::IsKeyPressedMap(ImGuiKey_Enter))) {
                        value = result;
                        isEditedAndDeactivated = true;
                        isOpen = false;
                    }
                    if (ctr > 32) {
                        break;
                    }
                    ctr++;
                }
            }
            ImGui::End();
            isOpen &= isFocused;
        }
    } else {
        isOpen = false;
    }

    if (isOpen) {
        isOpenSet.insert(elementCount);
    } else {
        isOpenSet.erase(elementCount);
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        isEditedAndDeactivated = true;
    }

    ImGui::PopID();
    return isEditedAndDeactivated;
}

bool
jleImGuiArchive::draw_ui_lua_reference(const char *name, std::string &className)
{
    ImGui::PushID(elementCount++);

    if (!ctx.get<jleLuaEnvironment>()) {
        ImGui::Text("Serialization context doesn't have a Lua Environment reference!");
        ImGui::PopID();
        return false;
    }

    std::vector<char> charData(className.begin(), className.end());
    charData.resize(1000);

    bool isEditedAndDeactivated = false;

    if (ImGui::InputText(jleImGuiHelpers::LeftLabelImGui(name).c_str(), &charData[0], charData.size())) {
        className = std::string(charData.data());
    }

    const auto &loadedClasses = ctx.get<jleLuaEnvironment>()->loadedLuaClasses();

    static std::set<int> isOpenSet;

    bool isOpen = isOpenSet.find(elementCount) != isOpenSet.end();

    if (!loadedClasses.empty()) {
        bool isFocused = ImGui::IsItemFocused();
        isOpen |= ImGui::IsItemActive();

        if (isOpen) {
            static std::string lastValue = "non-empty";
            static std::vector<std::string> searchResults;
            if (lastValue != className) {
                lastValue = className;
                searchResults.clear();

                // Perform search
                for (auto &luaClassName : loadedClasses) {
                    if (strstr(luaClassName.first.c_str(), className.c_str()) != NULL) {
                        searchResults.push_back(luaClassName.first);
                    }
                }
            }

            ImGui::SetNextWindowPos({ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y});
            ImGui::SetNextWindowSize({ImGui::GetItemRectSize().x, 0});
            if (ImGui::Begin(std::string{"popup_" + std::string{name}}.c_str(),
                             &isOpen,
                             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_Tooltip)) {
                ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
                isFocused |= ImGui::IsWindowFocused();
                int ctr = 0;
                for (auto &result : searchResults) {
                    if (ImGui::Selectable(result.c_str()) ||
                        (ImGui::IsItemFocused() && ImGui::IsKeyPressedMap(ImGuiKey_Enter))) {
                        className = result;
                        isEditedAndDeactivated = true;
                        isOpen = false;
                    }
                    if (ctr > 32) {
                        break;
                    }
                    ctr++;
                }
            }
            ImGui::End();
            isOpen &= isFocused;
        }
    } else {
        isOpen = false;
    }

    if (isOpen) {
        isOpenSet.insert(elementCount);
    } else {
        isOpenSet.erase(elementCount);
    }

    if (ImGui::IsItemDeactivatedAfterEdit()) {
        isEditedAndDeactivated = true;
    }

    ImGui::PopID();
    return isEditedAndDeactivated;
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar,
                                       const char *name,
                                       jleLuaClassSerialization &value)
{
    ImGui::PushID(elementCount++);

    if (!ctx.get<jleLuaEnvironment>()) {
        ImGui::Text("Serialization context doesn't have a Lua Environment reference!");
        ImGui::PopID();
        return;
    }

    std::vector<char> charData(value.luaClassName.begin(), value.luaClassName.end());
    charData.resize(1000);

    const auto &className = value.luaClassName;
    const auto &loadedClasses = ctx.get<jleLuaEnvironment>()->loadedLuaClasses();

    bool validClassName = false;
    auto it = loadedClasses.find(className);

    if (it != loadedClasses.end()) {
        validClassName = true;
    }

    if (!validClassName) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.8f, 0.0f, 0.0f, 1.0f)); // Red background
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));    // Orange text
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.76f, 0.0f, 1.0f)); // Green text
    }

    bool isEditedAndDeactivated = draw_ui_lua_reference(name, value.luaClassName);
    if (isEditedAndDeactivated) {
        value.load_minimal(ar, value.luaClassName);
    }

    if (!validClassName) {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Class '%s' is not loaded in Lua environment!", className.c_str());
            ImGui::EndTooltip();
        }

        ImGui::PopStyleColor(2);
    } else {
        ImGui::PopStyleColor(1);
        ImGui::Text("Loaded from script: %s", it->second.getScriptPathWhereClassIsDefined().getVirtualPath().str().c_str());
        ImGui::Separator();
    }

    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, jlePath &value)
{
    ImGui::PushID(elementCount++);

    const jleVirtualPath virtualPath = value.getVirtualPath();

    std::string copy = virtualPath.str();
    draw_ui(ar, std::string{name + std::string{" (path)"}}.c_str(), copy);
    if (copy != virtualPath.str()) {
        value = jlePath{jleVirtualPath{(copy.c_str())}};
    }

    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, uint8_t &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui(name).c_str(), ImGuiDataType_U8, &value, 0.2f, nullptr, nullptr, "%u");
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, int8_t &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui(name).c_str(), ImGuiDataType_S8, &value, 0.2f, nullptr, nullptr, "%d");
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, uint16_t &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui(name).c_str(), ImGuiDataType_U16, &value, 0.2f, nullptr, nullptr, "%u");
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, int16_t &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui(name).c_str(), ImGuiDataType_S16, &value, 0.2f, nullptr, nullptr, "%d");
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, uint32_t &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui(name).c_str(), ImGuiDataType_U32, &value, 0.2f, nullptr, nullptr, "%u");
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, int32_t &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui(name).c_str(), ImGuiDataType_S32, &value, 0.2f, nullptr, nullptr, "%d");
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, uint64_t &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui(name).c_str(), ImGuiDataType_U64, &value, 0.2f, nullptr, nullptr, "%llu");
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, int64_t &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui(name).c_str(), ImGuiDataType_S64, &value, 0.2f, nullptr, nullptr, "%lld");
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, float &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui(name).c_str(), ImGuiDataType_Float, &value, 0.005f, nullptr, nullptr, "%f");
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, double &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragScalar(jleImGuiHelpers::LeftLabelImGui(name).c_str(), ImGuiDataType_Double, &value, 0.005f, nullptr, nullptr, "%f");
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, bool &value)
{
    ImGui::PushID(elementCount++);

    ImGui::Checkbox(jleImGuiHelpers::LeftLabelImGui(name, 0.7f).c_str(), &value);
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, std::string &value)
{
    ImGui::PushID(elementCount++);

    std::vector<char> charData(value.begin(), value.end());
    charData.resize(1000);

    if (ImGui::InputText(jleImGuiHelpers::LeftLabelImGui(name).c_str(), &charData[0], charData.size())) {
        value = std::string(charData.data());
    }
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, glm::vec2 &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragFloat2(jleImGuiHelpers::LeftLabelImGui(name).c_str(), &value[0]);
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, glm::vec3 &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragFloat3(jleImGuiHelpers::LeftLabelImGui(name).c_str(), &value[0]);
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, glm::vec4 &value)
{
    ImGui::PushID(elementCount++);

    ImGui::DragFloat4(jleImGuiHelpers::LeftLabelImGui(name).c_str(), &value[0]);
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, jleRGB &value)
{
    ImGui::PushID(elementCount++);
    ImGui::ColorEdit3(name, &value[0]);
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, jleRGBA &value)
{
    ImGui::PushID(elementCount++);
    ImGui::ColorEdit4(name, &value[0]);
    ImGui::PopID();
}

void
jleImGuiArchive::draw_ui(jleImGuiArchive &ar, const char *name, glm::quat &value)
{
    ImGui::PushID(elementCount++);

    glm::vec3 euler = glm::eulerAngles(value);

    if (ImGui::DragFloat3(jleImGuiHelpers::LeftLabelImGui(name).c_str(), &euler[0])) {
        value = glm::quat(euler);
    }
    ImGui::PopID();
}
