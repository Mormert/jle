#pragma once

#include "iEditorImGuiWindow.h"

#include <filesystem>

namespace jle {
    class EditorContentBrowser : public iEditorImGuiWindow {
    public:
        explicit EditorContentBrowser(const std::string &window_name);

        virtual void Update(jleGameEngine &ge) override;

    private:
        std::shared_ptr<iTexture> mFileIcon;
        std::shared_ptr<iTexture> mDirectoryIcon;
        std::shared_ptr<iTexture> mBackDirectoryIcon;

        std::filesystem::path mSelectedDirectory;

        std::pair<bool, uint32_t> DirectoryTreeViewRecursive(const std::filesystem::path& path, uint32_t* count, int* selection_mask);

        void ContentHierarchy(std::string directoryPath, const std::string& folderName);

        void ContentBrowser();
    };
}



