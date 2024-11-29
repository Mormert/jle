
add_library(jleAudio STATIC
        jleAudio.cpp)

add_subdirectory(3rdparty/SoLoud)
target_link_libraries(jleAudio PUBLIC SoLoud)