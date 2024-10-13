add_library(jleWindowing STATIC
        jleWindow.cpp
)

if (NOT JLE_BUILD_EMSCRIPTEN) # Native build, excluding some Emscripten-embedded libraries
    target_link_libraries(jleWindowing PUBLIC ${CMAKE_DL_LIBS})

    # Compile glad.c only when building native
    target_sources(jleWindowing PRIVATE "${JLE_3RDPARTY_DIRECTORY}/glad/glad.cpp")

    # Remove GLFW tests, examples and docs from build
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(${JLE_3RDPARTY_DIRECTORY}/git_submodules/glfw "${CMAKE_CURRENT_BINARY_DIR}/glfw")
    target_link_libraries(jleWindowing PUBLIC glfw)

    target_include_directories(jleWindowing PUBLIC SYSTEM ${JLE_3RDPARTY_DIRECTORY}/git_submodules/glfw/include)
endif ()