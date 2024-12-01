
add_library(jleEditor STATIC
        "src/jleEditor.cpp"
        "src/jleImGuiCerealArchive.cpp"
        "src/jleEditorResourceViewer.cpp"
        "src/jleEditorResourceViewer.cpp"
        "src/jleEditorSettingsWindow.cpp"
        "src/jleConsoleEditorWindow.cpp"
        "src/jleEditorGameControllerWidget.cpp"
        "src/jleEditorSceneObjectsWindow.cpp"
        "src/jleEditorProfilerWindow.cpp"
        "src/jleEditorResourceViewer.cpp"
        "src/jleSceneEditorWindow.cpp"
        "src/jleGameEditorWindow.cpp"
        "src/jleEditorContentBrowser.cpp"
        "src/jleEditorWindowsPanel.cpp"
        "src/jleEditorTextEdit.cpp"
        "src/jleEditorResourceEdit.cpp"
        "src/jleEditorNotifications.cpp"
        "src/jleEditorGizmos.cpp"
        "src/jleEditorSaveState.cpp"
        "src/jleImGuiExtensions.cpp"
        "src/jleEditor3DImportWindow.cpp"
        "src/jleEditorFrameGraphWindow.cpp"
        "src/jleResourceIndexer.cpp"
        "src/jleEditorBuild.cpp"
        "src/jleECSEditorWindow.cpp"
        "src/jleEditorWindow.cpp"
        "src/jleLuaEnvironmentEditor.cpp"

        "3rdparty/ImGuiColorTextEdit/TextEditor.cpp"
        "3rdparty/ImGui/imgui.cpp"
        "3rdparty/ImGui/imgui_spectrum.cpp"
        "3rdparty/ImGui/imgui_demo.cpp"
        "3rdparty/ImGui/imgui_draw.cpp"
        "3rdparty/ImGui/imgui_impl_glfw.cpp"
        "3rdparty/ImGui/imgui_impl_opengl3.cpp"
        "3rdparty/ImGui/imgui_tables.cpp"
        "3rdparty/ImGui/imgui_widgets.cpp"
        "3rdparty/ImGui/imgui_stdlib.cpp"
        "3rdparty/ImGui/ImGuizmo.cpp"
        "3rdparty/implot/implot.cpp"
        "3rdparty/implot/implot_items.cpp")

if(APPLE)
    target_sources(jleEditor PRIVATE "src/jleMacOSWindow.mm")
endif ()

target_include_directories(jleEditor SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

target_include_directories(jleEditor PUBLIC SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/public")

target_include_directories(jleEditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/public/editor")

add_subdirectory(../core/editor ${CMAKE_CURRENT_BINARY_DIR}/coreEditor)
target_link_libraries(jleCore_Editor PRIVATE jleEditor)

add_subdirectory(../core/jlECS/editor ${CMAKE_CURRENT_BINARY_DIR}/jlECSEditor)
# Todo: fix this by not having jlECS be its own submodule, that is just annoying at this point
target_link_libraries(jlECS_Editor PRIVATE jleEditor)
target_link_libraries(jleEditor PRIVATE jlECS_Editor)

add_subdirectory(../modules/graphics/editor ${CMAKE_CURRENT_BINARY_DIR}/graphicsEditor)
target_link_libraries(jleGraphics_Editor PRIVATE jleEditor)

add_subdirectory(../modules/scripting/editor ${CMAKE_CURRENT_BINARY_DIR}/scriptingEditor)
target_link_libraries(jleScripting_Editor PRIVATE jleEditor)
