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

#include "jleECSEditorWindow.h"
#include <modules/graphics/runtime/components/cLight.h>
#include <modules/graphics/runtime/components/cMesh.h>

#include "modules/game/jleGame.h"

#include "core/jlECSSaveLoad.h"

jleECSEditorWindow::jleECSEditorWindow(const std::string &window_name) : jleEditorWindowInterface(window_name) {}

void
jleECSEditorWindow::renderUI(jleEditorModulesContext &ctx)
{
    if (!isOpened) {
        return;
    }

    if(ctx.engineModulesContext.gameRuntime.isGameKilled())
    {
        return;
    }

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

    auto &ecs = ctx.engineModulesContext.gameRuntime.getGame().getECS();

    if (_ecs != &ecs) {
        _selectedObject.reset();
        _ecs = &ecs;
    }

    ImGui::BeginGroup();

    if (ImGui::Button("Save")) {

        auto start = std::chrono::high_resolution_clock::now();


        std::ofstream i("save.ecs");

        jleSerializationContext serializationContext{&ctx.engineModulesContext.resourcesModule,
                                                     &ctx.engineModulesContext.luaEnvironment,
                                                     &ctx.engineModulesContext.renderThread};
        jleJSONOutputArchive ar{i, serializationContext};


        jlECS::save(ecs, ar);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "jlECS::save execution time: " << duration.count() << " ms" << std::endl;
    }

    if (ImGui::Button("SaveBinary")) {

        auto start = std::chrono::high_resolution_clock::now();

        std::ofstream i("saveB.ecs", std::ios::binary);


        jleSerializationContext serializationContext{&ctx.engineModulesContext.resourcesModule,
                                                     &ctx.engineModulesContext.luaEnvironment,
                                                     &ctx.engineModulesContext.renderThread};
        jleBinaryOutputArchive ar{i, serializationContext};


        jlECS::save(ecs, ar);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "jlECS::save bin execution time: " << duration.count() << " ms" << std::endl;
    }

    if (ImGui::Button("Load")) {

        _selectedObject.reset();
        ecs.reset();

        auto start = std::chrono::high_resolution_clock::now();

        std::ifstream i("save.ecs");

        jleSerializationContext serializationContext{&ctx.engineModulesContext.resourcesModule,
                                                     &ctx.engineModulesContext.luaEnvironment,
                                                     &ctx.engineModulesContext.renderThread};
        jleJSONInputArchive ar{i, serializationContext};



        jlECS::load(ecs, ar);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "jlECS::load execution time: " << duration.count() << " ms" << std::endl;
    }

    if (ImGui::Button("LoadBinary")) {

        _selectedObject.reset();
        ecs.reset();

        auto start = std::chrono::high_resolution_clock::now();

        std::ifstream i("saveB.ecs", std::ios::binary);

        jleSerializationContext serializationContext{&ctx.engineModulesContext.resourcesModule,
                                                     &ctx.engineModulesContext.luaEnvironment,
                                                     &ctx.engineModulesContext.renderThread};
        jleBinaryInputArchive ar{i, serializationContext};


        jlECS::load(ecs, ar);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "jlECS::load bin execution time: " << duration.count() << " ms" << std::endl;
    }

    ImGui::Text("Objects");
    ImGui::BeginChild("objects pane", ImVec2(280 * globalImguiScale, 0), true);

    auto *objectsDebug = ecs.getAllObjectsDebug();

    int i = 0;
    for (jlECS::ObjectRef &object : *objectsDebug) {
        std::string str = std::to_string(object.objectIndex()) + " <";

        auto components = object.componentsDebug2();
        bool first = true;
        for (auto &component : components) {
            if (!first) {
                str += ", ";
            }
            str += component->getName();
            first = false;
        }
        str += ">";

        if (ImGui::Selectable(str.c_str(), _selectedObject.has_value() && _selectedObject.value() == object)) {
            _selectedObject = object;
        }

        ++i;
    }

    if (ImGui::Button("Add Object")) {
        ecs.instantiateObject();
    }

    ImGui::EndChild();
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("Selected Object");
    ImGui::BeginChild("selected object pane", ImVec2(280 * globalImguiScale, 0), true);

    if (_selectedObject.has_value()) {
        if (_selectedObject.value().isValid()) {

            if (ImGui::Button("Destroy Object")) {
                ecs.destroyObject(_selectedObject.value());
            }

            ImGui::Text("Components");

            auto components = _selectedObject.value().componentsDebug2();
            for (auto &comp : components) {
                ImGui::PushID(i);
                ImGui::BeginGroupPanel(comp->getName());

                jleImGuiArchive ar{ctx};
                comp->serializeComponent(ar);

                int compIdx = comp->componentIndex;

                std::string removeString = "Remove " + std::string{comp->getName()};
                if (ImGui::Button(removeString.c_str())) {
                    comp->removeFromOwningObject();
                    _selectedObject.value().clearComponentsDebug();
                    ImGui::EndGroupPanel();
                    ImGui::PopID();
                    break;
                }

                ImGui::EndGroupPanel();
                ImGui::PopID();
            }

            if (ImGui::BeginMenu("Add Component")) {
                const auto &registeredComponentTypes = ecs.getRegisteredComponents();

                for (auto registeredComponentType : registeredComponentTypes) {
                    if (!ecs.getComponent(_selectedObject->objectIndex(), registeredComponentType.componentType) &&
                        ImGui::MenuItem(registeredComponentType.componentTypeName)) {
                        ecs.addComponent(_selectedObject->objectIndex(), registeredComponentType.componentType);
                    }
                }
                ImGui::EndMenu();
            }
        }
    }

    ImGui::EndChild();
    ImGui::EndGroup();

    ImGui::End();
}

void
jleECSEditorWindow::update(jleResourceHolder &resourcesModule)
{
}
