add_library(jleWindowing STATIC
        jleWindow.cpp
)

if (NOT JLE_BUILD_EMSCRIPTEN) # Native build, excluding some Emscripten-embedded libraries
    include(FetchContent)

    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG        3.4
    )

    # Remove GLFW tests, examples and docs from build
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(glfw)

    target_link_libraries(jleWindowing PUBLIC glfw)
    target_include_directories(jleWindowing PUBLIC SYSTEM "${glfw_SOURCE_DIR}/include")
endif ()