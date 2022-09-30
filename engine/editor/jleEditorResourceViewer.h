// Copyright (c) 2022. Johan Lind

#ifndef EDITORRESOURCEVIEWER_H
#define EDITORRESOURCEVIEWER_H

#include "jleEditorImGuiWindowInterface.h"

class jleEditorResourceViewer : public iEditorImGuiWindow {
public:
    explicit jleEditorResourceViewer(const std::string& window_name);

    void Update(jleGameEngine& ge) override;
};

#endif // EDITORRESOURCEVIEWER_H
