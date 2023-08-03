// Copyright (c) 2023. Johan Lind

#include "jleEditor.h"
#include "editor/jleEditorImGuiWindowInterface.h"
#include "editor/jleConsoleEditorWindow.h"
#include "editor/jleEditorContentBrowser.h"
#include "editor/jleEditorNotifications.h"
#include "editor/jleEditorProfilerWindow.h"
#include "editor/jleEditorResourceViewer.h"
#include "editor/jleEditorSceneObjectsWindow.h"
#include "editor/jleEditorWindowsPanel.h"
#include "editor/jleEditorSaveState.h"
#include "editor/jleEditorGizmos.h"
#include "jleFramebufferMultisample.h"
#include "jleGLError.h"
#include "jlePathDefines.h"
#include "jlePath.h"
#include "jleResourceRef.h"
#include "jleEditorResourceEdit.h"
#include "jleEditorSettingsWindow.h"
#include "jleEditorTextEdit.h"
#include "jleFileChangeNotifier.h"
#include "jleFramebufferScreen.h"
#include "jleGame.h"
#include "jleGameEditorWindow.h"
#include "jlePhysics.h"
#include "jleQuadRendering.h"
#include "jleSceneEditorWindow.h"
#include "jleWindow.h"
#include "jleLuaEnvironment.h"
#include "jle3DRenderer.h"


#include "Remotery/Remotery.h"
#include "plog/Log.h"
#include "ImGui/ImGuizmo.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

struct jleEditor::jleEditorInternal{
    jleResourceRef<jleEditorSaveState> editorSaveState;
};

jleEditor::jleEditor()
{
    gEditor = this;
    _internal = std::make_unique<jleEditorInternal>();
    _camera = std::make_unique<jleCamera>();
    _gizmos = std::make_unique<jleEditorGizmos>();
}

void
jleEditor::start()
{
    LOG_INFO << "Starting the editor";

    _internal->editorSaveState = jleResourceRef<jleEditorSaveState>(jlePath{"BI:editor_save.edsave"});

    std::vector<std::string> directoriesForNotification;
    directoriesForNotification.push_back(jlePath{"ER:/"}.getRealPath());
    directoriesForNotification.push_back(jlePath{"ED:/"}.getRealPath());
    directoriesForNotification.push_back(jlePath{"GR:/"}.getRealPath());
    _fileChangeNotifier = std::make_unique<jleFileChangeNotifier>(directoriesForNotification);

    initImgui();

    constexpr int initialScreenX = 1024;
    constexpr int initialScreenY = 1024;
    mainScreenFramebuffer = std::make_shared<jleFramebufferScreen>(initialScreenX, initialScreenY);

    editorScreenFramebuffer = std::make_shared<jleFramebufferScreen>(initialScreenX, initialScreenY);

    // Note: Important that menu comes first here, since the others are
    // dependent on the menu's dockspace.
    auto menu = std::make_shared<jleEditorWindowsPanel>("Menu");
    addImGuiWindow(menu);

    _textEditWindow = std::make_shared<jleEditorTextEdit>("Text Editor");
    addImGuiWindow(_textEditWindow);

    auto resourceEditor = std::make_shared<jleEditorResourceEdit>("Resource Edit");
    addImGuiWindow(resourceEditor);

    _sceneWindow = std::make_shared<jleSceneEditorWindow>("Scene Window", editorScreenFramebuffer);
    _sceneWindow->fpvCamController.position = saveState().cameraPosition;
    _sceneWindow->fpvCamController.yaw = saveState().cameraYaw;
    _sceneWindow->fpvCamController.pitch = saveState().cameraPitch;
    addImGuiWindow(_sceneWindow);
    menu->addWindow(_sceneWindow);

    auto gameWindow = std::make_shared<jleGameEditorWindow>("Game Window");
    addImGuiWindow(gameWindow);
    menu->addWindow(gameWindow);

    auto console = std::make_shared<jleConsoleEditorWindow>("Console Window");
    addImGuiWindow(console);
    menu->addWindow(console);

    auto settingsWindow = std::make_shared<jleEditorSettingsWindow>("Engine Settings");
    addImGuiWindow(settingsWindow);
    menu->addWindow(settingsWindow);

    // auto gameController =
    // std::make_shared<EditorGameControllerWidget>("Controller");
    // AddImGuiWindow(gameController);
    // menu->addWindow(gameController);

    _editorSceneObjects = std::make_shared<jleEditorSceneObjectsWindow>("Scene Objects");
    addImGuiWindow(_editorSceneObjects);
    menu->addWindow(_editorSceneObjects);

    auto contentBrowser = std::make_shared<jleEditorContentBrowser>("Content Browser", _textEditWindow, resourceEditor);
    addImGuiWindow(contentBrowser);
    menu->addWindow(contentBrowser);

    auto resourceViewer = std::make_shared<jleEditorResourceViewer>("Resource Viewer");
    addImGuiWindow(resourceViewer);
    menu->addWindow(resourceViewer);

    auto profilerWindow = std::make_shared<jleEditorProfilerWindow>("Profiler");
    addImGuiWindow(profilerWindow);
    menu->addWindow(profilerWindow);

    auto notifications = std::make_shared<jleEditorNotifications>("Notifications");
    addImGuiWindow(notifications);

    gEngine->window().addWindowResizeCallback(
        std::bind(&jleEditor::mainEditorWindowResized, this, std::placeholders::_1, std::placeholders::_2));

    int w, h;
    glfwGetFramebufferSize(gEngine->window().glfwWindow(), &w, &h);
    gEngine->window().executeResizeCallbacks(w, h);

    LOG_INFO << "Starting the game in editor mode";

    luaEnvironment()->loadScript("ER:/scripts/engine.lua");
    luaEnvironment()->loadScript("ED:/scripts/editor.lua");

    startRmlUi();

    if (saveState().gameRunning) {
        startGame();
    }

    for (auto &&scenePath : saveState().loadedScenePaths) {
        loadScene(scenePath, false);
    }
}

