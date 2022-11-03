# Cross-Platform 2D & 3D Game Engine

A simple, yet powerful custom-made C++ game engine intended for PC games. It is developed alongside an
old-school-inspired multiplayer Action-RPG, similar to games like Diablo II and Titan Quest.

![Game Showcase](https://mormert.com/jle-media/jle-logo-crop.png)

## Build status

[![CMake-Linux](https://github.com/Mormert/hexablo/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-linux.yml)
[![CMake-MacOS](https://github.com/Mormert/hexablo/actions/workflows/cmake-macos.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-macos.yml)
[![CMake-Windows](https://github.com/Mormert/hexablo/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-windows.yml)
[![CMake-Wasm](https://github.com/Mormert/hexablo/actions/workflows/cmake-wasm.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-wasm.yml)

# Engine: jle

The game engine jle is written in C++, uses OpenGL ES 3.0 or OpenGL Core 3.3. It has the ability to build games
targeting Windows, Mac and Linux, as well as browsers using WebAssembly via Emscripten and WebGL 2.

### Feature overview

- Runs on Windows, Linux, Mac and WebGL-enabled browsers
- Pixel perfect 2D rendering
- Isometric 2.5D rendering
- Pixel perfect font rendering
- C++ Scripting for objects & components
- Resource management
- Included audio engine
- Runs seamlessly in the editor

## The Editor

Games made with jle can be started as standalone programs, or in the jle editor. In the editor, the user can compose
scenes, object hierarchies, create object templates, modify data on objects and components in the scene. All of this can
be done during the runtime of the game.

![Editor Showcase](https://mormert.com/hexablo-press/jle_editor_demo.png)

### Feature overview
- Edit any object or component in runtime
- Construct scenes with object hierarchies
- Object templates (similar to Unity prefabs)
- Content browser with all game resources
- Game and engine settings

# Third Party
Everything is included, and used by jle:
* GLFW3
* GLAD
* stb_image
* JSON for Modern C++
* SoLoud Audio Engine
* Plog logging library
* Dear ImGui (docking branch)
* FreeType 2
* OpenGL Mathematics (GLM)

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
git clone https://github.com/Mormert/hexablo.git --recurse-submodules
```

## Building with CMake
```shell
mkdir build
cd build
cmake .. -DBUILD_EDITOR=ON -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
