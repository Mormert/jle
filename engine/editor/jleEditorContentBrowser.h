// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleEditorImGuiWindowInterface.h"
#include "jleTexture.h"
#include <filesystem>

class jleEditorTextEdit;

class jleEditorContentBrowser : public iEditorImGuiWindow
{
public:
    explicit jleEditorContentBrowser(const std::string &window_name,
                                     const std::shared_ptr<jleEditorTextEdit> &editorTextEdit);

    void update(jleGameEngine &ge) override;

private:
    std::shared_ptr<jleTexture> _fileIcon;
    std::shared_ptr<jleTexture> _directoryIcon;
    std::shared_ptr<jleTexture> _backDirectoryIcon;
    std::shared_ptr<jleTexture> _sceneFileIcon;
    std::shared_ptr<jleTexture> _imageFileIcon;
    std::shared_ptr<jleTexture> _jsonFileIcon;

    // Selected directory to show files in
    std::filesystem::path _selectedDirectory;

    // File selected for File Popup
    std::filesystem::path _fileSelected;

    std::pair<bool, uint32_t> directoryTreeViewRecursive(const std::filesystem::path &path,
                                                         uint32_t *count,
                                                         int *selection_mask);

    void contentHierarchy(std::string directoryPath, const std::string &folderName);

    void contentBrowser();

    void selectedFilePopup(std::filesystem::path &file);

    void selectedFilePopupScene(std::filesystem::path &file);

    void selectedFilePopupObjectTemplate(std::filesystem::path &file);

    void openAsText(std::filesystem::path &file);

    std::shared_ptr<jleEditorTextEdit> _editorTextEdit;
};
