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

#include "jleEditor.h"

#include "core/jlePath.h"
#include "core/jlePathDefines.h"
#include "core/jleResourceRef.h"

#include "jleConsoleEditorWindow.h"
#include "jleEditorBuild.h"
#include "jleEditorContentBrowser.h"
#include "jleEditorFrameGraphWindow.h"
#include "jleEditorGizmos.h"
#include "jleEditorNotifications.h"
#include "jleEditorProfilerWindow.h"
#include "jleEditorResourceViewer.h"
#include "jleEditorSaveState.h"
#include "jleEditorWindowsPanel.h"
#include "jleECSEditorWindow.h"
#include "jleResourceIndexer.h"
#include "jleEditor3DImportWindow.h"
#include "jleEditorResourceEdit.h"
#include "jleEditorSettingsWindow.h"
#include "jleEditorTextEdit.h"
#include "jleGameEditorWindow.h"
#include "jleSceneEditorWindow.h"
#include "jleUndoRedo.h"
#include "jlECS/jlECS.h"

#include "game/jleGame.h"
#include "modules/graphics/core/jleFramebufferMultisample.h"
#include "modules/graphics/core/jleFramebufferScreen.h"
#include "modules/graphics/core/jleGLError.h"
#include "modules/graphics/jleGraphics.h"
#include "modules/graphics/jleQuadRendering.h"
#include "modules/graphics/jleRenderThread.h"
#include "modules/physics/jlePhysics.h"
#include "modules/scripting/jleLuaEnvironment.h"
#include "modules/windowing/jleWindow.h"

#include <ImGui/ImGuizmo.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <implot/implot.h>
#include <modules/hierarchy/editor/jleHierarchyModuleEditor.h>
#include <modules/graphics/editor/jleGraphicsModuleEditor.h>

#include <WickedEngine/wiJobSystem.h>
#include <plog/Log.h>


struct jleEditor::jleEditorInternal {
    jleResourceRef<jleEditorSaveState> editorSaveState;
};

class jleEditor::jleEditorWindows
{
public:
    struct ConstructContext{
        jleSerializationContext& serializationContext;
        jleEditorSaveState& saveState;
        jleEngineSettings& engineSettings;
    };

    explicit jleEditorWindows(const ConstructContext &ctx)
    {
        jleSerializationContext& serializationContext = ctx.serializationContext;

        // Note: Important that menu comes first here, since the others are
        // dependent on the menu's dockspace.
        menu = std::make_shared<jleEditorWindowsPanel>("Menu", serializationContext, ctx.engineSettings);

        textEditWindow = std::make_shared<jleEditorTextEdit>("Text Editor");

        resourceEditor = std::make_shared<jleEditorResourceEdit>("Resource Edit");

        sceneWindow = std::make_shared<jleSceneEditorWindow>("Scene Window");
        sceneWindow->fpvCamController.position = ctx.saveState.cameraPosition;
        sceneWindow->fpvCamController.yaw = ctx.saveState.cameraYaw;
        sceneWindow->fpvCamController.pitch = ctx.saveState.cameraPitch;
        menu->addWindow(sceneWindow);

        gameWindow = std::make_shared<jleGameEditorWindow>("Game Window");
        menu->addWindow(gameWindow);

        console = std::make_shared<jleConsoleEditorWindow>("Console Window");
        menu->addWindow(console);

        settingsWindow = std::make_shared<jleEditorSettingsWindow>("Engine Settings");
        menu->addWindow(settingsWindow);

        contentBrowser = std::make_shared<jleEditorContentBrowser>(
            "Content Browser", serializationContext, textEditWindow, resourceEditor);
        menu->addWindow(contentBrowser);

        buildTool = std::make_shared<jleEditorBuild>("Build Tool");
        menu->addWindow(buildTool);

        resourceViewer = std::make_shared<jleEditorResourceViewer>("Resource Viewer");
        menu->addWindow(resourceViewer);

        profilerWindow = std::make_shared<jleEditorProfilerWindow>("Profiler");
        menu->addWindow(profilerWindow);

        import3DWindow = std::make_shared<jleEditor3DImportWindow>("Model Importer");
        menu->addWindow(import3DWindow);

        notifications = std::make_shared<jleEditorNotifications>("Notifications", serializationContext);

        frameGraph = std::make_shared<jleEditorFrameGraphWindow>("Frame Graph");

        ecsWindow = std::make_shared<jleECSEditorWindow>("ECS Window");
        menu->addWindow(ecsWindow);
    }

    std::shared_ptr<jleEditorWindowsPanel> menu;
    std::shared_ptr<jleEditorTextEdit> textEditWindow;
    std::shared_ptr<jleEditorResourceEdit> resourceEditor;
    std::shared_ptr<jleSceneEditorWindow> sceneWindow;
    std::shared_ptr<jleGameEditorWindow> gameWindow;
    std::shared_ptr<jleConsoleEditorWindow> console;
    std::shared_ptr<jleEditorSettingsWindow> settingsWindow;
    std::shared_ptr<jleEditorSceneObjectsWindow> editorSceneObjects;
    std::shared_ptr<jleEditorContentBrowser> contentBrowser;
    std::shared_ptr<jleEditorBuild> buildTool;
    std::shared_ptr<jleEditorResourceViewer> resourceViewer;
    std::shared_ptr<jleEditorProfilerWindow> profilerWindow;
    std::shared_ptr<jleEditor3DImportWindow> import3DWindow;
    std::shared_ptr<jleEditorNotifications> notifications;
    std::shared_ptr<jleEditorFrameGraphWindow> frameGraph;
    std::shared_ptr<jleECSEditorWindow> ecsWindow;

