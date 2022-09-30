// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleEditorImGuiWindowInterface.h"

#include <filesystem>

class jleEditorContentBrowser : public iEditorImGuiWindow {
public:
    explicit jleEditorContentBrowser(const std::string& window_name);

    void Update(jleGameEngine& ge) override;

private:
    std::shared_ptr<jleTextureInterface> _fileIcon;
    std::shared_ptr<jleTextureInterface> _directoryIcon;
    std::shared_ptr<jleTextureInterface> _backDirectoryIcon;
    std::shared_ptr<jleTextureInterface> _sceneFileIcon;
    std::shared_ptr<jleTextureInterface> _imageFileIcon;
    std::shared_ptr<jleTextureInterface> _jsonFileIcon;

    // Selected directory to show files in
    std::filesystem::path _selectedDirectory;

    // File selected for File Popup
    std::filesystem::path _fileSelected;

    std::pair<bool, uint32_t> DirectoryTreeViewRecursive(
        const std::filesystem::path& path,
        uint32_t *count,
        int *selection_mask);

    void ContentHierarchy(std::string directoryPath,
                          const std::string& folderName);

    void ContentBrowser();

    void SelectedFilePopup(std::filesystem::path& file);

    void SelectedFilePopupScene(std::filesystem::path& file);

    void SelectedFilePopupObjectTemplate(std::filesystem::path& file);
};
