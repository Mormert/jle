add_library(jlECS_Editor STATIC
        "src/jlECSEditor.cpp")

target_include_directories(jlECS_Editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/public/jlECSEditor")

target_include_directories(jlECS_Editor SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")