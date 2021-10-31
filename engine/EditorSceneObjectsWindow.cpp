#include "EditorSceneObjectsWindow.h"

#include "jleObjectTypeUtils.h"


#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

#include <sstream>

jle::EditorSceneObjectsWindow::EditorSceneObjectsWindow(const std::string& window_name)
    : iEditorImGuiWindow{ window_name }
{
}

void jle::EditorSceneObjectsWindow::Update(jleGameEngine& ge)
{
    if (!isOpened)
    {
        return;
    }

    // Using a static weak_ptr here so that it won't impact deletion
    static std::weak_ptr<jleScene> selectedScene;

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Create Scene"))
            {
                if (ImGui::MenuItem("jleScene"))
                {
                    ge.GetGameRef().CreateScene<jleScene>();
                }

                ImGui::EndMenu();
            }

            if (selectedScene.lock() && !selectedScene.lock()->bPendingSceneDestruction)
            {
                if (ImGui::BeginMenu("Create Object"))
                {
                    for (auto objectType : jleObjectTypeUtils::GetRegisteredObjectsRef())
                    {
                        if (ImGui::MenuItem(objectType.first.c_str()))
                        {
                            selectedScene.lock()->SpawnObject(objectType.first.c_str());
                        }
                    }
                    ImGui::EndMenu();
                }
            }

            ImGui::EndMenuBar();
        }

        ImGui::BeginGroup();
        ImGui::Text("Scene");
        ImGui::BeginChild("scene pane", ImVec2(150, 0), true);

        auto& activeScenes = ((jleGameEngine*)jleCore::core)->GetGameRef().GetActiveScenesRef();

        for (auto scene : activeScenes)
        {
            if (ImGui::Selectable(scene->mSceneName.c_str(), selectedScene.lock() == scene))
            {
                selectedScene = scene;
            }

            if (selectedScene.lock() == scene)
            {
                if (ImGui::Button("Destroy Scene", ImVec2(138, 0)))
                {
                    scene->DestroyScene();
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
        ImGui::BeginChild("objects pane", ImVec2(150, 0), true);

        if (auto selectedSceneSafePtr = selectedScene.lock())
        {
            for (auto object : selectedSceneSafePtr->GetSceneObjects())
            {
                if (ImGui::Selectable(object->mInstanceName.c_str(), selectedObject.lock() == object))
                {
                    selectedObject = object;
                }

                if (selectedObject.lock() == object)
                {
                    if (ImGui::Button("Destroy Object", ImVec2(138, 0)))
                    {
                        object->DestroyObject();
                    }
                }
            }
        }

        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::SameLine();

        // Right
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("selected object view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            bool hasAnObjectSelected = false;
            std::shared_ptr<jleObject> selectedObjectSafePtr;
            if (selectedObjectSafePtr = selectedObject.lock())
            {
                ImGui::Text(selectedObjectSafePtr->mInstanceName.c_str());
                hasAnObjectSelected = true;
            }
            else
            {
                ImGui::Text("No object selected");
            }

            static nlohmann::json selectedObjectJson;
            static std::weak_ptr<jleObject> lastSelectedObject;
            if (hasAnObjectSelected)
            {
                ImGui::Separator();
                if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
                {
                    if (ImGui::BeginTabItem("Object Properties"))
                    {
                        // Check to see if a new object has been selected
                        if (selectedObjectSafePtr != lastSelectedObject.lock())
                        {
                            selectedObjectSafePtr->ToJson(selectedObjectJson);
                            lastSelectedObject = selectedObjectSafePtr;

                            mJsonToImgui.JsonToImgui(selectedObjectJson);
                        }

                        // This does calls to ImGui:: to draw the editable object properties
                        mJsonToImgui.DrawAndGetInput();

                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem("Details"))
                    {
                        
                        ImGui::Text("Object type   : %s", selectedObjectSafePtr->GetObjectNameVirtual());
                        ImGui::Text("Instance name : %s", selectedObjectSafePtr->mInstanceName.c_str());
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }

            ImGui::EndChild();
            if (hasAnObjectSelected)
            {
                if (ImGui::Button("Refresh Object"))
                {
                    lastSelectedObject.reset();
                }
                ImGui::SameLine();
                if (ImGui::Button("Push Object Changes"))
                {
                    auto pushedObjectJson = mJsonToImgui.ImGuiToJson();
                    selectedObjectSafePtr->FromJson(pushedObjectJson);
                }
            }
            ImGui::EndGroup();
        }
    }
    ImGui::End();
}