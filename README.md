# Cross-Platform 2D & 3D Game Engine

A simple, yet powerful custom-made C++ game engine intended for PC games. The engine is developed alongside an unannounced game.

![Engine Logo](https://mormert.com/jle-media/jle-logo-cropped2.png)

## Build status

[![CMake-Linux](https://github.com/Mormert/hexablo/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-linux.yml)
[![CMake-MacOS](https://github.com/Mormert/hexablo/actions/workflows/cmake-macos.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-macos.yml)
[![CMake-Windows](https://github.com/Mormert/hexablo/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-windows.yml)
[![CMake-Wasm](https://github.com/Mormert/hexablo/actions/workflows/cmake-wasm.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-wasm.yml)

# Engine: jle

The engine is written in C++ 17 and currently runs OpenGL ES 3.0, OpenGL Core 3.3 or WebGL 2. It thus has the ability to build games
targeting Windows, Mac and Linux, as well as modern browsers using WebAssembly via Emscripten.

The way of working with the engine is inspired by a mix of Unreal Engine and Unity.

![Editor Showcase](https://mormert.com/jle-media/jle-screengrab.png)

# Third Party
The engine uses a set of libraries for certain specific tasks.
| Usage      | Library |
| :----:       | :----: |
| Window management & input      | GLFW3        |
| OpenGL loading (desktop)   | GLAD             |
| Image loading   | stb_image                   |
| Serialization   | nlohmann::json, cereal      |
| 3D importing    | tinyobjloader, Assimp       |
| Font importing   | FreeType 2                 |
| Networking   | Socket.IO C++ Client           |
| Audio   | SoLoud                              |
| Editor UI   | Dear ImGui, ImGuizmo, ImGuiColorTextEdit  |
| Logging   | Plog                              |
| Mathematics   | GLM                           |
| Real-time profiling   | Remotery              |
| Compression   | Zlib                            |
| Multi-threading   | moodycamel::ConcurrentQueue |

# Development
CMake 3.20+ is used as the build system.
Supported compilers are MSVC, GCC and Clang.

## Linux Dependencies
```shell
sudo apt update
sudo apt install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev wayland-protocols libwayland-dev libxkbcommon-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev
```

## Fetching the Source
No external dependencies needed, everything is included.
```shell
git clone https://github.com/Mormert/jle.git --recurse-submodules
```

## Building with CMake
```shell
mkdir build
cd build
cmake .. -DBUILD_EDITOR=ON -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
