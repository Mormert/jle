<div align="center">
    <h1>Cross-Platform 3D Game Engine</h1>
    <img src="https://mormert.com/jle-media/jetlaggedGitHub.png" width=40% alt="jle logo" />
    <h1></h1>
  <p>A simple, yet powerful custom-made C++ game engine intended for PC games.<br>
    The engine is developed alongside an <i>unannounced game</i>.</p>
</div>

<div align="center">
    <img src="https://mormert.com/jle-media/jleSponza.png" width=90% alt="Editor Showcase" />
</div>

## Build Status
[![CMake-Linux](https://github.com/Mormert/hexablo/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-linux.yml)
[![CMake-MacOS](https://github.com/Mormert/hexablo/actions/workflows/cmake-macos.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-macos.yml)
[![CMake-Windows](https://github.com/Mormert/hexablo/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-windows.yml)
[![CMake-Wasm](https://github.com/Mormert/hexablo/actions/workflows/cmake-wasm.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-wasm.yml)

# Engine: _jle_

The engine is written in C++ 17 and currently runs OpenGL ES 3.0, OpenGL Core 3.3 or WebGL 2. It thus has the ability to build games targeting Windows, Mac and Linux, as well as modern browsers using WebAssembly via Emscripten.

The way of working in the engine is inspired by a mix of Unreal Engine and Unity, with scripting support in Lua.

Watch the showcase video on [YouTube](https://youtu.be/2GiqLXTfKg4/).

# Third Party
The engine uses a set of libraries for certain specific tasks.
| Usage      | Library |
| :----:       | :----: |
| Window management & input      | [GLFW3](https://www.glfw.org/) (ZLib/LibPNG)        |
| OpenGL loading (desktop)   | [GLAD](https://github.com/dav1dde/glad-web) (CC0)             |
| Image loading   | [stb_image](https://github.com/nothings/stb) (MIT)                  |
| Serialization   | [nlohmann::json](https://github.com/nlohmann/json) (MIT), [cereal](https://github.com/USCiLab/cereal/) (BSD), [magic_enum](https://github.com/Neargye/magic_enum) (MIT) |
| 3D importing    | [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) (MIT), [Assimp](https://github.com/assimp/assimp) (Modified BSD)      |
| Font importing   | [FreeType 2](https://freetype.org/) (FTL or GPL)                |
| Networking   | [Socket.IO C++ Client](https://github.com/socketio/socket.io-client-cpp) (MIT)           |
| Audio   | [SoLoud](https://solhsa.com/soloud/) (ZLib/LibPNG)                             |
| Editor UI   | [Dear ImGui](https://github.com/ocornut/imgui) (MIT), [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) (MIT), [ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit) (MIT) |
| In-Game UI | [RmlUI](https://github.com/Mormert/jle_rmlui) (MIT) |
| Logging   | [Plog](https://github.com/SergiusTheBest/plog) (MIT)                             |
| Mathematics   | [GLM](https://github.com/g-truc/glm) (MIT)                          |
| Compression   | [Zlib](https://www.zlib.net/) (Zlib)                           |
| Multi-threading   | [ConcurrentQueue](https://github.com/cameron314/concurrentqueue) (~BSD), [wiJobSystem](https://github.com/turanszkij/JobSystem) (MIT) |
| Scripting (Lua) | [Lua 5.4](https://www.lua.org/source/5.4/) (MIT), [sol2 v3.0](https://github.com/ThePhD/sol2) (MIT) |
| Physics (3D) | [Bullet3](https://github.com/Mormert/jle_bullet3) (Zlib) |
| Template Library | [EASTL](https://github.com/mormert/jle_EASTL) (BSD-3) |
| Real-Time Profiling | [Tracy](https://github.com/wolfpld/tracy) (BSD-3) |

# Development
CMake 3.20+ is used as the build system.
Supported compilers are MSVC, GCC, Clang and Emscripten.

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
