// Copyright (c) 2022. Johan Lind

#pragma once

using InterfaceWindowPtr = void *;

class iWindowInitializer {
public:
    virtual ~iWindowInitializer() = default;;

    virtual InterfaceWindowPtr InitWindow(int width, int height, const char *title) = 0;
};