void
jleEditor::render()
{
    JLE_SCOPE_PROFILE_GPU(EditorRender);

    renderGameView();

    renderEditorSceneView();

    renderEditorUI();

    resetRenderGraphForNewFrame();

    glCheckError("Main Editor Render");
}

void
jleEditor::renderGameView()
{
    if (!gameHalted && game) {
        // Render to game view
        static jleFramebufferMultisample msaa{mainScreenFramebuffer->width(), mainScreenFramebuffer->height(), 4};

        if (mainScreenFramebuffer->width() != msaa.width() || mainScreenFramebuffer->height() != msaa.height()) {
            msaa.resize(mainScreenFramebuffer->width(), mainScreenFramebuffer->height());
        }

        renderer().render(msaa, game->mainCamera, renderGraph(), renderSettings());
        msaa.blitToOther(*mainScreenFramebuffer);
    }

    glCheckError("Render MSAA Game View");
}

void
jleEditor::renderEditorSceneView()
{
    renderEditorGizmos();

    renderEditorGridGizmo();

    if (!isGameKilled()) {
        physics().renderDebug();
    }

    if (perspectiveCamera) {
        gEditor->camera().setPerspectiveProjection(
            45.f, editorScreenFramebuffer->width(), editorScreenFramebuffer->height(), 10000.f, 0.1f);
    } else {
        gEditor->camera().setOrthographicProjection(editorScreenFramebuffer->width() * _sceneWindow->orthoZoomValue,
                                                    editorScreenFramebuffer->height() * _sceneWindow->orthoZoomValue,
                                                    10000.f,
                                                    -10000.f);
    }

    static jleFramebufferMultisample msaa{editorScreenFramebuffer->width(), editorScreenFramebuffer->height(), 4};

    if (editorScreenFramebuffer->width() != msaa.width() || editorScreenFramebuffer->height() != msaa.height()) {
        msaa.resize(editorScreenFramebuffer->width(), editorScreenFramebuffer->height());
    }

    // Render to editor scene view
    renderer().render(msaa, gEditor->camera(), renderGraph(), renderSettings());
    msaa.blitToOther(*editorScreenFramebuffer);

    glCheckError("Render MSAA Scene View");
}

void
jleEditor::renderEditorUI()
{
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
        for (const auto &window : _imGuiWindows) {
            window->update(*this);
        }

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
jleEditor::addImGuiWindow(std::shared_ptr<jleEditorWindowInterface> window)
{
    _imGuiWindows.push_back(window);
}

void
jleEditor::mainEditorWindowResized(int w, int h)
{
    auto &&io = ImGui::GetIO();
    io.FontGlobalScale = w / 1920.f;
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
    _fileChangeNotifier->periodicSweep();
    jleGameEngine::update(dt);
    if (isGameKilled()) {
        JLE_SCOPE_PROFILE_CPU(updateEditorLoadedScenes)
        updateEditorLoadedScenes(dt);
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
    for (auto &&c : object->customComponents()) {
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
        saveState().loadedScenePaths.push_back(jlePath{scene->filepath, false});
    }
    saveState().cameraYaw = _sceneWindow->fpvCamController.yaw;
    saveState().cameraPitch = _sceneWindow->fpvCamController.pitch;
    //saveState().orthographicCamera = !static_cast<bool>(projectionType);
    saveState().saveToFile();

    jleGameEngine::exiting();
}

jleEditorTextEdit &
jleEditor::editorTextEdit()
{
    return *_textEditWindow;
}

jleEditorSceneObjectsWindow &
jleEditor::editorSceneObjects()
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
jleEditor::loadScene(const jlePath &scenePath, bool startObjects)
{
    auto scene = gEngine->resources().loadResourceFromFile<jleScene>(scenePath, true);

    auto it = std::find(_editorScenes.begin(), _editorScenes.end(), scene);
    if (it == _editorScenes.end()) {
        _editorScenes.push_back(scene);
        scene->onSceneCreation();
        if (startObjects) {
            scene->startObjects();
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