    jleUndoRedoManager _undoRedo{};

    void
    renderUI(jleEditorUpdateContext &context)
    {
        jlECS::ECS* ecs = context.engineUpdateContext.gameRuntime.isGameKilled() ? nullptr : context.gameState.ecs.get();

        // Temporary work-around to ensure we get the *game* ECS
        if (!ecs) {
            context.engineUpdateContext.gameRuntime.startGame();
            return;
        }
        assert(dynamic_cast<jlECS::Debug::ECS_Debug*>(ecs));

        menu->renderUI(context.engineUpdateContext);
        textEditWindow->renderUI();
        resourceEditor->renderUI(context);

        const auto ecsWindowOutput = ecsWindow->renderUI({
            .editorUpdate = context,
            .ecs = *ecs,
            .undoRedo = _undoRedo
        });

        sceneWindow->renderUI({.editorUpdate = context,
                               .ecs = *ecs,
                               .selectedObjects = ecsWindowOutput.selectedObjects,
                               .physics = *context.gameState.physics,
                               .undoRedo = _undoRedo
        });

        gameWindow->renderUI(context.engineUpdateContext, context.engineUpdateContext.inputModule);
        console->renderUI(context.engineUpdateContext, context.engineUpdateContext.luaEnvironment);
        settingsWindow->renderUI(context);
        contentBrowser->renderUI(context);
        buildTool->renderUI(context.engineUpdateContext, context.resourceIndexer);
        resourceViewer->renderUI(context.engineUpdateContext);
        profilerWindow->renderUI(context.engineUpdateContext);
        import3DWindow->renderUI(context);
        notifications->renderUI(context.engineUpdateContext);
        frameGraph->renderUI(context.engineUpdateContext);
    }
};

jleEditor::jleEditor(EngineConstructConfig& config) : jleGameEngine(config) {}

void
jleEditor::start(jleEngineUpdateContext &ctx)
{
    _internal = std::make_unique<jleEditorInternal>();
    _gizmos = std::make_unique<jleEditorGizmos>(ctx.serializationContext);

    LOG_INFO << "Starting the editor";

    jleSerializationContext& serializationContext = ctx.serializationContext;

    _internal->editorSaveState =
        jleResourceRef<jleEditorSaveState>(jlePath{"BI:editor_save.edsave"}, serializationContext);

    std::vector<std::string> directoriesForNotification;
    directoriesForNotification.push_back(jlePath{"ER:/"}.getRealPath());
    directoriesForNotification.push_back(jlePath{"ED:/"}.getRealPath());
    directoriesForNotification.push_back(jlePath{"GR:/"}.getRealPath());
    _resourceIndexer = std::make_unique<jleResourceIndexer>(directoriesForNotification);

    initImgui();

    jleEditorWindows::ConstructContext editorWindowsConstructCtx{
        .serializationContext = serializationContext,
        .saveState = *_internal->editorSaveState.get(),
        .engineSettings = ctx.settings,
    };
   _editorWindows = std::make_unique<jleEditorWindows>(editorWindowsConstructCtx);

    _sceneWindow = _editorWindows->sceneWindow;
    _editorSceneObjects = _editorWindows->editorSceneObjects;

    ctx.windowModule.addWindowResizeCallback(
        std::bind(&jleEditor::mainEditorWindowResized, this, std::placeholders::_1));

    int x, y;
    glfwGetFramebufferSize(ctx.windowModule.glfwWindow(), &x, &y);
    ctx.windowModule.glfwFramebufferSizeCallback(ctx.windowModule.glfwWindow(), x, y);

    LOG_INFO << "Starting the game in editor mode";

    _luaEnvironment->loadScript("ER:/scripts/engine.lua", serializationContext);
    _luaEnvironment->loadScript("ER:/scripts/globals.lua", serializationContext);
    _luaEnvironment->loadScript("ED:/scripts/editor.lua", serializationContext);

    startRmlUi();

    if (saveState().gameRunning) {
        ctx.gameRuntime.startGame();
    }
}

