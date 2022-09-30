// Copyright (c) 2022. Johan Lind

#pragma once

using InterfaceWindowPtr = void *;

class jleWindowInitializerInterface {
public:
    virtual ~jleWindowInitializerInterface() = default;
    ;

    virtual InterfaceWindowPtr InitWindow(int width,
                                          int height,
                                          const char *title) = 0;
};