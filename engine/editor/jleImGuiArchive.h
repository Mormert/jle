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

#ifndef JLEIMGUICEREALARCHIVE_H
#define JLEIMGUICEREALARCHIVE_H

#include "jleBuildConfig.h"

#if JLE_BUILD_EDITOR
// The following macro only works on arithmetic types: float, int, double, etc, and std::string
#define JLE_TOOLTIP_ARITHMETIC(TYPE, TIP, NAME) jleToolTip<TYPE>(NAME){TIP};
#else
#define JLE_TOOLTIP_ARITHMETIC(TYPE, TIP, NAME) TYPE NAME;
#endif

#if JLE_BUILD_EDITOR

#include "serialization/jleSerialization.h"

#include "core/jleFileWatcher.h"
#include "jleComponent.h"
#include "jleEditor.h"
#include "jleExternalSerialization.h"
#include "jleLuaClassSerialization.h"
#include "jleRGB.h"
#include "jleResourceRef.h"
#include "jleTextureRefOrRGBA.h"
#include "jleTransform.h"

#include "jleImGuiExtensions.h"
#include <ImGui/ImGuizmo.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <cereal/cereal.hpp>
#include <cereal/details/helpers.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>

#include <magic_enum/magic_enum.hpp>

#include <Tracy.hpp>

#include <filesystem>
#include <iostream>
#include <jleSTL/jleFixedVector.h>
#include <vector>

// A tooltip utility for showing tooltips in the editor
// Works on arithmetic types: float, int, double, etc, and std::string
template <typename T>
struct jleToolTip {
    explicit jleToolTip<T>(const std::string_view tip) : tip_view{tip} {}

    jleToolTip<T>(T i) { item = i; }
    operator T() { return item; }

    jleToolTip<T>(const jleToolTip<T> &other) : item{other.item}, tip_view{other.tip_view} {}

    jleToolTip<T> &
    operator=(const jleToolTip<T> &other)
    {
        if (this != &other) {
            item = other.item;
            tip_view = other.tip_view;
        }
        return *this;
    }

    T item{};

    template <class Archive>
    T
    save_minimal(Archive const &) const
    {
        return item;
    }

    template <class Archive>
    void
    load_minimal(Archive const &, T const &value)
    {
        item = value;
    }

    std::string_view tip_view;
};

class jleImGuiArchiveInternal : public jleSerializationArchive_EditorOnly,
                                public cereal::OutputArchive<jleImGuiArchiveInternal>
{
public:
    explicit jleImGuiArchiveInternal(jleEditorModulesContext &context)
        : jleSerializationArchive_EditorOnly(jleSerializationContext{&context.engineModulesContext.resourcesModule,
                                                                     &context.engineModulesContext.luaEnvironment,
                                                                     &context.engineModulesContext.renderThread},
                                             context),
          OutputArchive<jleImGuiArchiveInternal>(this)
    {
    }

    ~jleImGuiArchiveInternal() override = default;

    std::string nextPolymorhphicTypeName{};
};

class jleImGuiArchive : public jleSerializationArchive_EditorOnly, public cereal::InputArchive<jleImGuiArchive>
{
public:
    explicit jleImGuiArchive(jleEditorModulesContext &context)
        : jleSerializationArchive_EditorOnly(jleSerializationContext{&context.engineModulesContext.resourcesModule,
                                                                     &context.engineModulesContext.luaEnvironment,
                                                                     &context.engineModulesContext.renderThread},
                                             context),
          InputArchive<jleImGuiArchive>(this)
    {
    }

    ~jleImGuiArchive() override = default;

    template <class T>
    void
    draw(jleImGuiArchive &ar, std::string name, T &value)
    {
        elementCount += 1;

        if (name == "") {
            name = std::to_string(elementCount);
        }

        draw_ui(ar, name.c_str(), value);
    }

private:
    void draw_ui(jleImGuiArchive &ar, const char *name, jleTextureRefOrRGBA &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, jleTextureRefOrRGB &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, jleTextureRefOrAlpha &value);

    bool draw_ui_reference(const char *name, std::string &value, std::vector<std::string> fileExtensions);
    bool draw_ui_lua_reference(const char *name, std::string &value);

    // clang-format off
    void draw_ui(jleImGuiArchive &ar, const char *name, jleLuaClassSerialization &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, jlePath &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, uint32_t &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, int32_t &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, uint64_t &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, int64_t &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, float &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, double &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, bool &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, std::string &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, glm::vec2 &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, glm::vec3 &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, glm::vec4 &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, jleRGB &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, jleRGBA &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, glm::quat &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, jleTransform &value);
    // clang-format on

