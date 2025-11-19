# Copyright (c) 2022. Johan Lind

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

project(SoLoud)

add_library(SoLoud STATIC
        "core/soloud.cpp"
        "core/soloud_audiosource.cpp"
        "core/soloud_bus.cpp"
        "core/soloud_core_3d.cpp"
        "core/soloud_core_basicops.cpp"
        "core/soloud_core_faderops.cpp"
        "core/soloud_core_filterops.cpp"
        "core/soloud_core_getters.cpp"
        "core/soloud_core_setters.cpp"
        "core/soloud_core_voicegroup.cpp"
        "core/soloud_core_voiceops.cpp"
        "core/soloud_fader.cpp"
        "core/soloud_fft.cpp"
        "core/soloud_fft_lut.cpp"
        "core/soloud_file.cpp"
        "core/soloud_filter.cpp"
        "core/soloud_misc.cpp"
        "core/soloud_queue.cpp"
        "core/soloud_thread.cpp"
        "audiosource/wav/dr_impl.cpp"
        "audiosource/wav/soloud_wav.cpp"
        "audiosource/wav/soloud_wavstream.cpp"
        "audiosource/wav/stb_vorbis.c"
        "backend/miniaudio/soloud_miniaudio.cpp")

# Use MiniAudio as the audio backend
add_compile_definitions(WITH_MINIAUDIO)

target_include_directories(SoLoud SYSTEM PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include")