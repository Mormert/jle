// Copyright (c) 2022. Johan Lind

#include "EditorSceneObjectsWindow.h"

#include "jleTypeReflectionUtils.h"

#include "3rdparty/ImGui/imgui.h"
#include "3rdparty/ImGui/imgui_stdlib.h"

#include <fstream>
#include <filesystem>

jle::EditorSceneObjectsWindow::EditorSceneObjectsWindow(const std::string &window_name)
        : iEditorImGuiWindow{window_name} {
}


std::weak_ptr<jle::jleObject> &jle::EditorSceneObjectsWindow::GetSelectedObject() {
    return selectedObject;
}

void jle::EditorSceneObjectsWindow::Update(jleGameEngine &ge) {
    if (!isOpened) {
        return;
    }

    // Using a static weak_ptr here so that it won't impact deletion
    static std::weak_ptr<jleScene> selectedScene;
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_MenuBar)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Create Scene")) {
                if (ImGui::MenuItem("jleScene")) {
                    ge.GetGameRef().CreateScene<jleScene>();
                }

                ImGui::EndMenu();
            }

            if (selectedScene.lock() && !selectedScene.lock()->bPendingSceneDestruction) {
                if (ImGui::BeginMenu("Create Object")) {
                    for (auto &&objectType: jleTypeReflectionUtils::GetRegisteredObjectsRef()) {
                        if (ImGui::MenuItem(objectType.first.c_str())) {
                            selectedScene.lock()->SpawnObject(objectType.first);
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

        auto &activeScenes = ((jleGameEngine *) jleCore::core)->GetGameRef().GetActiveScenesRef();

        for (auto scene: activeScenes) {
            if (ImGui::Selectable(scene->mSceneName.c_str(), selectedScene.lock() == scene)) {
                selectedScene = scene;
            }

            if (selectedScene.lock() == scene) {

                { // Destroy Scene
                    static bool opened = false;
                    if (ImGui::Button("Destroy Scene", ImVec2(138 * globalImguiScale, 0))) {
                        opened = true;
                        ImGui::OpenPopup("Confirm Scene Destroy");
                    }

                    if (ImGui::BeginPopupModal("Confirm Scene Destroy", &opened, 0)) {
                        if (ImGui::Button("Destroy")) {
                            scene->DestroyScene();
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
                        buf = std::string{scene->mSceneName};
                        ImGui::OpenPopup("Rename Scene");
                    }

                    if (ImGui::BeginPopupModal("Rename Scene", &opened, 0)) {
                        ImGui::InputText("Scene Name", &buf);
                        if (ImGui::Button("Confirm")) {
                            scene->mSceneName = std::string{buf};
                            opened = false;
                        }
                        ImGui::EndPopup();
                    }
                }

                { // Save Scene
                    if (ImGui::Button("Save Scene", ImVec2(138 * globalImguiScale, 0))) {
                        std::filesystem::create_directories(GAME_RESOURCES_DIRECTORY + "/scenes");
                        std::ofstream sceneSave{GAME_RESOURCES_DIRECTORY + "/scenes/" + scene->mSceneName + ".scn"};
                        nlohmann::json j;
                        jle::to_json(j, *scene);
                        sceneSave << j.dump(4);
                        sceneSave.close();
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
            auto &sceneObjectsRef = selectedSceneSafePtr->GetSceneObjects();
            for (int32_t i = sceneObjectsRef.size() - 1; i >= 0; i--) {
                if (sceneObjectsRef[i]) {
                    ObjectTreeRecursive(sceneObjectsRef[i]);
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
                              ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            bool hasAnObjectSelected = false;
            std::shared_ptr<jleObject> selectedObjectSafePtr;
            if (selectedObjectSafePtr = selectedObject.lock()) {
                ImGui::Text(selectedObjectSafePtr->mInstanceName.c_str());
                hasAnObjectSelected = true;
            } else {
                ImGui::Text("No object selected");
            }

            static std::weak_ptr<jleObject> lastSelectedObject;
            if (hasAnObjectSelected) {
                ImGui::Separator();
                if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
                    if (ImGui::BeginTabItem("Object Properties")) {
                        // Check to see if a new object has been selected
                        if (selectedObjectSafePtr != lastSelectedObject.lock()) {
                            nlohmann::json selectedObjectJson;
                            selectedObjectSafePtr->ToJson(selectedObjectJson);

                            selectedObjectJson["_custom_components"] = selectedObjectSafePtr->GetCustomComponents();
                            //selectedObjectJson["_childObjects"] = selectedObjectSafePtr->GetChildObjects();

                            lastSelectedObject = selectedObjectSafePtr;

                            mJsonToImgui.JsonToImgui(selectedObjectJson,
                                                     {std::string{selectedObjectSafePtr->GetObjectNameVirtual()}});
                        }

                        // This does calls to ImGui:: to draw the editable object properties
                        mJsonToImgui.DrawAndGetInput();

                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Custom Components")) {
                        if (ImGui::BeginMenu("Add Custom Component")) {
                            for (auto &&componentType: jleTypeReflectionUtils::GetRegisteredComponentsRef()) {
                                if (ImGui::MenuItem(componentType.first.c_str())) {
                                    selectedObjectSafePtr->AddCustomComponent(componentType.first, true);
                                    lastSelectedObject.reset(); // refresh
                                }
                            }
                            ImGui::EndMenu();
                        }

                        auto &&customComponents = selectedObjectSafePtr->GetCustomComponents();
                        if (customComponents.size() > 0) {
                            if (ImGui::BeginMenu("Remove Custom Component")) {
                                for (int i = customComponents.size() - 1; i >= 0; i--) {
                                    if (ImGui::MenuItem(customComponents[i]->GetComponentName().data())) {
                                        customComponents[i]->Destroy();
                                        lastSelectedObject.reset(); // refresh
                                    }
                                }
                                ImGui::EndMenu();
                            }
                        }

                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Details")) {
                        ImGui::Text("Object type   : %s", selectedObjectSafePtr->GetObjectNameVirtual());
                        ImGui::Text("Instance name : %s", selectedObjectSafePtr->mInstanceName.c_str());
                        ImGui::Text("Components attached count: %d", selectedObjectSafePtr->GetComponentCount());
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }

            ImGui::EndChild();
            if (hasAnObjectSelected) {
                if (ImGui::Button("Refresh Object")) {
                    lastSelectedObject.reset();
                }
                ImGui::SameLine();
                if (ImGui::Button("Push Object Changes")) {
                    auto pushedObjectJson = mJsonToImgui.ImGuiToJson();
                    jle::from_json(pushedObjectJson, selectedObjectSafePtr);
                    selectedObjectSafePtr->FromJson(pushedObjectJson);

                }
            }
            ImGui::EndGroup();
        }
    }
    ImGui::End();
}

void jle::EditorSceneObjectsWindow::SetSelectedObject(std::shared_ptr<jleObject> object) {
    selectedObject = object;
}

void jle::EditorSceneObjectsWindow::ObjectTreeRecursive(std::shared_ptr<jleObject> object) {
    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
    bool open = ImGui::TreeNodeEx(object->mInstanceName.c_str(),
                                  ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen |
                                  (selectedObject.lock() == object ? ImGuiTreeNodeFlags_Selected : 0) |
                                  (object->GetChildObjects().empty() ? ImGuiTreeNodeFlags_Leaf : 0),
                                  "%s", object->mInstanceName.c_str());
    ImGui::PopStyleVar();

    ImGui::PushID(object->mInstanceName.c_str());
    if (ImGui::BeginPopupContextItem()) {

        if (ImGui::BeginMenu("Create Object")) {
            for (auto &&objectType: jleTypeReflectionUtils::GetRegisteredObjectsRef()) {
                if (ImGui::MenuItem(objectType.first.c_str())) {
                    object->SpawnChildObject(objectType.first);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::Button("Destroy Object", ImVec2(138 * globalImguiScale, 0))) {
            object->DestroyObject();
        }

        { // Rename Object
            static bool opened = false;
            static std::string buf;
            if (ImGui::Button("Rename Object", ImVec2(138 * globalImguiScale, 0))) {
                opened = true;
                buf = std::string{object->mInstanceName};
                ImGui::OpenPopup("Rename Object");
            }

            if (ImGui::BeginPopupModal("Rename Object", &opened, 0)) {
                ImGui::InputText("Object Name", &buf);
                if (ImGui::Button("Confirm")) {
                    object->mInstanceName = std::string{buf};
                    opened = false;
                }
                ImGui::EndPopup();
            }
        }

        if (object->GetParent()) {
            if (ImGui::Button("Detach Object", ImVec2(138 * globalImguiScale, 0))) {
                object->DetachObjectFromParent();
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
            auto moveFrom = *(std::shared_ptr<jleObject> *) payload->Data;

            // We check if the object is the owner of the object that it is being attached to
            // If that's the case, we first detach the object from it's parent, and then attach it
            // to the target object
            bool canAttachDirectly = true;
            jleObject *o = object->GetParent();
            while (o) {
                if (moveFrom.get() == o) {
                    canAttachDirectly = false;
                    break;
                }
                o = o->GetParent();
            }
            if (canAttachDirectly) {
                object->AttachChildObject(moveFrom);
            } else {
                object->DetachObjectFromParent();
                object->AttachChildObject(moveFrom);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::Text("Moving object: %s", object->mInstanceName.c_str());
        ImGui::SetDragDropPayload("JLE_OBJECT", &object, sizeof(std::shared_ptr<jleObject>));
        ImGui::EndDragDropSource();
    }

    if (open) {
        auto &childObjectsRef = object->GetChildObjects();
        for (int32_t i = childObjectsRef.size() - 1; i >= 0; i--) {
            ObjectTreeRecursive(childObjectsRef[i]);
        }
        ImGui::TreePop();
    }
}