    template <class T>
    void
    draw_ui(jleImGuiArchive &ar, std::string name, std::vector<T> &vector)
    {
        int removeIndex = -1;

        if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            int i = 0;
            for (auto &&value : vector) {
                name = std::to_string(i);

                draw_ui(ar, name.c_str(), value);

                ImGui::PushID(elementCount++);
                ImGui::SameLine();
                if (ImGui::Button("-")) {
                    removeIndex = i;
                }
                ImGui::PopID();

                i++;
            }

            ImGui::TreePop();
        }

        if (removeIndex != -1) {
            vector.erase(vector.begin() + removeIndex);
        }

        ImGui::PushID(elementCount++);
        if (ImGui::Button("+")) {
            vector.push_back({});
        }
        ImGui::PopID();
    }

#define STD_MAPS_IMPL                                                                                                  \
    elementCount += 1;                                                                                                 \
                                                                                                                       \
    ImGui::PushID(elementCount);                                                                                       \
                                                                                                                       \
    if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {                                             \
        int e = 0;                                                                                                     \
        for (auto &kvp : map) {                                                                                        \
            ImGui::PushID(e++);                                                                                        \
                                                                                                                       \
            std::string elementName;                                                                                   \
            if constexpr (std::is_arithmetic<KEY>()) {                                                                 \
                elementName = std::to_string(kvp.first);                                                               \
            } else {                                                                                                   \
                elementName = std::to_string(e);                                                                       \
            }                                                                                                          \
                                                                                                                       \
            draw_ui(ar, elementName.c_str(), kvp.second);                                                              \
                                                                                                                       \
            ImGui::PopID();                                                                                            \
        }                                                                                                              \
                                                                                                                       \
        ImGui::TreePop();                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    ImGui::PopID();

    template <class KEY, class VALUE>
    void
    draw_ui(jleImGuiArchive &ar, std::string name, std::map<KEY, VALUE> &map)
    {
        STD_MAPS_IMPL
    }

    template <class KEY, class VALUE>
    void
    draw_ui(jleImGuiArchive &ar, std::string name, std::unordered_map<KEY, VALUE> &map)
    {
        STD_MAPS_IMPL
    }

    template <class T, std::enable_if_t<!std::is_enum<T>{}> * = nullptr>
    void
    draw_ui(jleImGuiArchive &ar, std::string name, T &value)
    {
        elementCount += 1;

        ImGui::PushID(elementCount);

        if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ar(value);
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    template <class T>
    void
    draw_ui(jleImGuiArchive &ar, const char *name, jleToolTip<T> &value)
    {
        ImGui::PushID(elementCount++);

        draw_ui(ar, name, value.item);

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 220, 180, 255));
        ImGui::Text("   %s", value.tip_view.data());
        ImGui::PopStyleColor();

        ImGui::PopID();
    }

    template <class T>
    void
    draw_ui(jleImGuiArchive &ar, std::string name, jleToolTip<T> &value)
    {
        ImGui::PushID(elementCount++);

        draw_ui(ar, name, value.item);

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 220, 180, 255));
        ImGui::Text("   %s", value.tip_view.data());
        ImGui::PopStyleColor();

        ImGui::PopID();
    }

    template <class T>
    void
    draw_ui(jleImGuiArchive &ar, std::string name, const T &value)
    {
        ImGui::PushID(elementCount++);
        auto cpy = value;
        draw_ui(ar, name.c_str(), cpy);

        ImGui::PopID();
    }

    template <class E, std::enable_if_t<std::is_enum<E>{}> * = nullptr>
    void
    draw_ui(jleImGuiArchive &ar, std::string name, E &e)
    {
        ImGui::PushID(elementCount++);

        static std::unique_ptr<std::vector<std::string>> enumNames{};
        if (!enumNames) {
            enumNames = std::make_unique<std::vector<std::string>>();
            auto vals = magic_enum::enum_names<E>();
            for (auto &v : vals) {
                enumNames->push_back({v.begin(), v.end()});
            }
        }

        std::string currentItem = std::string(magic_enum::enum_name(e));

        ImGui::Text("%s", name.c_str());
        if (ImGui::BeginCombo("##enum_combo", currentItem.c_str())) {
            for (int n = 0; n < enumNames->size(); n++) {
                bool is_selected = (currentItem == enumNames->at(n));
                if (ImGui::Selectable(enumNames->at(n).c_str(), is_selected)) {
                    currentItem = enumNames->at(n);
                    std::optional<E> getter = magic_enum::enum_cast<E>(currentItem);
                    if (getter.has_value()) {
                        e = getter.value();
                    }
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopID();
    }

    template <class T>
    void
    draw_ui(jleImGuiArchive &ar, std::string name, std::shared_ptr<T> &ptr)
    {
        ImGui::PushID(elementCount++);

        ImGui::BeginGroupPanel(name.c_str());

        jleImGuiArchiveInternal archiveInternal{ar.editorCtx};
        archiveInternal(ptr);

        ImGui::EndGroupPanel();

        ImGui::PopID();
    }

    template <class T>
    void
    draw_ui(jleImGuiArchive &ar, const char *name, jleResourceRef<T> &value)
    {
        ImGui::PushID(elementCount++);

        std::string copy = value.path._virtualPath;

        static std::unique_ptr<T> dummyResource;
        if (!dummyResource) {
            dummyResource = std::make_unique<T>();
        }

        auto fileExtensionAssociated = dummyResource->getFileAssociations();

        bool isEditedAndDeactivated = draw_ui_reference(
            std::string{name + std::string{" (ref)"}}.c_str(), value.path._virtualPath, fileExtensionAssociated);
        if (isEditedAndDeactivated) {
            value.load_minimal(ar, value.path._virtualPath);
        }

        ImGui::PopID();
    }

    static std::string LeftLabelImGui(const char *const label, float factor = 0.3f);

public:
    int elementCount = 0;
};

template <class T>
inline void
CEREAL_LOAD_FUNCTION_NAME(jleImGuiArchive &ar, cereal::NameValuePair<T> &t)
{
    std::string newName = t.name;

    if (newName.size() >= 2 && newName[0] == '_' && newName[1] == '_') {
        // Don't render out variables starting with "__".
        return;
    }

    if (newName[0] == '_') {
        newName = newName.substr(1, newName.size());
    }

    if (std::islower(newName[0])) {
        newName[0] = std::toupper(newName[0]);
    }

    ar.draw<T>(ar, newName, t.value);
}

template <class T, cereal::traits::EnableIf<std::is_arithmetic<T>::value> = cereal::traits::sfinae>
inline void
CEREAL_LOAD_FUNCTION_NAME(jleImGuiArchive &ar, T &t)
{
    ar.draw<T>(ar, "", t);
}

template <class T>
inline void
CEREAL_SAVE_FUNCTION_NAME(
    jleImGuiArchiveInternal &ar,
    cereal::NameValuePair<cereal::memory_detail::PtrWrapper<const std::shared_ptr<const T> &>> const &t)
{
    std::shared_ptr<T> f = std::const_pointer_cast<T>(t.value.ptr);

    if (auto component = std::dynamic_pointer_cast<jleComponent>(f)) {
        jleImGuiArchive nonPolymorphicArchive{ar.editorCtx};
        nonPolymorphicArchive.elementCount += 1;

        ImGui::PushID(nonPolymorphicArchive.elementCount);

        if (ImGui::TreeNodeEx(std::string{ar.nextPolymorhphicTypeName + " (ptr)"}.c_str(),
                              ImGuiTreeNodeFlags_DefaultOpen)) {
            nonPolymorphicArchive(*f);
            component->editorInspectorImGuiRender(ar.editorCtx);
            ImGui::TreePop();
        }

        ImGui::PopID();
    } else {
        jleImGuiArchive nonPolymorphicArchive{ar.editorCtx};
        nonPolymorphicArchive.draw(nonPolymorphicArchive, ar.nextPolymorhphicTypeName + " (ptr)", *f.get());
    }
}

template <class T>
inline void
CEREAL_SAVE_FUNCTION_NAME(jleImGuiArchiveInternal &ar, cereal::NameValuePair<T> const &t)
{
    if constexpr (std::is_same_v<std::string &, T>) {
        ar.nextPolymorhphicTypeName = t.value;
    }
}

template <class T, cereal::traits::EnableIf<std::is_arithmetic<T>::value> = cereal::traits::sfinae>
inline void
CEREAL_SAVE_FUNCTION_NAME(jleImGuiArchiveInternal &ar, T &t)
{
    // Do nothing
}

CEREAL_REGISTER_ARCHIVE(jleImGuiArchiveInternal)
CEREAL_REGISTER_ARCHIVE(jleImGuiArchive)

#endif // JLE_BUILD_EDITOR

#endif // JLEIMGUICEREALARCHIVE_H
