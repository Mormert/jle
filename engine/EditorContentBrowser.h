#pragma once

#include "iEditorImGuiWindow.h"

namespace jle {
    class EditorContentBrowser : public iEditorImGuiWindow {
    public:
        explicit EditorContentBrowser(const std::string &window_name);

        virtual void Update(jleGameEngine &ge) override;

    private:
        std::shared_ptr<iTexture> mFileIcon;
        std::shared_ptr<iTexture> mDirectoryIcon;
    };
}



