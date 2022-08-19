// Copyright (c) 2022. Johan Lind

#include "EditorResourceViewer.h"
#include "jleResourceHolder.h"
#include "3rdparty/ImGui/imgui.h"

void jle::EditorResourceViewer::Update(jle::jleGameEngine &ge) {
    if (!isOpened) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_MenuBar)) {

        std::vector<std::string> resourcesToBeUnloaded;
        for (auto &&drive: jleResourceHolder::GetResourcesMap()) {
            const std::string treeNodeStr = drive.first + " (" + std::to_string(drive.second.size()) + ")";

            const bool open = ImGui::TreeNode(treeNodeStr.c_str());
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::Button("Unload Everything")) {
                    jleResourceHolder::UnloadAllResources(drive.first);
                }
                ImGui::EndPopup();
            }
            if (open) {

                for (auto &&path: drive.second) {
                    const std::string pathStr =
                            path.first + " (" + std::to_string(path.second.use_count() - 1) + " users)";

                    if (ImGui::TreeNode(pathStr.c_str())) {
                        if (ImGui::Button("Unload")) {
                            resourcesToBeUnloaded.push_back(path.first);
                        }
                        ImGui::TreePop();
                    }


                }
                ImGui::TreePop();
            }

        }

        // Postpone the unloading to outside the loop
        for (auto &&resource: resourcesToBeUnloaded) {
            jleResourceHolder::UnloadResource(jleRelativePath{resource});
        }

        ImGui::End();
    }
}


jle::EditorResourceViewer::EditorResourceViewer(const std::string &window_name)
        : iEditorImGuiWindow{window_name} {}
