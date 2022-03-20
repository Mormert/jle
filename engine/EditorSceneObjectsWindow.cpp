#include "EditorSceneObjectsWindow.h"

#include "jleTypeReflectionUtils.h"

#include "3rdparty/ImGui/imgui.h"
#include "3rdparty/ImGui/imgui_stdlib.h"
#include "3rdparty/ImGui/imgui_impl_glfw.h"
#include "3rdparty/ImGui/imgui_impl_opengl3.h"

#include <sstream>
#include <fstream>
#include <filesystem>

jle::EditorSceneObjectsWindow::EditorSceneObjectsWindow(const std::string &window_name)
        : iEditorImGuiWindow{window_name} {
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

        // Using a static weak_ptr here so that it won't impact deletion
        static std::weak_ptr<jleObject> selectedObject;

        ImGui::BeginGroup();
        ImGui::Text("Object");
        ImGui::BeginChild("objects pane", ImVec2(150 * globalImguiScale, 0), true);

        if (auto selectedSceneSafePtr = selectedScene.lock()) {
            for (auto &&object: selectedSceneSafePtr->GetSceneObjects()) {
                if (ImGui::Selectable(object->mInstanceName.c_str(), selectedObject.lock() == object)) {
                    selectedObject = object;
                }

                if (selectedObject.lock() == object) {

                    { // Destroy Object
                        static bool opened = false;
                        if (ImGui::Button("Destroy Object", ImVec2(138 * globalImguiScale, 0))) {
                            opened = true;
                            ImGui::OpenPopup("Confirm Object Destruction");
                        }

                        if (ImGui::BeginPopupModal("Confirm Object Destruction", &opened, 0)) {
                            if (ImGui::Button("Destroy")) {
                                object->DestroyObject();
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Cancel")) {
                                opened = false;
                            }
                            ImGui::EndPopup();
                        }
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

                   // if (ImGui::Button("Destroy Object", ImVec2(138 * globalImguiScale, 0))) {
                   //     object->DestroyObject();
                    //}
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