// Copyright (c) 2022. Johan Lind

#ifndef EDITORRESOURCEVIEWER_H
#define EDITORRESOURCEVIEWER_H

#include "iEditorImGuiWindow.h"

namespace jle {
    class EditorResourceViewer : public iEditorImGuiWindow {
    public:
        explicit EditorResourceViewer(const std::string &window_name);

        void Update(jleGameEngine &ge) override;

    };
}


#endif //EDITORRESOURCEVIEWER_H
