// Copyright (c) 2023. Johan Lind

#include "jleEditorSceneObjectsWindow.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
#include "jleImGuiCerealArchive.h"
#include "jleNetScene.h"
#include "jleTypeReflectionUtils.h"

#include <cereal/archives/json.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>

jleEditorSceneObjectsWindow::jleEditorSceneObjectsWindow(const std::string &window_name)
    : iEditorImGuiWindow{window_name}
{
}

std::weak_ptr<jleObject> &
jleEditorSceneObjectsWindow::GetSelectedObject()
{
    return selectedObject;
}

void
jleEditorSceneObjectsWindow::update(jleGameEngine &ge)
{
    if (!isOpened) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_MenuBar)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Create Scene")) {
                if (ImGui::MenuItem("jleScene")) {
                    ge.gameRef().createScene<jleScene>();
                }
                if (ImGui::MenuItem("jleNetScene")) {
                    ge.gameRef().createScene<jleNetScene>();
                }

                ImGui::EndMenu();
            }

            if (selectedScene.lock() && !selectedScene.lock()->bPendingSceneDestruction) {
                if (ImGui::BeginMenu("Create Object")) {
                    for (auto &&objectType : jleTypeReflectionUtils::registeredObjectsRef()) {
                        if (ImGui::MenuItem(objectType.first.c_str())) {
                            selectedScene.lock()->spawnObject(objectType.first);
                        }
                    }
                    ImGui::EndMenu();
                }
            }

            ImGui::EndMenuBar();
        }

        ImGui::BeginGroup();
        ImGui::Text("Scene");
        const float globalImguiScale = ImGui::GetIO().FontGlobalScale;
        ImGui::BeginChild("scene pane", ImVec2(150 * globalImguiScale, 0), true);

        auto &activeScenes = ((jleGameEngine *)gCore)->gameRef().activeScenesRef();

        for (auto scene : activeScenes) {
            if (ImGui::Selectable(scene->sceneName.c_str(), selectedScene.lock() == scene)) {
                selectedScene = scene;
            }

            if (selectedScene.lock() == scene) {

                { // destroy Scene
                    static bool opened = false;
                    if (ImGui::Button("Destroy Scene", ImVec2(138 * globalImguiScale, 0))) {
                        opened = true;
                        ImGui::OpenPopup("Confirm Scene destroy");
                    }

                    if (ImGui::BeginPopupModal("Confirm Scene destroy", &opened, 0)) {
                        if (ImGui::Button("Destroy")) {
                            scene->destroyScene();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel")) {
                            opened = false;
                        }
                        ImGui::EndPopup();
                    }
                }

                { // Rename Scene
                    static bool opened = false;
                    static std::string buf;
                    if (ImGui::Button("Rename Scene", ImVec2(138 * globalImguiScale, 0))) {
                        opened = true;
                        buf = std::string{scene->sceneName};
                        ImGui::OpenPopup("Rename Scene");
                    }

                    if (ImGui::BeginPopupModal("Rename Scene", &opened, 0)) {
                        ImGui::InputText("Scene Name", &buf);
                        if (ImGui::Button("Confirm")) {
                            scene->sceneName = std::string{buf};
                            opened = false;
                        }
                        ImGui::EndPopup();
                    }
                }

                { // Save Scene
                    if (ImGui::Button("Save Scene", ImVec2(138 * globalImguiScale, 0))) {
                        std::filesystem::create_directories(GAME_RESOURCES_DIRECTORY + "/scenes");
                        std::ofstream sceneSave{GAME_RESOURCES_DIRECTORY + "/scenes/" + scene->sceneName + ".scn"};
                        nlohmann::json j;
                        to_json(j, *scene);
                        // sceneSave << j.dump(4);
                        // sceneSave.close();

                        std::stringstream outputSS;

                        {
                            cereal::JSONOutputArchive outputArchive(sceneSave);
                            scene->serialize(outputArchive);
                        }

                        // DEBUG SCENE SAVE...

                        std::cout << "JSON:\n";
                        std::cout << j.dump(4) << std::endl;

                        std::cout << "CEREAL:\n";
                        std::string outputStr = outputSS.str();
                        std::cout << outputStr << std::endl;
                    }
                }
            }
        }

        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::Text("Object");
        ImGui::BeginChild("objects pane", ImVec2(280 * globalImguiScale, 0), true);

        if (auto selectedSceneSafePtr = selectedScene.lock()) {
            auto &sceneObjectsRef = selectedSceneSafePtr->sceneObjects();
            for (int32_t i = sceneObjectsRef.size() - 1; i >= 0; i--) {
                if (sceneObjectsRef[i]) {
                    objectTreeRecursive(sceneObjectsRef[i]);
                }
            }
        }

        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::SameLine();

        // Right
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("selected object view",
                              ImVec2(0,
                                     -ImGui::GetFrameHeightWithSpacing())); // Leave room for
                                                                            // 1 line below us
            bool hasAnObjectSelected = false;
            std::shared_ptr<jleObject> selectedObjectSafePtr;
            if (selectedObjectSafePtr = selectedObject.lock()) {
                auto text = std::string_view{selectedObjectSafePtr->_instanceName};
                ImGui::Text("%.*s", static_cast<int>(text.size()), text.data());
                hasAnObjectSelected = true;
            } else {
                ImGui::Text("No object selected");
            }

            static std::weak_ptr<jleObject> lastSelectedObject;
            if (hasAnObjectSelected) {
                ImGui::Separator();
                if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
                    if (ImGui::BeginTabItem("Object Properties")) {

                        // If this is an object template
                        if (selectedObjectSafePtr->_templatePath.has_value()) {
                            ImGui::Text("Object template: %s", selectedObjectSafePtr->_templatePath->c_str());
                            ImGui::NewLine();
                        }

                        cereal::jleImGuiCerealArchive ar1;
                        ar1(*selectedObjectSafePtr);

                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Custom Components")) {
                        if (ImGui::BeginMenu("Add Custom Component")) {
                            for (auto &&componentType : jleTypeReflectionUtils::registeredComponentsRef()) {
                                if (ImGui::MenuItem(componentType.first.c_str())) {
                                    selectedObjectSafePtr->addComponent(componentType.first);
                                    lastSelectedObject.reset(); // refresh
                                }
                            }
                            ImGui::EndMenu();
                        }

                        auto &&customComponents = selectedObjectSafePtr->customComponents();
                        if (customComponents.size() > 0) {
                            if (ImGui::BeginMenu("Remove Custom Component")) {
                                for (int i = customComponents.size() - 1; i >= 0; i--) {
                                    if (ImGui::MenuItem(customComponents[i]->componentName().data())) {
                                        customComponents[i]->destroy();
                                        lastSelectedObject.reset(); // refresh
                                    }
                                }
                                ImGui::EndMenu();
                            }
                        }

                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Details")) {
                        {
                            auto name = selectedObjectSafePtr->objectNameVirtual();
                            ImGui::Text("Object type   : %.*s", static_cast<int>(name.size()), name.data());
                        }
                        ImGui::Text("Instance name : %s", selectedObjectSafePtr->_instanceName.c_str());
                        ImGui::Text("Components attached count: %d", selectedObjectSafePtr->componentCount());
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }

            ImGui::EndChild();

            ImGui::EndGroup();
        }
    }
    ImGui::End();
}

