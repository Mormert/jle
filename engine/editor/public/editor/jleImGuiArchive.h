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

#include "serialization/jleEditorSerialization.h"

#include "core/jleComponent.h"
#include "core/jleFileWatcher.h"
#include "core/jleRGB.h"
#include "core/jleResourceRef.h"
#include "core/serialization/jleExternalSerialization.h"
#include "modules/jleEditorUpdateContext.h"
#include "modules/graphics/jleTextureRefOrRGBA.h"
#include "modules/scripting/jleLuaClassSerialization.h"

#include "jleImGuiExtensions.h"
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <glm/glm.hpp>

#include <cereal/cereal.hpp>
#include <cereal/details/helpers.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>

#include <magic_enum/magic_enum.hpp>

#include <Tracy.hpp>

#include <filesystem>
#include <vector>

class jleImGuiArchiveInternal : public jleSerializationArchive_EditorOnly,
                                public cereal::OutputArchive<jleImGuiArchiveInternal>
{
public:
    explicit jleImGuiArchiveInternal(jleEditorUpdateContext &context)
        : jleSerializationArchive_EditorOnly(context.engineUpdateContext.serializationContext, context),
          OutputArchive<jleImGuiArchiveInternal>(this)
    {
    }

    ~jleImGuiArchiveInternal() override = default;

    std::string nextPolymorhphicTypeName{};
};

class jleImGuiArchive : public jleSerializationArchive_EditorOnly, public cereal::InputArchive<jleImGuiArchive>
{
public:
    explicit jleImGuiArchive(jleEditorUpdateContext &context)
        : jleSerializationArchive_EditorOnly(context.engineUpdateContext.serializationContext, context),
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
    void draw_ui(jleImGuiArchive &ar, const char *name, uint8_t &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, int8_t &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, uint16_t &value);
    void draw_ui(jleImGuiArchive &ar, const char *name, int16_t &value);
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

        std::string copy = value.getPath().isEmpty() ? "" : value.getPath().getVirtualPath().str();

        static std::unique_ptr<T> dummyResource;
        if (!dummyResource) {
            dummyResource = std::make_unique<T>();
        }

        auto fileExtensionAssociated = dummyResource->getFileAssociations();

        bool isEditedAndDeactivated = draw_ui_reference(std::string{name + std::string{" (ref)"}}.c_str(), copy, fileExtensionAssociated);
        if (isEditedAndDeactivated) {
            jlePath newPath{jleVirtualPath{copy.c_str()}};
            value.load_minimal(ar, newPath.getHash());
        }

        ImGui::PopID();
    }

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

    jleImGuiArchive nonPolymorphicArchive{ar.editorCtx};
    nonPolymorphicArchive.draw(nonPolymorphicArchive, ar.nextPolymorhphicTypeName + " (ptr)", *f.get());
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
