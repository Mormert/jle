// Copyright (c) 2023. Johan Lind

#include "jleEditor.h"
#include <jleFramebufferMultisample.h>

#include "ImGui/ImGuizmo.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "jlePathDefines.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include "glad/glad.h"

#endif

#include "GLFW/glfw3.h"
#include "Remotery/Remotery.h"
#include "editor/jleConsoleEditorWindow.h"
#include "editor/jleEditorContentBrowser.h"
#include "editor/jleEditorGameControllerWidget.h"
#include "editor/jleEditorProfilerWindow.h"
#include "editor/jleEditorResourceViewer.h"
#include "editor/jleEditorSceneObjectsWindow.h"
#include "editor/jleEditorWindowsPanel.h"
#include "jleEditorSettingsWindow.h"
#include "jleEditorTextEdit.h"
#include "jleFramebufferScreen.h"
#include "jleGameEditorWindow.h"
#include "jleQuadRendering.h"
#include "jleSceneEditorWindow.h"
#include "jleWindow.h"
#include "plog/Log.h"

jleEditor::jleEditor()
{
    gEditor = this;
}

void
jleEditor::start()
{

    LOG_INFO << "Starting the editor";

    initImgui();

    constexpr int initialScreenX = 1024;
    constexpr int initialScreenY = 1024;
    mainScreenFramebuffer = std::make_shared<jleFramebufferScreen>(initialScreenX, initialScreenY);

    editorScreenFramebuffer = std::make_shared<jleFramebufferScreen>(initialScreenX, initialScreenY);

    // Note: Important that menu comes first here, since the others are
    // dependent on the menu's dockspace.
    auto menu = std::make_shared<jleEditorWindowsPanel>("Menu");
    addImGuiWindow(menu);

    auto textEditWindow = std::make_shared<jleEditorTextEdit>("Text Editor");
    addImGuiWindow(textEditWindow);

    auto sceneWindow = std::make_shared<jleSceneEditorWindow>("Scene Window", editorScreenFramebuffer);
    addImGuiWindow(sceneWindow);
    menu->addWindow(sceneWindow);

    auto gameWindow = std::make_shared<jleGameEditorWindow>("Game Window");
    addImGuiWindow(gameWindow);
    menu->addWindow(gameWindow);

    auto console = std::make_shared<jleConsoleEditorWindow>("Console Window");
    plog::get<0>()->addAppender(&console->appender());
    addImGuiWindow(console);
    menu->addWindow(console);

    auto settingsWindow = std::make_shared<jleEditorSettingsWindow>("Engine Settings");
    addImGuiWindow(settingsWindow);
    menu->addWindow(settingsWindow);

    // auto gameController =
    // std::make_shared<EditorGameControllerWidget>("Controller");
    // AddImGuiWindow(gameController);
    // menu->addWindow(gameController);

    auto editorSceneObjects = std::make_shared<jleEditorSceneObjectsWindow>("Scene Objects");
    addImGuiWindow(editorSceneObjects);
    menu->addWindow(editorSceneObjects);

    auto contentBrowser = std::make_shared<jleEditorContentBrowser>("Content Browser", textEditWindow);
    addImGuiWindow(contentBrowser);
    menu->addWindow(contentBrowser);

    auto resourceViewer = std::make_shared<jleEditorResourceViewer>("Resource Viewer");
    addImGuiWindow(resourceViewer);
    menu->addWindow(resourceViewer);

    auto profilerWindow = std::make_shared<jleEditorProfilerWindow>("Profiler");
    addImGuiWindow(profilerWindow);
    menu->addWindow(profilerWindow);

    gCore->window().addWindowResizeCallback(
        std::bind(&jleEditor::mainEditorWindowResized, this, std::placeholders::_1, std::placeholders::_2));

    LOG_INFO << "Starting the game in editor mode";

    startGame();
}