void
jleEditorSceneObjectsWindow::SetSelectedObject(std::shared_ptr<jleObject> object)
{
    selectedObject = object;
}

void
jleEditorSceneObjectsWindow::objectTreeRecursive(std::shared_ptr<jleObject> object)
{
    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

    std::string instanceDisplayName = object->_instanceName;
    if (object->_templatePath.has_value()) {
        instanceDisplayName += " [T]";
    }

    ImGui::PushID(object->instanceID()); // push instance id
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
    bool open = ImGui::TreeNodeEx(object->_instanceName.c_str(),
                                  ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen |
                                      (selectedObject.lock() == object ? ImGuiTreeNodeFlags_Selected : 0) |
                                      (object->childObjects().empty() ? ImGuiTreeNodeFlags_Leaf : 0),
                                  "%s",
                                  instanceDisplayName.c_str());
    ImGui::PopStyleVar();
    ImGui::PopID(); // pop instance id

    ImGui::PushID(object->_instanceName.c_str());
    if (ImGui::BeginPopupContextItem()) {

        if (ImGui::BeginMenu("Create Object")) {
            for (auto &&objectType : jleTypeReflectionUtils::registeredObjectsRef()) {
                if (ImGui::MenuItem(objectType.first.c_str())) {
                    object->spawnChildObject(objectType.first);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::Button("Destroy Object", ImVec2(138 * globalImguiScale, 0))) {
            object->destroyObject();
        }

        if (ImGui::Button("Save Template", ImVec2(138 * globalImguiScale, 0))) {
            jleRelativePath p{""};
            object->saveObjectTemplate(p);
        }

        if (ImGui::Button("Duplicate", ImVec2(138 * globalImguiScale, 0))) {
            jleRelativePath p{""};
            nlohmann::json j;
            object->duplicate();
        }

        { // Rename Object
            static bool opened = false;
            static std::string buf;
            if (ImGui::Button("Rename Object", ImVec2(138 * globalImguiScale, 0))) {
                opened = true;
                buf = std::string{object->_instanceName};
                ImGui::OpenPopup("Rename Object");
            }

            if (ImGui::BeginPopupModal("Rename Object", &opened, 0)) {
                ImGui::InputText("Object Name", &buf);
                if (ImGui::Button("Confirm")) {
                    object->_instanceName = std::string{buf};
                    opened = false;
                }
                ImGui::EndPopup();
            }
        }

        if (object->parent()) {
            if (ImGui::Button("Detach Object", ImVec2(138 * globalImguiScale, 0))) {
                object->detachObjectFromParent();
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopID();

    if (ImGui::IsItemClicked()) {
        selectedObject = object;
    }

    if (ImGui::BeginDragDropTarget()) {
        ImGuiDragDropFlags target_flags = 0;
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("JLE_OBJECT", target_flags)) {
            auto moveFrom = *(std::shared_ptr<jleObject> *)payload->Data;

            // We check if the object is the owner of the object that it is
            // being attached to If that's the case, we first detach the object
            // from it's parent, and then attach it to the target object
            bool canAttachDirectly = true;
            jleObject *o = object->parent();
            while (o) {
                if (moveFrom.get() == o) {
                    canAttachDirectly = false;
                    break;
                }
                o = o->parent();
            }
            if (canAttachDirectly) {
                object->attachChildObject(moveFrom);
            } else {
                object->detachObjectFromParent();
                object->attachChildObject(moveFrom);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::Text("Moving object: %s", object->_instanceName.c_str());
        ImGui::SetDragDropPayload("JLE_OBJECT", &object, sizeof(std::shared_ptr<jleObject>));
        ImGui::EndDragDropSource();
    }

    if (open) {
        auto &childObjectsRef = object->childObjects();
        for (int32_t i = childObjectsRef.size() - 1; i >= 0; i--) {
            objectTreeRecursive(childObjectsRef[i]);
        }
        ImGui::TreePop();
    }
}

std::weak_ptr<jleScene> &
jleEditorSceneObjectsWindow::GetSelectedScene()
{
    return selectedScene;
}
