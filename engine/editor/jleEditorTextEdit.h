// Copyright (c) 2023. Johan Lind

#ifndef JLEEDITORTEXTEDIT_H
#define JLEEDITORTEXTEDIT_H

#include "3rdparty/ImGuiColorTextEdit/TextEditor.h"
#include "jleEditorImGuiWindowInterface.h"
#include <jleResourceRef.h>

#include <unordered_map>
#include <memory>

class jleEditorTextEdit : public iEditorImGuiWindow
{
public:
    explicit jleEditorTextEdit(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    void open(const jleRelativePath &path);

private:
    std::unordered_map<jleRelativePath, std::unique_ptr<TextEditor>> _textEditorsMap;
};

#endif // JLEEDITORTEXTEDIT_H
