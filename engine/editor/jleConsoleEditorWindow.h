// Copyright (c) 2023. Johan Lind

// ConsoleEditorWindow is based on examples found in ImGui demo source code

#pragma once

#ifdef BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"

#include "ImGui/imgui.h"

#include "plog/Appenders/IAppender.h"

class jleConsoleEditorWindow : public jleEditorWindowInterface, public plog::IAppender {
public:
    explicit jleConsoleEditorWindow(const std::string &window_name);

    ~jleConsoleEditorWindow() override;

    static int stricmp(const char *s1, const char *s2);

    static int strnicmp(const char *s1, const char *s2, int n);

    static char *strdup(const char *s);

    static void strtrim(char *s);

    void clearLog();

    void addLog(const char *fmt, ...) IM_FMTARGS(2);

    void execCommand(const char *command_line);

    virtual void update(jleGameEngine &ge) override;

    static int textEditCallbackStub(ImGuiInputTextCallbackData *data);

    int textEditCallback(ImGuiInputTextCallbackData *data);

    void write(const plog::Record &record) override;

private:
    static const unsigned int InputBufSize = 256;
    char InputBuf[InputBufSize];
    ImVector<char *> Items; // TODO: Do somoething more c++ ish
    ImVector<const char *> Commands;
    ImVector<char *> History;
    int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter Filter;
    bool AutoScroll;
    bool ScrollToBottom;
};

#endif // BUILD_EDITOR
