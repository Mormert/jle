// ConsoleEditorWindow is based on examples found in ImGui demo source code

#pragma once

#include "iEditorImGuiWindow.h"

#include "imgui.h" // uses vcpkg

namespace jle
{
	class ConsoleEditorWindow : public iEditorImGuiWindow
	{
    public:
        ConsoleEditorWindow();
        ~ConsoleEditorWindow();

        static int Stricmp(const char* s1, const char* s2);
        static int Strnicmp(const char* s1, const char* s2, int n);
        static char* Strdup(const char* s);
        static void Strtrim(char* s);

        void ClearLog();
        void AddLog(const char* fmt, ...) IM_FMTARGS(2);
        void ExecCommand(const char* command_line);

        virtual void Update(jleGameEngine& ge) override;

        static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
        int TextEditCallback(ImGuiInputTextCallbackData* data);

	private:
        static const unsigned int InputBufSize = 256;
        char                  InputBuf[InputBufSize];
        ImVector<char*>       Items;
        ImVector<const char*> Commands;
        ImVector<char*>       History;
        int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
        ImGuiTextFilter       Filter;
        bool                  AutoScroll;
        bool                  ScrollToBottom;
	};
}