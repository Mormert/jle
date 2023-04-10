// Copyright (c) 2023. Johan Lind

#include "jleEditorContentBrowser.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
#include "jleEditor.h"
#include "jleEditorSceneObjectsWindow.h"
#include "jleEditorTextEdit.h"
#include "jleResource.h"

#include "plog/Log.h"

#include "jleGame.h"

#include <fstream>
#include <string>
#include <utility>

jleEditorContentBrowser::jleEditorContentBrowser(const std::string &window_name,
                                                 const std::shared_ptr<jleEditorTextEdit> &editorTextEdit)
    : iEditorImGuiWindow(window_name)
{
    _directoryIcon = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{"ED:/icons/directory.png"});
    _fileIcon = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{"ED:/icons/files.png"});
    _backDirectoryIcon = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{"ED:/icons/back_directory.png"});

    _sceneFileIcon = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{"ED:/icons/scene.png"});

    _imageFileIcon = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{"ED:/icons/image.png"});

    _jsonFileIcon = gCore->resources().loadResourceFromFile<jleTexture>(jlePath{"ED:/icons/json.png"});

    _selectedDirectory = GAME_RESOURCES_DIRECTORY;

    _editorTextEdit = std::move(editorTextEdit);
}

#define BIT(x) (1 << x)

std::pair<bool, uint32_t>
jleEditorContentBrowser::directoryTreeViewRecursive(const std::filesystem::path &path,
                                                    uint32_t *count,
                                                    int *selection_mask)
{
    ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                    ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

    bool any_node_clicked = false;
    uint32_t node_clicked = 0;

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
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

        bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)(*count), node_flags, "%s", name.c_str());

        if (ImGui::IsItemClicked()) {
            node_clicked = *count;
            any_node_clicked = true;
            if (!entry.is_directory()) {
                _selectedDirectory = entry.path().parent_path();
            } else if (entry.is_directory()) {
                _selectedDirectory = entry.path();
            }
        }

        (*count)--;

        if (!entryIsFile) {
            if (node_open) {

                auto clickState = directoryTreeViewRecursive(entry.path(), count, selection_mask);

                if (!any_node_clicked) {
                    any_node_clicked = clickState.first;
                    node_clicked = clickState.second;
                }

                ImGui::TreePop();
            } else {
                for (const auto &e : std::filesystem::recursive_directory_iterator(entry.path()))
                    (*count)--;
            }
        }
    }

    return {any_node_clicked, node_clicked};
}

void
jleEditorContentBrowser::contentHierarchy(std::string directoryPath, const std::string &folderName)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});

    ImGui::Begin("Content Hierarchy");

    if (ImGui::CollapsingHeader(folderName.c_str())) {
        uint32_t count = 0;
        for (const auto &entry : std::filesystem::recursive_directory_iterator(directoryPath))
            count++;

        static int selection_mask = 0;

        auto clickState = directoryTreeViewRecursive(directoryPath, &count, &selection_mask);

        if (clickState.first) {
            // Update selection state
            // (process outside of tree loop to avoid visual inconsistencies
            // during the clicking frame)
            if (ImGui::GetIO().KeyCtrl)
                selection_mask ^= BIT(clickState.second); // CTRL+click to toggle
            else                                          // if (!(selection_mask & (1 << clickState.second))) //
                                                          // Depending on selection behavior you want, may want to
                // preserve selection when clicking on item that is part of the
                // selection
                selection_mask = BIT(clickState.second); // Click to single-select
        }
    }
    ImGui::End();

    ImGui::PopStyleVar();
}

void
jleEditorContentBrowser::update(jleGameEngine &ge)
{
    if (!isOpened) {
        return;
    }

    // contentHierarchy(JLE_ENGINE_PATH + "/EditorResources", "Editor");
    contentHierarchy(GAME_RESOURCES_DIRECTORY, "Game Resources");

    contentBrowser();
}

