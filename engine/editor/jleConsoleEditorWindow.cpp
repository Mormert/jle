/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleConsoleEditorWindow.h"
#include "jleDynamicLogAppender.h"
#include "jleGameEngine.h"
#include "jleLuaEnvironment.h"

#include <ImGui/imgui.h>
#include <plog/Formatters/FuncMessageFormatter.h>

#include <algorithm>
#include <iostream>

namespace {

std::string recordToString(const plog::Record &record) {

    std::wostringstream woss;
    if(strlen(record.getFile()) > 0)
    {
        woss << PLOG_NSTR("<") << plog::severityToString(record.getSeverity())
             << PLOG_NSTR("> ") << record.getFile() << PLOG_NSTR(": ") << record.getMessage()
             << PLOG_NSTR("\n");
    } else{
        woss << PLOG_NSTR("<") << plog::severityToString(record.getSeverity())
             << PLOG_NSTR("> ") << record.getFunc() << PLOG_NSTR("@")
             << record.getLine() << PLOG_NSTR(":\n") << record.getMessage()
             << PLOG_NSTR("\n");
    }

    auto wstr = woss.str();

    std::string converted_str(wstr.length(), 0);
    std::transform(wstr.begin(),
                   wstr.end(),
                   converted_str.begin(),
                   [](wchar_t c) { return (char)c; });

    return converted_str;
}

// This is done because consolewindow might be unloaded before all logging is
// finished
struct ConsoleAppender : public plog::IAppender {
    jleConsoleEditorWindow *window = nullptr;
    void write(const plog::Record &record) override {
        if (window) {
            window->write(record);
        }
        else {
            std::cerr << recordToString(record) << "\n";
        }
    }
};

} // namespace


jleConsoleEditorWindow::jleConsoleEditorWindow(const std::string &window_name)
    : jleEditorWindowInterface{window_name} {
    clearLog();
    memset(InputBuf, 0, sizeof(InputBuf));
    HistoryPos = -1;

    AutoScroll = true;
    ScrollToBottom = false;

    dynamicAppender().addAppender(this);
}

jleConsoleEditorWindow::~jleConsoleEditorWindow() {
    clearLog();
    for (int i = 0; i < History.Size; i++)
        free(History[i]);

    dynamicAppender().removeAppender(this);
}

int jleConsoleEditorWindow::stricmp(const char *s1, const char *s2) {
    int d;
    while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
        s1++;
        s2++;
    }
    return d;
}

int jleConsoleEditorWindow::strnicmp(const char *s1, const char *s2, int n) {
    int d = 0;
    while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
        s1++;
        s2++;
        n--;
    }
    return d;
}

char *jleConsoleEditorWindow::strdup(const char *s) {
    IM_ASSERT(s);
    size_t len = strlen(s) + 1;
    void *buf = malloc(len);
    IM_ASSERT(buf);
    return (char *)memcpy(buf, (const void *)s, len);
}

void jleConsoleEditorWindow::strtrim(char *s) {
    char *str_end = s + strlen(s);
    while (str_end > s && str_end[-1] == ' ')
        str_end--;
    *str_end = 0;
}

void jleConsoleEditorWindow::clearLog() {
    for (int i = 0; i < Items.Size; i++)
        free(Items[i]);
    Items.clear();
}

void jleConsoleEditorWindow::addLog(const char *fmt, ...) {
    // FIXME-OPT
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    Items.push_back(strdup(buf));
}

void jleConsoleEditorWindow::execCommand(const char *command_line, jleLuaEnvironment& luaEnvironment) {
    addLog("# %s\n", command_line);

    // Insert into history. First find match and delete it so it can be pushed
    // to the back. This isn't trying to be smart or optimal.
    HistoryPos = -1;
    for (int i = History.Size - 1; i >= 0; i--)
        if (stricmp(History[i], command_line) == 0) {
            free(History[i]);
            History.erase(History.begin() + i);
            break;
        }
    History.push_back(strdup(command_line));

    luaEnvironment.executeScript(command_line);

    // On command input, we scroll to bottom even if AutoScroll==false
    ScrollToBottom = true;
}

void jleConsoleEditorWindow::renderUI(jleEngineModulesContext &ctx, jleLuaEnvironment& luaEnvironment) {
    if (!isOpened) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(window_name.c_str(), &isOpened)) {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Clear")) {
        clearLog();
    }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::Button("Copy");
    // static float t = 0.0f; if (ImGui::time() - t > 0.02f) { t =
    // ImGui::time(); addLog("Spam %f", t); }

    ImGui::SameLine();

    // Options menu
    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }

    // Options, Filter
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve =
        ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion",
                      ImVec2(0, -footer_height_to_reserve),
                      false,
                      ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::Selectable("Clear"))
            clearLog();
        ImGui::EndPopup();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(4, 1)); // Tighten spacing
    if (copy_to_clipboard)
        ImGui::LogToClipboard();
    for (int i = 0; i < Items.Size; i++) {
        const char *item = Items[i];
        if (!Filter.PassFilter(item))
            continue;

        ImVec4 color;
        bool has_color = false;
        if (strstr(item, "<ERROR>") || strstr(item, "<FATAL>")) {
            color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
            has_color = true;
        }
        else if (strncmp(item, "<WARNING>", 2) == 0) {
            color = ImVec4(0.8f, 0.8f, 0.6f, 1.0f);
            has_color = true;
        }else if (strncmp(item, "<INFO>", 2) == 0) {
            color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
            has_color = true;
        }
        else if (strncmp(item, "<VERB>", 2) == 0) {
            color = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
            has_color = true;
        }
        else if (strncmp(item, "#", 1) == 0) {
            color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
            has_color = true;
        }
        if (has_color)
            ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(item);
        if (has_color)
            ImGui::PopStyleColor();
    }
    if (copy_to_clipboard)
        ImGui::LogFinish();

    if (ScrollToBottom ||
        (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);
    ScrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags =
        ImGuiInputTextFlags_EnterReturnsTrue |
        ImGuiInputTextFlags_CallbackCompletion |
        ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Execute Lua",
                         InputBuf,
                         IM_ARRAYSIZE(InputBuf),
                         input_text_flags,
                         &textEditCallbackStub,
                         (void *)this)) {
        char *s = InputBuf;
        strtrim(s);
        if (s[0])
            execCommand(s, luaEnvironment);
        // strcpy(s, "");
        std::fill(s, s + InputBufSize, 0);
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

int jleConsoleEditorWindow::textEditCallbackStub(
    ImGuiInputTextCallbackData *data) {
    jleConsoleEditorWindow *console = (jleConsoleEditorWindow *)data->UserData;
    return console->textEditCallback(data);
}

int jleConsoleEditorWindow::textEditCallback(ImGuiInputTextCallbackData *data) {
    return 0;
}

void jleConsoleEditorWindow::write(const plog::Record &record) {
    const auto str = recordToString(record);
    addLog("%s", str.c_str());
}
