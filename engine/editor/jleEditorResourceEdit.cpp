// Copyright (c) 2023. Johan Lind

#include "jleEditorResourceEdit.h"
#include "jleCore.h"
#include "jleResource.h"

jleEditorResourceEdit::jleEditorResourceEdit(const std::string &window_name) : iEditorImGuiWindow(window_name) {}

void
jleEditorResourceEdit::update(jleGameEngine &ge)
{
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    cereal::jleImGuiCerealArchiveInternal ar1;
    ar1(resource);

    ImGui::Text("helo");

    ImGui::End();
}


bool
jleEditorResourceEdit::tryOpen(const jlePath &path)
{
    try {
        resource = gCore->resources().loadSerializedResourceFromFile(path);

    } catch (std::exception &e) {
        LOGV << "Failed to open resource with path: " << path.getVirtualPath() << ", error: " << e.what();
        return false;
    }

    return true;
}
