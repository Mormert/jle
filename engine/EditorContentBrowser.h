// Copyright (c) 2022. Johan Lind

#pragma once

#include "iEditorImGuiWindow.h"

#include <filesystem>

namespace jle {
    class EditorContentBrowser : public iEditorImGuiWindow {
    public:
        explicit EditorContentBrowser(const std::string &window_name);

        void Update(jleGameEngine &ge) override;

    private:
        std::shared_ptr<iTexture> mFileIcon;
        std::shared_ptr<iTexture> mDirectoryIcon;
        std::shared_ptr<iTexture> mBackDirectoryIcon;
        std::shared_ptr<iTexture> mSceneFileIcon;
        std::shared_ptr<iTexture> mImageFileIcon;
        std::shared_ptr<iTexture> mJsonFileIcon;

        // Selected directory to show files in
        std::filesystem::path mSelectedDirectory;

        // File selected for File Popup
        std::filesystem::path mFileSelected;

        std::pair<bool, uint32_t>
        DirectoryTreeViewRecursive(const std::filesystem::path &path, uint32_t *count, int *selection_mask);

        void ContentHierarchy(std::string directoryPath, const std::string &folderName);

        void ContentBrowser();

        void SelectedFilePopup(std::filesystem::path &file);

        void SelectedFilePopupScene(std::filesystem::path &file);

        void SelectedFilePopupObjectTemplate(std::filesystem::path &file);
    };
}
