#include "EditorSceneObjectsWindow.h"

#include "jleObjectTypeUtils.h"


#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"


jle::EditorSceneObjectsWindow::EditorSceneObjectsWindow(const std::string& window_name)
: iEditorImGuiWindow { window_name }
{
}

void jle::EditorSceneObjectsWindow::Update(jleGameEngine& ge)
{
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Close")) isOpened = false;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Using a static weak_ptr here so that it won't impact deletion
        static std::weak_ptr<jleScene> selectedScene;

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
        }

        ImGui::EndChild();
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::Text("Object");
        ImGui::BeginChild("objects pane", ImVec2(150, 0), true);
        
        if (auto selectedSceneSafePtr = selectedScene.lock())
        {
            for (auto object : selectedSceneSafePtr->GetSceneObjects())
            {
                if (ImGui::Selectable(std::string{ object->GetObjectNameVirtual() }.c_str(), false))
                {

                }
            }
        }
        
        ImGui::EndChild();
        ImGui::EndGroup();

            /*for (int i = 0; i < 100; i++)
            {
                // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
                char label[128];
                sprintf_s(label, "MyObject %d", i);
                if (ImGui::Selectable(("Object " + std::to_string(i)).c_str(), selected == i))
                {
                    selected = i;
                }       
            }
            ImGui::EndChild();*/
       // }
        /*ImGui::SameLine();

        ImGui::BeginChild("object pane", ImVec2(150, 0), true);
        for (int i = 0; i < 100; i++)
        {
            // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
            char label[128];
            sprintf_s(label, "MyObject %d", i);
            if (ImGui::Selectable(("Object " + std::to_string(i)).c_str(), selected == i))
            {
                selected = i;
            }
        }
        ImGui::EndChild();*/
        // }
        ImGui::SameLine();

        // Right
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            ImGui::Text("MyObject: 1");
            ImGui::Separator();
            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Description"))
                {
                    ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Details"))
                {
                    ImGui::Text("ID: 0123456789");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndChild();
            if (ImGui::Button("Revert")) {}
            ImGui::SameLine();
            if (ImGui::Button("Save")) {}
            ImGui::EndGroup();
        }
    }
    ImGui::End();
}