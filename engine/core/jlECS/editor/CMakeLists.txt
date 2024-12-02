target_sources(jleEditor PRIVATE
        "src/jlECSEditor.cpp")

target_include_directories(jleEditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/public/jlECSEditor")
target_include_directories(jleEditor SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")