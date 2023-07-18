// Copyright (c) 2023. Johan Lind

#include "jleEditorTextEdit.h"
jleEditorTextEdit::jleEditorTextEdit(const std::string &window_name) : iEditorImGuiWindow(window_name)
{
    ImGuiIO &io = ImGui::GetIO();
    std::string path = jlePath{"ED:/fonts/Cascadia.ttf"}.getRealPath();
    font = io.Fonts->AddFontFromFileTTF(path.c_str(), 20.0f);
}

void
jleEditorTextEdit::update(jleGameEngine &ge)
{
    std::vector<jlePath> toBeClosed;

    for (auto &[path, textEditorPtr] : _textEditorsMap) {
        auto &textEditor = *textEditorPtr.get();
        auto tabName = "File: " + path.getVirtualPath();
        bool shouldBeOpened = true;
        ImGui::SetNextWindowSize(ImVec2(500, 700), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(tabName.c_str(), &shouldBeOpened, ImGuiWindowFlags_MenuBar)) {
            auto cpos = textEditor.GetCursorPosition();
            if (!shouldBeOpened) {
                toBeClosed.push_back(path);
            }
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Save", "Ctrl-S")) {
                        std::ofstream stream{path.getRealPath()};
                        stream << textEditor.GetText();
                    }
                    if (ImGui::MenuItem("Reload", "Ctrl-R")) {
                        std::ifstream i(path.getRealPath());
                        if (i.good()) {
                            std::stringstream buffer;
                            buffer << i.rdbuf();
                            _textEditorsMap[path].get()->SetText(buffer.str());
                        } else {
                            LOGE << "Failed to open file with absolute path:" << path.getRealPath();
                        }
                    }
                    if (ImGui::MenuItem("Close")) {
                        toBeClosed.push_back(path);
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit")) {
                    bool ro = textEditor.IsReadOnly();
                    if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                        textEditor.SetReadOnly(ro);
                    ImGui::Separator();

                    if (ImGui::MenuItem("Undo", "Ctrl-Z", nullptr, !ro && textEditor.CanUndo()))
                        textEditor.Undo();
                    if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && textEditor.CanRedo()))
                        textEditor.Redo();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, textEditor.HasSelection()))
                        textEditor.Copy();
                    if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && textEditor.HasSelection()))
                        textEditor.Cut();
                    if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && textEditor.HasSelection()))
                        textEditor.Delete();
                    if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                        textEditor.Paste();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Select all", "Ctrl-A", nullptr))
                        textEditor.SetSelection(TextEditor::Coordinates(),
                                                TextEditor::Coordinates(textEditor.GetTotalLines(), 0));

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View")) {
                    if (ImGui::MenuItem("Dark palette"))
                        textEditor.SetPalette(TextEditor::GetDarkPalette());
                    if (ImGui::MenuItem("Light palette"))
                        textEditor.SetPalette(TextEditor::GetLightPalette());
                    if (ImGui::MenuItem("Retro blue palette"))
                        textEditor.SetPalette(TextEditor::GetRetroBluePalette());
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s (%s)",
                        cpos.mLine + 1,
                        cpos.mColumn + 1,
                        textEditor.GetTotalLines(),
                        textEditor.IsOverwrite() ? "Ovr" : "Ins",
                        textEditor.CanUndo() ? "*" : " ",
                        textEditor.GetLanguageDefinition().mName.c_str(),
                        path.getRealPath().c_str(),
                        path.getVirtualPath().c_str());

            ImGui::PushFont(font);
            textEditor.Render(tabName.c_str(), ImVec2(), true);
            if(textEditor.DocumentSavedThisFrame())
            {
                std::ofstream stream{path.getRealPath()};
                stream << textEditor.GetText();
            }
            ImGui::PopFont();
        }
        ImGui::End();
    }

    for (auto &&shouldClose : toBeClosed) {
        auto it = _textEditorsMap.find(shouldClose);
        _textEditorsMap.erase(it);
    }
}

void
jleEditorTextEdit::open(const jlePath &path)
{
    if (_textEditorsMap.find(path) != _textEditorsMap.end()) {
        return;
    }

    auto e = std::make_unique<TextEditor>();
    e->SetShowWhitespaces(false);

    const auto ends_with = [](std::string const &value, std::string const &ending) -> bool {
        if (ending.size() > value.size())
            return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    };

    auto pathStr = path.getVirtualPath();
    if (ends_with(pathStr, ".frag") || ends_with(pathStr, ".vert")) {
        e->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
    } else if (ends_with(pathStr, ".cpp") || ends_with(pathStr, ".h") || ends_with(pathStr, ".c")) {
        e->SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    } else if (ends_with(pathStr, ".lua")) {
        e->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
    } else {
        e->SetLanguageDefinition(TextEditor::LanguageDefinition::RegularText());
    }

    std::ifstream i(path.getRealPath());
    if (i.good()) {
        std::stringstream buffer;
        buffer << i.rdbuf();
        e->SetText(buffer.str());
    } else {
        LOGE << "Failed to open file with real path: " << path.getRealPath();
    }

    _textEditorsMap.insert(std::make_pair(path, std::move(e)));
}

void
jleEditorTextEdit::reloadIfOpened(const jlePath &path)
{
    auto doc = _textEditorsMap.find(path);
    if(doc == _textEditorsMap.end())
    {
        return;
    }

    std::ifstream i(path.getRealPath());
    if (i.good()) {
        std::stringstream buffer;
        buffer << i.rdbuf();
        doc->second->SetText(buffer.str());
    } else {
        LOGE << "Failed to open file with real path: " << path.getRealPath();
    }

}