void
jleEditor::render(jleCamera& camera, jleEngineUpdateContext &ctx, wi::jobsystem::context &jobsCtx)
{
    JLE_SCOPE_PROFILE_GPU(EditorRender);

    _resourceIndexer->update(ctx.serializationContext, *_editorWindows->textEditWindow);
    _luaEnvironment->loadNewlyAddedScripts(ctx.serializationContext);

    jleFramePacket& framePacket = *_previousFramePacket;

    if(!ctx.gameRuntime.isGameKilled() && _previousFramePacket)
    {
        jleFramebufferInterface& gameFramebuffer = *ctx.gameRuntime.mainGameScreenFramebuffer;
        framePacket.camera = camera;
        renderGameView(framePacket, gameFramebuffer);
    }

    // Wait for game thread
    Wait(jobsCtx);

    if (_previousFramePacket) {
        jleFramePacket& framePacketModifiedByEditor = *_previousFramePacket;
        _editorWindows->sceneWindow->renderEditorGrid(framePacketModifiedByEditor);

        jleEditorUpdateContext editorUpdateCtx{
            .engineUpdateContext = ctx,
            .resourceIndexer = *_resourceIndexer,
            .gizmos = *_gizmos,
            .gameState = _gameRuntime->getGame().getGameState(),
            .editorFramePacket = framePacketModifiedByEditor
        };

        updateEditorGameModules(editorUpdateCtx);

        renderEditorSceneView(editorUpdateCtx);

        renderEditorUI(editorUpdateCtx);
    }

    glCheckError("Main Editor Render");
}

void
jleEditor::renderGameView(const jleFramePacket &framePacketIn,
                          jleFramebufferInterface &framebufferOut)
{
    JLE_SCOPE_PROFILE_CPU(RenderGameView);

    _renderThread->processRenderQueue();

    static jleFramebufferMultisample msaa{framebufferOut.width(), framebufferOut.height(), 4};

    if (framebufferOut.width() != msaa.width() || framebufferOut.height() != msaa.height()) {
        msaa.resize(framebufferOut.width(), framebufferOut.height());
    }

    renderer().render(msaa, framePacketIn);
    msaa.blitToOther(framebufferOut);

    glCheckError("Render MSAA Game View");
}

void
jleEditor::renderEditorSceneView(jleEditorUpdateContext &ctx)
{
    JLE_SCOPE_PROFILE_CPU(RenderEditorSceneView);

    if (_previousFramePacket) {
        _editorWindows->sceneWindow->render(*_previousFramePacket, ctx);
    }

    glCheckError("Render MSAA Scene View");
}

void
jleEditor::renderEditorUI(jleEditorUpdateContext& ctx)
{
    ZoneScoped;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set viewport to cover the entire screen
    glViewport(0, 0, _window->width(), _window->height());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();

    _editorWindows->renderUI(ctx);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glCheckError("ImGui");

    auto &&io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glCheckError("Render Editor UI");
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
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    static const std::string iniFile = GAME_RESOURCES_DIRECTORY + "/imgui.ini";
    io.IniFilename = iniFile.c_str();

    ImGui::Spectrum::StyleColorsSpectrum();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(_window->glfwWindow(), true);

    JLE_EXEC_IF(JLE_BUILD_OPENGLES30) { ImGui_ImplOpenGL3_Init("#version 300 es"); }
    else
    {
        ImGui_ImplOpenGL3_Init("#version 330 core");
    }

    io.Fonts->Clear();
    ImGui::Spectrum::LoadFont();
}


void
jleEditor::mainEditorWindowResized(const jleWindowResizeEvent &resizeEvent)
{
    auto &&io = ImGui::GetIO();
    io.FontGlobalScale = 1.0f;
    int w = resizeEvent.framebufferWidth;
    int h = resizeEvent.framebufferHeight;

    w = static_cast<int>(static_cast<float>(w) / resizeEvent.contentScaleX);
    h = static_cast<int>(static_cast<float>(h) / resizeEvent.contentScaleY);

    constexpr int scale0 = 1080 * 720;
    constexpr int scale1 = 1920 * 1080;
    constexpr int scale2 = 2048 * 1536;
    constexpr int scale3 = 4096 * 1728;

    io.FontGlobalScale = 1.0f;

    if (w * h >= scale0) {
        io.FontGlobalScale = 1.0f;
    }

    if (w * h >= scale1) {
        io.FontGlobalScale = 1.25f;
    }

    if (w * h >= scale2) {
        io.FontGlobalScale = 1.5f;
    }

    if (w * h > scale3) {
        io.FontGlobalScale = 2.0f;
    }
}

void
jleEditor::exiting()
{
    saveState().gameRunning = !_gameRuntime->isGameKilled();
    saveState().cameraPosition = _editorWindows->sceneWindow->getCameraPosition();
    saveState().loadedScenePaths.clear();
    saveState().cameraYaw = _sceneWindow->fpvCamController.yaw;
    saveState().cameraPitch = _sceneWindow->fpvCamController.pitch;

    auto serializationContext = createSerializationContext();
    saveState().saveToFile(serializationContext);

    jleGameEngine::exiting();
}
jleEditorGizmos &
jleEditor::gizmos()
{
    return *_gizmos;
}

jleEditorSaveState &
jleEditor::saveState()
{
    return *_internal->editorSaveState.get();
}

jleEditor::~jleEditor() = default;

void jleEditor::updateEditorGameModules(jleEditorUpdateContext &ctx) {

    auto& modules = ctx.engineUpdateContext.gameRuntime.getGame().getModules();

    if (auto* graphicsEditorModule = dynamic_cast<jleGraphicsModuleEditor*>(modules.graphicsModule.get())){
        graphicsEditorModule->updateEditor(ctx);
    }
}