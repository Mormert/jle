add_library (engine STATIC
    "3rdparty/glad/glad.c"
    "3rdparty/gltext/gltext.c"
    "3rdparty/ImGui/imgui.cpp"
    "3rdparty/ImGui/imgui_demo.cpp"
    "3rdparty/ImGui/imgui_draw.cpp"
    "3rdparty/ImGui/imgui_impl_glfw.cpp"
    "3rdparty/ImGui/imgui_impl_opengl3.cpp"
    "3rdparty/ImGui/imgui_tables.cpp"
    "3rdparty/ImGui/imgui_widgets.cpp"
    "3rdparty/stb_image.cpp"
    "ConsoleEditorWindow.cpp"
    "EditorBackgroundImage.cpp"
    "EditorGameControllerWidget.cpp"
    "EditorJsonToImgui.cpp"
    "EditorSceneObjectsWindow.cpp"
    "EditorWindowsPanel.cpp"
    "EngineSettingsWindow.cpp"
    "GameAssets.cpp"
    "GameEditorWindow.cpp"
    "Image.cpp"
    "iTexture.cpp"
    "jleComponent.cpp"
    "jleCore.cpp"
    "EngineStatus.cpp"
    "jleEditor.cpp"
    "jleGame.cpp"
    "jleObject.cpp"
    "jleScene.cpp"
    "ProfilerWindow.cpp"
    "TextRendering_OpenGL.cpp"
    "TextureCreator_OpenGL.cpp"
    "WindowInitializer_GLFW_OpenGL.cpp"
    "WindowFactory_GLFW.cpp"
    "GLState.cpp"
    "jleGameEngine.cpp"
    "KeyboardInputInternal.cpp"
    "MouseInputInternal.cpp"
    "FrameBuffer_OpenGL.cpp"
    "FramebufferFullscreenRenderer_OpenGL.cpp"
    "FullscreenRendering_OpenGL.cpp"
    "QuadRendering_OpenGL.cpp"
    "RenderingAPI_OpenGL.cpp"
    "RenderingFactory_OpenGL.cpp"
    "Shader_OpenGL.cpp"
    "Texture.cpp"
    "Viewport.cpp"
    "Window_GLFW_OpenGL.cpp"
    "EditorContentBrowser.cpp"
    "cSprite.cpp"
    "cTransform.cpp"
    "oStaticSprite.cpp")

target_link_libraries(engine PUBLIC ${CMAKE_DL_LIBS})

# Remove GLFW tests, examples and docs from build
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/git_submodules/glfw)
target_link_libraries(engine PUBLIC glfw)

target_include_directories(engine SYSTEM PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/plog/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/git_submodules/glm")

target_include_directories (engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

FILE(COPY EditorResources DESTINATION ${PROJECT_BINARY_DIR}/${JLE_GAME_BUILD})