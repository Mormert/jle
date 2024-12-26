add_library(jleGraphics STATIC
        "core/jleFrameBufferInterface.cpp"
        "core/jleFramebufferMultisample.cpp"
        "core/jleFramebufferShadowMap.cpp"
        "core/jleFramebufferShadowCubeMap.cpp"
        "core/jleFramebufferPicking.cpp"
        "core/jleFullscreenRendering.cpp"
        "core/jleFramebufferScreen.cpp"
        "core/jleGLError.cpp"
        "jleGraphicsModule.cpp"
        "jleShader.cpp"
        "jleTexture.cpp"
        "jleGraphics.cpp"
        "jleFramePacket.cpp"
        "jleMesh.cpp"
        "jleSkybox.cpp"
        "jleMaterial.cpp"
        "jleTextureRefOrRGBA.cpp"
        "jleSkinnedMesh.cpp"
        "jleRenderThread.cpp"
        "runtime/components/cCamera.cpp"
        "runtime/components/cMesh.cpp"
        "runtime/components/cSkinnedMesh.cpp"
        "runtime/components/cSkybox.cpp"
        "runtime/components/cLight.cpp"
        "runtime/components/cLightDirectional.cpp"

        "RmlUiImplementation/RmlUi_Backend_GLFW_GL3.cpp"
        "RmlUiImplementation/RmlUi_Renderer_GL3.cpp"
        "RmlUiImplementation/RmlUi_Platform_GLFW.cpp"
        "RmlUiImplementation/shell/src/PlatformExtensions.cpp"
        "RmlUiImplementation/shell/src/RendererExtensions.cpp"
        "RmlUiImplementation/shell/src/Shell.cpp"
        "RmlUiImplementation/shell/src/ShellFileInterface.cpp"
)

if (NOT JLE_BUILD_EMSCRIPTEN) # Native build, excluding some Emscripten-embedded libraries
    # Compile glad.c only when building native
    target_sources(jleGraphics PRIVATE "3rdparty/glad/glad.cpp")

    target_link_libraries(jleGraphics PRIVATE ${CMAKE_DL_LIBS})
endif ()

add_definitions(-DRMLUI_STATIC_LIB)
set(BUILD_SAMPLES OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(FREETYPE_INCLUDE_DIRS ../../core/3rdparty/freetype)
set(FREETYPE_LIBRARY freetype)

add_subdirectory(3rdparty/git_submodules/jle_RmlUi)
if (WIN32)
    target_link_libraries(jleGraphics PRIVATE shlwapi imm32) # shlwapi and imm32 needed for "shell"
endif ()
target_include_directories(jleGraphics PUBLIC 3rdparty/git_submodules/jle_RmlUi/Include RmlUiImplementation RmlUiImplementation/shell/include)
target_link_libraries(jleGraphics PUBLIC RmlCore RmlDebugger)

target_include_directories(jleGraphics PUBLIC "${JLE_3RDPARTY_DIRECTORY}/git_submodules_forks/jle_RmlUi/Include" "RmlUiImplementation" "RmlUiImplementation/shell/include")
