#include "EditorContentBrowser.h"
#include "ImGui/imgui.h"
#include "plog/Log.h"

#include <string>

jle::EditorContentBrowser::EditorContentBrowser(const std::string &window_name) : iEditorImGuiWindow(window_name) {
    mDirectoryIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath(
            JLE_ENGINE_PATH + "/EditorResources/icons/directory.png");
    mFileIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath(
            JLE_ENGINE_PATH + "/EditorResources/icons/files.png");
    mBackDirectoryIcon = jle::jleCore::core->texture_creator->LoadTextureFromPath(
            JLE_ENGINE_PATH + "/EditorResources/icons/back_directory.png");

    mSelectedDirectory = GAME_RESOURCES_DIRECTORY;
}

#define BIT(x) (1 << x)

std::pair<bool, uint32_t>
jle::EditorContentBrowser::DirectoryTreeViewRecursive(const std::filesystem::path &path, uint32_t *count,
                                                      int *selection_mask) {
    ImGuiTreeNodeFlags base_flags =
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_SpanFullWidth;

    bool any_node_clicked = false;
    uint32_t node_clicked = 0;

    for (const auto &entry: std::filesystem::directory_iterator(path)) {
        ImGuiTreeNodeFlags node_flags = base_flags;
        const bool is_selected = (*selection_mask & BIT(*count)) != 0;
        if (is_selected)
            node_flags |= ImGuiTreeNodeFlags_Selected;

        std::string name = entry.path().string();

        auto lastSlash = name.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        name = name.substr(lastSlash, name.size() - lastSlash);

        bool entryIsFile = !std::filesystem::is_directory(entry.path());
        if (entryIsFile)
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        bool node_open = ImGui::TreeNodeEx((void *) (intptr_t) (*count), node_flags, name.c_str());

        if (ImGui::IsItemClicked()) {
            node_clicked = *count;
            any_node_clicked = true;
            if (!entry.is_directory()) {
                mSelectedDirectory = entry.path().parent_path();
            } else if (entry.is_directory()) {
                mSelectedDirectory = entry.path();
            }
        }

        (*count)--;

        if (!entryIsFile) {
            if (node_open) {

                auto clickState = DirectoryTreeViewRecursive(entry.path(), count, selection_mask);

                if (!any_node_clicked) {
                    any_node_clicked = clickState.first;
                    node_clicked = clickState.second;
                }

                ImGui::TreePop();
            } else {
                for (const auto &e: std::filesystem::recursive_directory_iterator(entry.path()))
                    (*count)--;
            }
        }
    }

    return {any_node_clicked, node_clicked};
}

void jle::EditorContentBrowser::ContentHierarchy(std::string directoryPath, const std::string &folderName) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});

    ImGui::Begin("Content Hierarchy");

    if (ImGui::CollapsingHeader(folderName.c_str())) {
        uint32_t count = 0;
        for (const auto &entry: std::filesystem::recursive_directory_iterator(directoryPath))
            count++;

        static int selection_mask = 0;

        auto clickState = DirectoryTreeViewRecursive(directoryPath, &count, &selection_mask);

        if (clickState.first) {
            // Update selection state
            // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
            if (ImGui::GetIO().KeyCtrl)
                selection_mask ^= BIT(clickState.second);          // CTRL+click to toggle
            else //if (!(selection_mask & (1 << clickState.second))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                selection_mask = BIT(clickState.second);           // Click to single-select
        }
    }
    ImGui::End();

    ImGui::PopStyleVar();
}

void jle::EditorContentBrowser::Update(jle::jleGameEngine &ge) {
    if (!isOpened) {
        return;
    }

    //ContentHierarchy(JLE_ENGINE_PATH + "/EditorResources", "Editor");
    ContentHierarchy(GAME_RESOURCES_DIRECTORY, "Game Resources");

    ContentBrowser();

}

void jle::EditorContentBrowser::ContentBrowser() {
    ImGui::Begin(window_name.c_str(), &isOpened);

    static float iconScaleSliderValue = 1.f;

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;
    const ImVec2 fileSize{92 * globalImguiScale * iconScaleSliderValue, 92 * globalImguiScale * iconScaleSliderValue};


    if (mSelectedDirectory != GAME_RESOURCES_DIRECTORY && !mSelectedDirectory.empty()) {
        if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(mBackDirectoryIcon->GetTextureID()),
                               ImVec2{19 * globalImguiScale, 16 * globalImguiScale})) {
            mSelectedDirectory = mSelectedDirectory.parent_path();
        }
        ImGui::SameLine();

    }

    ImGui::Text("%ls", mSelectedDirectory.c_str());

    ImGui::BeginGroup();

    ImGui::BeginChild("Content", ImVec2{0, -ImGui::GetFrameHeightWithSpacing()}, true);

    if (!mSelectedDirectory.empty()) {

        int columns = ImGui::GetWindowContentRegionWidth() / fileSize.x - 1;
        if(columns <= 0)
        {
            columns = 1;
        }

        ImGui::Columns(columns, nullptr, false);
        ImGui::Separator();

        for (auto const &dir_entry: std::filesystem::directory_iterator(mSelectedDirectory)) {
            if (!(dir_entry.is_regular_file() || dir_entry.is_directory())) {
                continue;
            }

            {
                ImGui::BeginGroup();

                if (dir_entry.is_regular_file()) {
                    ImGui::ImageButton(reinterpret_cast<ImTextureID>(mFileIcon->GetTextureID()), fileSize);
                } else if (dir_entry.is_directory()) {
                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(mDirectoryIcon->GetTextureID()), fileSize)) {
                        mSelectedDirectory = dir_entry.path();
                    }
                }

                ImGui::Text("%s", dir_entry.path().filename().string().c_str());

                ImGui::Dummy(ImVec2(0.0f, 4.0f * globalImguiScale));

                ImGui::EndGroup();
            }

            ImGui::NextColumn();
        }

        ImGui::Columns(1);
        ImGui::Separator();
    }

    ImGui::EndChild();

    ImGui::PushItemWidth(100 * globalImguiScale);
    ImGui::SliderFloat("Icon Scale", &iconScaleSliderValue, 0.25f, 2.f);

    ImGui::EndGroup();


    ImGui::End();
}
