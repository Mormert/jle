// Copyright (c) 2023. Johan Lind

#pragma once

class jleEngineStatus {
public:
    static int fps();

    static double deltaTime();

    static double time();

private:
    static int _fps;
    static double _deltaTime;
    static double _currentFrame;
    static double _lastFrame;

    static void updateEngineStatus();

    friend class jleCore;
};
