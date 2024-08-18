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
#include "editor/jleConsoleEditorWindow.h"
#include "editor/jleEditorBuild.h"
#include "editor/jleEditorContentBrowser.h"
#include "editor/jleEditorFrameGraphWindow.h"
#include "editor/jleEditorGizmos.h"
#include "editor/jleEditorImGuiWindowInterface.h"
#include "editor/jleEditorNotifications.h"
#include "editor/jleEditorProfilerWindow.h"
#include "editor/jleEditorResourceViewer.h"
#include "editor/jleEditorSaveState.h"
#include "editor/jleEditorSceneObjectsWindow.h"
#include "editor/jleEditorWindowsPanel.h"

#include "core/jleResourceIndexer.h"
#include "jleEditor3DImportWindow.h"
#include "jleEditorResourceEdit.h"
#include "jleEditorSettingsWindow.h"
#include "jleEditorTextEdit.h"
#include "jleGLError.h"
#include "jleGame.h"
#include "jleGameEditorWindow.h"
#include "jleLuaEnvironment.h"
#include "jlePath.h"
#include "jlePathDefines.h"
#include "jlePhysics.h"
#include "jleQuadRendering.h"
#include "jleRenderThread.h"
#include "jleResourceRef.h"
#include "jleSceneEditorWindow.h"
#include "modules/graphics/core/jleFramebufferMultisample.h"
#include "modules/graphics/core/jleFramebufferScreen.h"
#include "modules/graphics/jle3DRenderer.h"
#include "modules/windowing/jleWindow.h"

#include <ImGui/ImGuizmo.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <implot/implot.h>

#include <WickedEngine/wiJobSystem.h>
#include <plog/Log.h>

struct jleEditor::jleEditorInternal {
    jleResourceRef<jleEditorSaveState> editorSaveState;
};

