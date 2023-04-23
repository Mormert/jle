// Copyright (c) 2023. Johan Lind

#ifndef JLE_EDITORRESOURCEEDIT_H
#define JLE_EDITORRESOURCEEDIT_H

#include "jleEditorImGuiWindowInterface.h"
#include <jleResourceRef.h>

#include <memory>
#include <unordered_map>

class jleEditorResourceEdit : public iEditorImGuiWindow
{
public:
    explicit jleEditorResourceEdit(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    bool tryOpen(const jlePath &path);

private:

    std::shared_ptr<jleResourceInterface> resource{};

};

#endif // JLE_EDITORRESOURCEEDIT_H