void
jleEditor::render()
{
    JLE_SCOPE_PROFILE_GPU(EditorRender);

    if (!gameHalted && game) {
        // Render to game view
        static jleFramebufferMultisample msaa{mainScreenFramebuffer->width(), mainScreenFramebuffer->height(), 4};

        if (mainScreenFramebuffer->width() != msaa.width() || mainScreenFramebuffer->height() != msaa.height()) {
            msaa.resize(mainScreenFramebuffer->width(), mainScreenFramebuffer->height());
        }

        rendering().renderMSAA(*mainScreenFramebuffer, msaa, game->mainCamera);
    }

    if (projectionType == jleCameraProjection::Orthographic) {
        editorCamera.setOrthographicProjection(
            editorScreenFramebuffer->width(), editorScreenFramebuffer->height(), 10000.f, -10000.f);
    } else {
        editorCamera.setPerspectiveProjection(
            45.f, editorScreenFramebuffer->width(), editorScreenFramebuffer->height(), 10000.f, 0.1f);
    }

    static jleFramebufferMultisample msaa{editorScreenFramebuffer->width(), editorScreenFramebuffer->height(), 4};

    if (editorScreenFramebuffer->width() != msaa.width() || editorScreenFramebuffer->height() != msaa.height()) {
        msaa.resize(editorScreenFramebuffer->width(), editorScreenFramebuffer->height());
    }

    // Render to editor scene view
    rendering().renderMSAA(*editorScreenFramebuffer, msaa, editorCamera);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set viewport to cover the entire screen
    glViewport(0, 0, window().width(), window().height());

    {
        JLE_SCOPE_PROFILE_GPU(ImGuiRender);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuizmo::BeginFrame();

        // Update loop for all ImGui windows
        for (auto window : _imGuiWindows) {
            window->update(*this);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        auto &&io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    checkGlErrors();

    rendering().clearBuffersForNextFrame();
}

void
jleEditor::initImgui()
{
    ImGui::DebugCheckVersionAndDataLayout(IMGUI_VERSION,
                                          sizeof(ImGuiIO),
                                          sizeof(ImGuiStyle),
                                          sizeof(ImVec2),
                                          sizeof(ImVec4),
                                          sizeof(ImDrawVert),
                                          sizeof(ImDrawIdx));
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    static const std::string iniFile = GAME_RESOURCES_DIRECTORY + "/imgui.ini";
    io.IniFilename = iniFile.c_str();

    ImGui::Spectrum::StyleColorsSpectrum();

    // up Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window().glfwWindow(), true);

#ifdef BUILD_OPENGLES30
    ImGui_ImplOpenGL3_Init("#version 300 es");
#else
    ImGui_ImplOpenGL3_Init("#version 330 core");
#endif

    ImFontConfig config;
    config.OversampleH = 3;
    config.OversampleV = 2;
    config.GlyphExtraSpacing.x = 1.0f;

    io.Fonts->Clear();
    ImGui::Spectrum::LoadFont();
}

void
jleEditor::imguiTheme()
{
    // cherry colors, 3 intensities
#define HI(v) ImVec4(0.502f, 0.075f, 0.256f, v)
#define MED(v) ImVec4(0.455f, 0.198f, 0.301f, v)
#define LOW(v) ImVec4(0.232f, 0.201f, 0.271f, v)
    // backgrounds (@todo: complete with BG_MED, BG_LOW)
#define BG(v) ImVec4(0.200f, 0.220f, 0.270f, v)
    // text
#define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

    auto &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = TEXT(0.78f);
    style.Colors[ImGuiCol_TextDisabled] = TEXT(0.28f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    // style.Colors[ImGuiCol_ChildWindowBg] = BG(0.58f);
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
    // style.Colors[ImGuiCol_ModalWindowDarkening] = BG(0.73f);

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

void
jleEditor::addImGuiWindow(std::shared_ptr<iEditorImGuiWindow> window)
{
    _imGuiWindows.push_back(window);
}

void
jleEditor::mainEditorWindowResized(int w, int h)
{
    auto &&io = ImGui::GetIO();
    io.FontGlobalScale = w / 1920.f;
}
void
jleEditor::checkGlErrors()
{
    // Goes through all the errors that OpenGL have queued up
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            error = "UNKNOWN_ERROR";
            break;
        }
        LOGE << "Detected OpenGL error somewhere of type: " << error;
    }
}

std::vector<std::shared_ptr<jleScene>> &
jleEditor::getEditorScenes()
{
    return _editorScenes;
}

void
jleEditor::updateEditorLoadedScenes(float dt)
{
    JLE_SCOPE_PROFILE_CPU(jleEditor_updateEditorLoadedScenes)
    for (int i = _editorScenes.size() - 1; i >= 0; i--) {
        if (_editorScenes[i]->bPendingSceneDestruction) {
            _editorScenes.erase(_editorScenes.begin() + i);
            continue;
        }

        _editorScenes[i]->updateSceneEditor();
        _editorScenes[i]->processNewSceneObjects();
        _editorScenes[i]->updateSceneObejctsEditor(dt);
    }
}

void
jleEditor::update(float dt)
{
    jleGameEngine::update(dt);
    if(isGameKilled())
    {
        updateEditorLoadedScenes(dt);
    }
}
