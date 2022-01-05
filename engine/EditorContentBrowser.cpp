#include "EditorContentBrowser.h"
#include "ImGui/imgui.h"
#include "plog/Log.h"

jle::EditorContentBrowser::EditorContentBrowser(const std::string &window_name) : iEditorImGuiWindow(window_name) {
    mDirectoryIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath("EditorResources/icons/directory.png");
    mFileIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath("EditorResources/icons/files.png");
}

void jle::EditorContentBrowser::Update(jle::jleGameEngine &ge) {
    if (!isOpened) {
        return;
    }

    ImGui::Begin(window_name.c_str(), &isOpened);

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;
    const ImVec2 fileSize{92 * globalImguiScale, 92 * globalImguiScale};

    float AssetsChildWidth = 300.f;

    ImGui::BeginChild("Assets", ImVec2{AssetsChildWidth, ImGui::GetWindowHeight() - 50}, true);

    for (int i = 0; i < 10; i++)
        ImGui::Text("SomeDir/FileWithLongName...%d", i);

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Content", ImVec2{ImGui::GetWindowWidth() - AssetsChildWidth - 15, ImGui::GetWindowHeight() - 50},true);
    int itemsOnOneRowCount = static_cast<int>((ImGui::GetWindowWidth() - AssetsChildWidth) / fileSize.x);
    if (itemsOnOneRowCount <= 0) {
        itemsOnOneRowCount = 1;
    }

    for (int i = 0; i < 26; i++) {
        if ((i % itemsOnOneRowCount) != 0) {
            ImGui::SameLine();
        }

        ImGui::BeginGroup();

        ImGui::ImageButton(reinterpret_cast<ImTextureID>(mFileIcon->GetTextureID()), fileSize);

        ImGui::Text("image%d.png", i);

        ImGui::Dummy(ImVec2(0.0f, 4.0f * globalImguiScale));

        ImGui::EndGroup();
    }

    ImGui::EndChild();


    ImGui::End();

}
