// Copyright (c) 2022. Johan Lind

// ConsoleEditorWindow is based on examples found in ImGui demo source code

#pragma once

#include "jleEditorImGuiWindowInterface.h"

#include "ImGui/imgui.h"

#include "plog/Appenders/IAppender.h"

class jleConsoleEditorWindow : public iEditorImGuiWindow,
                               public plog::IAppender {
public:
    jleConsoleEditorWindow(const std::string& window_name);

    ~jleConsoleEditorWindow();

    static int stricmp(const char *s1, const char *s2);

    static int strnicmp(const char *s1, const char *s2, int n);

    static char *strdup(const char *s);

    static void strtrim(char *s);

    void clearLog();

    void addLog(const char *fmt, ...) IM_FMTARGS(2);

    void execCommand(const char *command_line);

    virtual void Update(jleGameEngine& ge) override;

    static int textEditCallbackStub(ImGuiInputTextCallbackData *data);

    int textEditCallback(ImGuiInputTextCallbackData *data);

    // Inherited from plog::IAppender
    void write(const plog::Record& record) override;

private:
    static const unsigned int InputBufSize = 256;
    char InputBuf[InputBufSize];
    ImVector<char *> Items;
    ImVector<const char *> Commands;
    ImVector<char *> History;
    int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter Filter;
    bool AutoScroll;
    bool ScrollToBottom;
};
