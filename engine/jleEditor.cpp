#include "jleEditor.h"

#include "imgui.h" // uses vcpkg
#include "3rdparty/imgui_impl_glfw.h"
#include "3rdparty/imgui_impl_opengl3.h"

#include "GLState.h"
#include <glad/glad.h>

#include "Window_GLFW_OpenGL.h"
#include <GLFW/glfw3.h>

#include <iostream>

#include <plog/Log.h>

#include "GameEditorWindow.h"
#include "ConsoleEditorWindow.h"

namespace jle
{
	jleEditor::jleEditor(std::shared_ptr<GameSettings> gs) : jleGameEngine{ gs }
	{
	}

	void jleEditor::StartEditor()
	{
	}

	void jleEditor::Start()
	{

        InitImgui();

		auto dims = GetFramebufferDimensions(core_settings->windowSettings.windowWidth, core_settings->windowSettings.windowHeight);
		framebuffer_main = renderingFactory->CreateFramebuffer(dims.first, dims.second);

        AddImGuiWindow(std::make_shared<GameEditorWindow>());

        auto console = std::make_shared<ConsoleEditorWindow>();
        plog::get<0>()->addAppender(&*console);
        AddImGuiWindow(console);

        LOG_INFO << "Starting the game engine in editor mode";

		game->Start();

	}

	void jleEditor::Render()
	{
		((iRenderingInternalAPI*)rendering.get())->Render(*framebuffer_main.get());

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set viewport to cover the entire screen
		glViewport(0, 0, window->GetWindowWidth(), window->GetWindowHeight());

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

        // Update loop for all ImGui windows
        for (auto window : ImGuiWindows)
        {
            window->Update(*this);
        }


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

    void jleEditor::InitImgui()
    {
        ImGui::DebugCheckVersionAndDataLayout(IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx));
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(std::static_pointer_cast<Window_GLFW_OpenGL>(window)->GetGLFWWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        // Setup Dear ImGui style
        SetImguiTheme();
    }

    void jleEditor::SetImguiTheme()
	{
        // cherry colors, 3 intensities
        #define HI(v)   ImVec4(0.502f, 0.075f, 0.256f, v)
        #define MED(v)  ImVec4(0.455f, 0.198f, 0.301f, v)
        #define LOW(v)  ImVec4(0.232f, 0.201f, 0.271f, v)
        // backgrounds (@todo: complete with BG_MED, BG_LOW)
        #define BG(v)   ImVec4(0.200f, 0.220f, 0.270f, v)
        // text
        #define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

        auto& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Text] = TEXT(0.78f);
        style.Colors[ImGuiCol_TextDisabled] = TEXT(0.28f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
        style.Colors[ImGuiCol_ChildWindowBg] = BG(0.58f);
        style.Colors[ImGuiCol_PopupBg] = BG(0.9f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = BG(1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
        style.Colors[ImGuiCol_FrameBgActive] = MED(1.00f);
        style.Colors[ImGuiCol_TitleBg] = LOW(1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = HI(1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
        style.Colors[ImGuiCol_MenuBarBg] = BG(0.47f);
        style.Colors[ImGuiCol_ScrollbarBg] = BG(1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        style.Colors[ImGuiCol_ButtonHovered] = MED(0.86f);
        style.Colors[ImGuiCol_ButtonActive] = MED(1.00f);
        style.Colors[ImGuiCol_Header] = MED(0.76f);
        style.Colors[ImGuiCol_HeaderHovered] = MED(0.86f);
        style.Colors[ImGuiCol_HeaderActive] = HI(1.00f);
        // style.Colors[ImGuiCol_Column] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
        // style.Colors[ImGuiCol_ColumnHovered] = MED(0.78f);
        // style.Colors[ImGuiCol_ColumnActive] = MED(1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
        style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
        style.Colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
        style.Colors[ImGuiCol_PlotLines] = TEXT(0.63f);
        style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = TEXT(0.63f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
        // [...]
        style.Colors[ImGuiCol_ModalWindowDarkening] = BG(0.73f);

        style.WindowPadding = ImVec2(6, 4);
        style.WindowRounding = 0.0f;
        style.FramePadding = ImVec2(5, 2);
        style.FrameRounding = 3.0f;
        style.ItemSpacing = ImVec2(7, 1);
        style.ItemInnerSpacing = ImVec2(1, 1);
        style.TouchExtraPadding = ImVec2(0, 0);
        style.IndentSpacing = 6.0f;
        style.ScrollbarSize = 12.0f;
        style.ScrollbarRounding = 16.0f;
        style.GrabMinSize = 20.0f;
        style.GrabRounding = 2.0f;

        style.WindowTitleAlign.x = 0.50f;

        style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
        style.FrameBorderSize = 0.0f;
        style.WindowBorderSize = 1.0f;
	}

    void jleEditor::AddImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window)
    {
        ImGuiWindows.push_back(window);
    }

}