class jleEditor::jleEditorWindows
{
public:
    explicit jleEditorWindows(const jleEditorModulesContext &ctx)
    {
        // Note: Important that menu comes first here, since the others are
        // dependent on the menu's dockspace.
        menu = std::make_shared<jleEditorWindowsPanel>("Menu");

        textEditWindow = std::make_shared<jleEditorTextEdit>("Text Editor");

        resourceEditor = std::make_shared<jleEditorResourceEdit>("Resource Edit");

        sceneWindow = std::make_shared<jleSceneEditorWindow>("Scene Window");
        sceneWindow->fpvCamController.position = ctx.editor.saveState().cameraPosition;
        sceneWindow->fpvCamController.yaw = ctx.editor.saveState().cameraYaw;
        sceneWindow->fpvCamController.pitch = ctx.editor.saveState().cameraPitch;
        menu->addWindow(sceneWindow);

        gameWindow = std::make_shared<jleGameEditorWindow>("Game Window");
        menu->addWindow(gameWindow);

        console = std::make_shared<jleConsoleEditorWindow>("Console Window");
        menu->addWindow(console);

        settingsWindow = std::make_shared<jleEditorSettingsWindow>("Engine Settings");
        menu->addWindow(settingsWindow);

        editorSceneObjects = std::make_shared<jleEditorSceneObjectsWindow>("Scene Objects");
        menu->addWindow(editorSceneObjects);

        contentBrowser = std::make_shared<jleEditorContentBrowser>("Content Browser", textEditWindow, resourceEditor);
        menu->addWindow(contentBrowser);

        buildTool = std::make_shared<jleEditorBuild>("Build Tool");
        menu->addWindow(buildTool);

        resourceViewer = std::make_shared<jleEditorResourceViewer>("Resource Viewer");
        menu->addWindow(resourceViewer);

        profilerWindow = std::make_shared<jleEditorProfilerWindow>("Profiler");
        menu->addWindow(profilerWindow);

        import3DWindow = std::make_shared<jleEditor3DImportWindow>("Model Importer");
        menu->addWindow(import3DWindow);

        notifications = std::make_shared<jleEditorNotifications>("Notifications", ctx.engineModulesContext.resourcesModule);

        frameGraph = std::make_shared<jleEditorFrameGraphWindow>("Frame Graph");
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

    void
    renderUI(const jleEditorModulesContext &context) const
    {
        menu->renderUI({context.engineModulesContext.frameInfo, context.engine});
        textEditWindow->renderUI();
        resourceEditor->renderUI(context.engine);
        sceneWindow->renderUI(context.engine);
        gameWindow->renderUI(context.engine);
        console->renderUI(context.engine);
        settingsWindow->renderUI(context.engine);
        editorSceneObjects->renderUI(context.engine, context.engineModulesContext);
        contentBrowser->renderUI(context.engineModulesContext);
        buildTool->renderUI(context.engine, context.editor.resourceIndexer());
        resourceViewer->renderUI(context.engine);
        profilerWindow->renderUI(context.engine);
        import3DWindow->renderUI(context.engine, context.engineModulesContext);
        notifications->renderUI(context.engine);
        frameGraph->renderUI(context.engine);
    }
};

jleEditor::jleEditor()
{
    gEditor = this;

}

void
jleEditor::start(jleEngineModulesContext &context)
{
    _editorContext = std::make_unique<jleEditorModulesContext>(context, *this, *this);

    _internal = std::make_unique<jleEditorInternal>();
    _camera = std::make_unique<jleCamera>();
    _gizmos = std::make_unique<jleEditorGizmos>(_editorContext->engineModulesContext.resourcesModule);

    LOG_INFO << "Starting the editor";

    _internal->editorSaveState = jleResourceRef<jleEditorSaveState>(jlePath{"BI:editor_save.edsave"}, context.resourcesModule);

    std::vector<std::string> directoriesForNotification;
    directoriesForNotification.push_back(jlePath{"ER:/"}.getRealPath());
    directoriesForNotification.push_back(jlePath{"ED:/"}.getRealPath());
    directoriesForNotification.push_back(jlePath{"GR:/"}.getRealPath());
    _resourceIndexer = std::make_unique<jleResourceIndexer>(directoriesForNotification);

    initImgui();

    constexpr int initialScreenX = 1024;
    constexpr int initialScreenY = 1024;
    mainScreenFramebuffer = std::make_shared<jleFramebufferScreen>(initialScreenX, initialScreenY);

    // editorScreenFramebuffer = std::make_shared<jleFramebufferScreen>(initialScreenX, initialScreenY);

    _editorWindows = std::make_unique<jleEditorWindows>(*_editorContext);

    _sceneWindow = _editorWindows->sceneWindow;
    _editorSceneObjects = _editorWindows->editorSceneObjects;

    context.windowModule.addWindowResizeCallback(
        std::bind(&jleEditor::mainEditorWindowResized, this, std::placeholders::_1));

    int x, y;
    glfwGetFramebufferSize(context.windowModule.glfwWindow(), &x, &y);
    context.windowModule.glfwFramebufferSizeCallback(context.windowModule.glfwWindow(), x, y);

    LOG_INFO << "Starting the game in editor mode";

    luaEnvironment()->loadScript("ER:/scripts/engine.lua", context.resourcesModule);
    luaEnvironment()->loadScript("ER:/scripts/globals.lua", context.resourcesModule);

    luaEnvironment()->loadScript("ED:/scripts/editor.lua", context.resourcesModule);

    startRmlUi();

    if (saveState().gameRunning) {
        startGame();
    }

    for (auto &&scenePath : saveState().loadedScenePaths) {
        loadScene(scenePath, context, false);
    }
}

void
jleEditor::render(jleEngineModulesContext& modulesContext, wi::jobsystem::context &jobsCtx)
{
    JLE_SCOPE_PROFILE_GPU(EditorRender);

    // TODO : This needs to be restructured.
    // Possibly fix with ECS/context passing struct
    Wait(jobsCtx);

    renderGameView(modulesContext.resourcesModule);

    renderEditorSceneView();

    // UI can touch game state, so we need to for game logic update to complete

    renderEditorGridGizmo();
    // renderEditorGizmos(); // disable since for some reason we're rendering it into the game view lol
    renderEditorUI();

    // resetRenderGraphForNewFrame();

    glCheckError("Main Editor Render");
}

void
jleEditor::renderGameView(jleResources& resources)
{
    JLE_SCOPE_PROFILE_CPU(RenderGameView);

    _renderThread->processRenderQueue();

    if (!_gameHalted && _game) {
        // Render to game view
        static jleFramebufferMultisample msaa{mainScreenFramebuffer->width(), mainScreenFramebuffer->height(), 4};

        if (mainScreenFramebuffer->width() != msaa.width() || mainScreenFramebuffer->height() != msaa.height()) {
            msaa.resize(mainScreenFramebuffer->width(), mainScreenFramebuffer->height());
        }

        if (_3dRenderGraphForRendering) {
            renderer().render(msaa, _game->mainCamera, *_3dRenderGraphForRendering, renderSettings(), resources);
            msaa.blitToOther(*mainScreenFramebuffer);
        }
    }

    glCheckError("Render MSAA Game View");
}

void
jleEditor::renderEditorSceneView()
{
    JLE_SCOPE_PROFILE_CPU(RenderEditorSceneView);

    if (!isGameKilled()) {
        if (auto &&scene = gEditor->getEditorSceneObjectsWindow().GetSelectedScene().lock()) {
            if (scene->getPhysics().renderDebugEnabled) {
                if (_3dRenderGraphForRendering) {
                    scene->getPhysics().renderDebug(*_3dRenderGraphForRendering);
                }
            }
        }
    }

    if (_3dRenderGraphForRendering) {
        _editorWindows->sceneWindow->render(*_3dRenderGraphForRendering, *_editorContext);
    }

    glCheckError("Render MSAA Scene View");
}

void
jleEditor::renderEditorUI()
{
    ZoneScoped;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set viewport to cover the entire screen
    glViewport(0, 0, window().width(), window().height());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();

    _editorWindows->renderUI(*_editorContext);

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
    ImGui_ImplGlfw_InitForOpenGL(window().glfwWindow(), true);

    JLE_EXEC_IF(JLE_BUILD_OPENGLES30) { ImGui_ImplOpenGL3_Init("#version 300 es"); }
    else
    {
        ImGui_ImplOpenGL3_Init("#version 330 core");
    }

    io.Fonts->Clear();
    ImGui::Spectrum::LoadFont();
}

void
jleEditor::addImGuiWindow(std::shared_ptr<jleEditorWindowInterface> window)
{
    _imGuiWindows.push_back(window);
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

std::vector<std::shared_ptr<jleScene>> &
jleEditor::getEditorScenes()
{
    return _editorScenes;
}

void
jleEditor::updateEditorLoadedScenes(jleEngineModulesContext& ctx)
{
    JLE_SCOPE_PROFILE_CPU(jleEditor_updateEditorLoadedScenes)
    for (int i = _editorScenes.size() - 1; i >= 0; i--) {
        if (!_editorScenes[i] || _editorScenes[i]->bPendingSceneDestruction) {
            _editorScenes.erase(_editorScenes.begin() + i);
            continue;
        }

        _editorScenes[i]->updateSceneEditor(ctx);
    }
}

void
jleEditor::update(jleEngineModulesContext& ctx)
{
    _resourceIndexer->update(resources(), *_editorWindows->textEditWindow);

    _luaEnvironment->loadNewlyAddedScripts(ctx.resourcesModule);
    jleGameEngine::update(ctx);
    if (isGameKilled()) {
        JLE_SCOPE_PROFILE_CPU(updateEditorLoadedScenes)
        updateEditorLoadedScenes(ctx);
    }
}

void
jleEditor::renderEditorGizmos()
{
    JLE_SCOPE_PROFILE_CPU(renderEditorGizmos)

    if (!gEngine->isGameKilled()) {
        for (const auto &scene : gEngine->gameRef().activeScenesRef()) {
            for (auto &&o : scene->sceneObjects()) {
                renderEditorGizmosObject(o.get());
            }
        }
    }

    for (const auto &scene : gEditor->getEditorScenes()) {
        for (auto &&o : scene->sceneObjects()) {
            renderEditorGizmosObject(o.get());
        }
    }
}

void
jleEditor::renderEditorGizmosObject(jleObject *object)
{
    for (auto &&c : object->components()) {
        c->editorGizmosRender(true);
    }
    for (auto &&child : object->childObjects()) {
        renderEditorGizmosObject(child.get());
    }
}
void
jleEditor::renderEditorGridGizmo()
{
    JLE_SCOPE_PROFILE_CPU(renderEditorGridGizmo)

    jle3DLineVertex v1{glm::vec3{0.f}, glm::vec3{1.f, 0.f, 0.f}, {1.0f, 0.44f, 1.2f}};
    jle3DLineVertex v2 = v1;
    v2.position = glm::vec3{100.f, 0.f, 0.f};
    v1.position = glm::vec3{-100.f, 0.f, 0.f};

    renderGraph().sendLine(v1, v2);

    v2.position = glm::vec3{0.f, 100.f, 0.f};
    v1.position = glm::vec3{0.f, -100.f, 0.f};
    v1.color = glm::vec3{0.f, 1.f, 0.f};
    v2.color = glm::vec3{0.f, 1.f, 0.f};
    renderGraph().sendLine(v1, v2);

    v2.position = glm::vec3{0.f, 0.f, 100.f};
    v1.position = glm::vec3{0.f, 0.f, -100.f};
    v1.color = glm::vec3{0.f, 0.f, 1.f};
    v2.color = glm::vec3{0.f, 0.f, 1.f};
    renderGraph().sendLine(v1, v2);

    v1.color = glm::vec3(1.0f, 0.3f, 0.3f);
    v2.color = glm::vec3(1.0f, 0.3f, 0.3f);
    auto pos = gEditor->camera().getPosition();

    float scale = 1.f;
    if (abs(pos.y) < 50.f) {
        scale = 0.5f;
    }

    if (abs(pos.y) < 10.f) {
        scale = 0.1f;
    }

    pos.y = 0;
    pos.x = glm::round(pos.x / (100.f * scale)) * (100.f * scale);
    pos.z = glm::round(pos.z / (100.f * scale)) * (100.f * scale);

    v1.position = pos;
    v1.position.z = -1000 * scale + pos.z;
    v2.position = pos;
    v2.position.z = 1000 * scale + pos.z;

    v1.color = glm::vec3{0.3f, 0.3f, 0.7f};
    v2.color = glm::vec3{0.3f, 0.3f, 0.7f};

    for (int i = -10; i <= 10; i++) {
        v1.position.x = 100.f * scale * i + pos.x;
        v2.position.x = 100.f * scale * i + pos.x;
        renderGraph().sendLine(v1, v2);
    }

    v1.position = pos;
    v1.position.x = -1000 * scale + pos.x;
    v2.position = pos;
    v2.position.x = 1000 * scale + pos.x;

    v1.color = glm::vec3{0.7f, 0.3f, 0.3f};
    v2.color = glm::vec3{0.7f, 0.3f, 0.3f};

    for (int i = -10; i <= 10; i++) {
        v1.position.z = 100.f * scale * i + pos.z;
        v2.position.z = 100.f * scale * i + pos.z;
        renderGraph().sendLine(v1, v2);
    }
}

void
jleEditor::exiting()
{
    saveState().gameRunning = !isGameKilled();
    saveState().cameraPosition = gEditor->camera().getPosition();
    saveState().loadedScenePaths.clear();
    for (auto &&scene : _editorScenes) {
        saveState().loadedScenePaths.push_back(scene->path);
    }
    saveState().cameraYaw = _sceneWindow->fpvCamController.yaw;
    saveState().cameraPitch = _sceneWindow->fpvCamController.pitch;
    // saveState().orthographicCamera = !static_cast<bool>(projectionType);
    saveState().saveToFile();

    jleGameEngine::exiting();
}

jleEditorTextEdit &
jleEditor::editorTextEdit()
{
    return *_textEditWindow;
}

jleEditorSceneObjectsWindow &
jleEditor::getEditorSceneObjectsWindow()
{
    return *_editorSceneObjects;
}

jleCamera &
jleEditor::camera()
{
    return *_camera.get();
}
bool

jleEditor::checkSceneIsActiveEditor(const std::string &sceneName)
{
    for (auto &&scene : _editorScenes) {
        if (sceneName == scene->sceneName) {
            return true;
        }
    }

    return false;
}

std::shared_ptr<jleScene>
jleEditor::loadScene(const jlePath &scenePath, jleEngineModulesContext& ctx, bool startObjects)
{
    auto scene = gEngine->resources().loadResourceFromFile<jleScene>(scenePath, true);

    auto it = std::find(_editorScenes.begin(), _editorScenes.end(), scene);
    if (it == _editorScenes.end()) {
        _editorScenes.push_back(scene);
        if (startObjects) {
            scene->startObjects(ctx);
        }
    } else {
        LOG_WARNING << "Loaded scene is already loaded";
    }

    return scene;
}

jleEditorGizmos &
jleEditor::gizmos()
{
    return *_gizmos.get();
}

jleEditorSaveState &
jleEditor::saveState()
{
    return *_internal->editorSaveState.get();
}

jleResourceIndexer &
jleEditor::resourceIndexer()
{
    return *_resourceIndexer.get();
}

jleEditor::~jleEditor() = default;