void
jleEditorContentBrowser::contentBrowser()
{
    ImGui::Begin(window_name.c_str(), &isOpened);

    static float iconScaleSliderValue = 1.f;

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;
    const ImVec2 fileSize{92 * globalImguiScale * iconScaleSliderValue, 92 * globalImguiScale * iconScaleSliderValue};

    if (_selectedDirectory != GAME_RESOURCES_DIRECTORY && !_selectedDirectory.empty()) {
        if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(_backDirectoryIcon->id()),
                               ImVec2{19 * globalImguiScale, 16 * globalImguiScale})) {
            _selectedDirectory = _selectedDirectory.parent_path();
        }
        ImGui::SameLine();
    }

    {
        auto text = _selectedDirectory.wstring();
        ImGui::Text("%.*ls", static_cast<int>(text.size()), text.data());
    }

    ImGui::BeginGroup();

    ImGui::BeginChild("Content", ImVec2{0, -ImGui::GetFrameHeightWithSpacing()}, true);

    if (!_selectedDirectory.empty()) {

        int columns = ImGui::GetWindowContentRegionWidth() / fileSize.x - 1;
        if (columns <= 0) {
            columns = 1;
        }

        ImGui::Columns(columns, nullptr, false);
        ImGui::Separator();

        int buttonID = 1;
        bool isFileSelected = false;

        for (auto const &dir_entry : std::filesystem::directory_iterator(_selectedDirectory)) {
            if (!(dir_entry.is_regular_file() || dir_entry.is_directory())) {
                continue;
            }

            {
                ImGui::BeginGroup();

                if (dir_entry.is_regular_file()) {
                    ImGui::PushID(buttonID++);

                    std::shared_ptr<jleTexture> iconTexture;
                    if (dir_entry.path().extension() == ".scn") {
                        iconTexture = _sceneFileIcon;
                    } else if (dir_entry.path().extension() == ".png") {
                        iconTexture = _imageFileIcon;
                    } else if (dir_entry.path().extension() == ".json") {
                        iconTexture = _jsonFileIcon;
                    } else {
                        iconTexture = _fileIcon;
                    }

                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(iconTexture->id()), fileSize)) {
                        _fileSelected = dir_entry;
                        isFileSelected = true;
                    }
                    ImGui::PopID();
                } else if (dir_entry.is_directory()) {
                    ImGui::PushID(buttonID++);
                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(_directoryIcon->id()), fileSize)) {
                        _selectedDirectory = dir_entry.path();
                    }
                    ImGui::PopID();
                }

                ImGui::Text("%s", dir_entry.path().filename().string().c_str());

                ImGui::Dummy(ImVec2(0.0f, 4.0f * globalImguiScale));

                ImGui::EndGroup();
            }

            ImGui::NextColumn();
        }

        if (isFileSelected) {
            ImGui::OpenPopup("selected_file_popup");
        }

        if (!_fileSelected.empty() && ImGui::BeginPopup("selected_file_popup")) {

            selectedFilePopup(_fileSelected);
            ImGui::EndPopup();
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

void
jleEditorContentBrowser::selectedFilePopup(std::filesystem::path &file)
{

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;
    ImVec2 size{100 * globalImguiScale, 25 * globalImguiScale};

    const auto fileExtension = file.extension();

    ImGui::Text("%s", file.filename().string().c_str());

    const auto popupWidth = ImGui::GetWindowContentRegionWidth();
    if (popupWidth > size.x) {
        size.x = popupWidth;
    }

    if (fileExtension == ".scn") {
        selectedFilePopupScene(file);
    }

    if (fileExtension == ".jobj") {
        selectedFilePopupObjectTemplate(file);
    }

    openAsText(file);

    { // Delete File
        static bool opened = false;
        if (ImGui::Button("Delete", size)) {
            opened = true;
            ImGui::OpenPopup("Confirm File Deletion");
        }

        if (ImGui::BeginPopupModal("Confirm File Deletion", &opened, 0)) {
            if (ImGui::Button("Delete")) {
                try {
                    std::filesystem::remove(file);
                    LOGV << "Deleted file: " << file.string();
                } catch (std::exception e) {
                    LOGE << "Could not delete file: " << file.string();
                }
                file = std::filesystem::path();
                opened = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                opened = false;
            }
            ImGui::EndPopup();
        }
    }

    { // Rename File
        static bool opened = false;
        static std::string buf;
        if (ImGui::Button("Rename", size)) {
            opened = true;
            buf = std::string{file.filename().string()};
            ImGui::OpenPopup("Rename File");
        }

        if (ImGui::BeginPopupModal("Rename File", &opened, 0)) {
            ImGui::InputText("New Name", &buf);
            if (ImGui::Button("Confirm")) {
                std::filesystem::path newFile = file;
                newFile.remove_filename();
                newFile.append(std::string{buf});
                if (!exists(newFile)) {
                    std::filesystem::rename(file, newFile);
                    LOGV << "Renamed file: " << file.string() << " to: " << newFile.string();
                } else {
                    LOGE << "File with name: " << newFile.filename().string() << " already exists.";
                }

                file = std::filesystem::path();

                opened = false;
            }
            ImGui::EndPopup();
        }
    }

    { // Duplicate File
        static bool opened = false;
        static std::string buf;
        if (ImGui::Button("Duplicate", size)) {
            opened = true;
            buf = std::string{file.filename().string() + "2"};
            ImGui::OpenPopup("Duplicate File");
        }

        if (ImGui::BeginPopupModal("Duplicate File", &opened, 0)) {
            ImGui::InputText("New Name", &buf);
            if (ImGui::Button("Confirm")) {
                std::filesystem::path newFile = file;
                newFile.remove_filename();
                newFile.append(std::string{buf});
                try {
                    std::filesystem::copy(file, newFile);
                    LOGV << "Copied file: " << file.string() << " to: " << newFile.string();
                } catch (std::exception e) {
                    LOGE << "File with name: " << newFile.filename().string() << " already exists.";
                }
                file = std::filesystem::path();
                opened = false;
            }
            ImGui::EndPopup();
        }
    }
}

void
jleEditorContentBrowser::selectedFilePopupScene(std::filesystem::path &file)
{

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;
    const ImVec2 size{100 * globalImguiScale, 25 * globalImguiScale};

    if (!gEngine->isGameKilled()) {
        if (ImGui::Button("Load Scene (Game)", size)) {

            std::string sceneName = file.filename().string();
            int dot = sceneName.rfind(file.extension().string());
            if (dot != std::string::npos) {
                sceneName.resize(dot);
            }

            auto &game = ((jleGameEngine *)gCore)->gameRef();
            game.loadScene(jlePath{file.string(), false});
        }
    } else {
        if (ImGui::Button("Load Scene (Editor)", size)) {
            std::string sceneName = file.filename().string();
            int dot = sceneName.rfind(file.extension().string());
            if (dot != std::string::npos) {
                sceneName.resize(dot);
            }

            gEditor->loadScene(jlePath{file.string(), false});
        }
    }
}

void
jleEditorContentBrowser::selectedFilePopupObjectTemplate(std::filesystem::path &file)
{
    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;
    const ImVec2 size{100 * globalImguiScale, 25 * globalImguiScale};

    if (ImGui::Button("Add Template", size)) {

        std::string objectName = file.filename().string();
        int dot = objectName.rfind(file.extension().string());
        if (dot != std::string::npos) {
            objectName.resize(dot);
        }

        if (auto &&scene = jleEditorSceneObjectsWindow::GetSelectedScene().lock()) {
            try {
                std::shared_ptr<jleObject> object;

                std::ifstream i(file);
                cereal::JSONInputArchive iarchive{i};
                iarchive(object);
                object->__templatePath = jlePath{file.string(), false};
                scene->spawnObject(object);
            } catch (std::exception &e) {
                LOGE << "Failed to load object template: " << e.what();
            }
        }
    }
}

void
jleEditorContentBrowser::openAsText(std::filesystem::path &file)
{
    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;
    const ImVec2 size{100 * globalImguiScale, 25 * globalImguiScale};

    if (ImGui::Button("Open As Text", size)) {
        _editorTextEdit->open(jlePath{file.string(), false});
    }
}
