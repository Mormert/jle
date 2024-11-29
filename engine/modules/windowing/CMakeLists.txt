add_library(jleWindowing STATIC
        jleWindow.cpp
)

if (NOT JLE_BUILD_EMSCRIPTEN) # Native build, excluding some Emscripten-embedded libraries
    # Remove GLFW tests, examples and docs from build
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(3rdparty/git_submodules/glfw)
    target_link_libraries(jleWindowing PUBLIC glfw)

    target_include_directories(jleWindowing PUBLIC SYSTEM 3rdparty/git_submodules/glfw/include)
endif ()