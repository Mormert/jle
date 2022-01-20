# Hexablo

Hexablo is a work-in-progress game inspired by the Diablo game franchise, developed together with the jle engine.

![Game Showcase](https://mormert.com/eajdtho89a375kajdhfjh388835ihujage/FullScene.png)

## Build status
[![CMake-Linux](https://github.com/Mormert/hexablo/actions/workflows/cmake-linux.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-linux.yml)
[![CMake-MacOS](https://github.com/Mormert/hexablo/actions/workflows/cmake-macos.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-macos.yml)
[![CMake-Windows](https://github.com/Mormert/hexablo/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/Mormert/hexablo/actions/workflows/cmake-windows.yml)

# Engine (jle)

The engine jle (pronounced as "jile") is written in C++, uses OpenGL 3.3 and holds the ability to build games for Windows, Mac and Linux. It's primarily thought to be used for games with low resolution graphics, where the pixels are clearly visible.

## Libraries used
* GLFW3
* GLAD
* stb_image
* Dear ImGui
* OpenGL Mathematics (GLM)

## Install dependencies on Ubuntu
```shell
sudo apt update
sudo apt install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev wayland-protocols
sudo apt install libwayland-dev libxkbcommon-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev
```
