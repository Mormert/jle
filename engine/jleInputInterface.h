// Copyright (c) 2022. Johan Lind

#pragma once

class jleInputInterface {
public:
    virtual ~jleInputInterface() = default;

    virtual void SetInputEnabled(bool enabled) { input_enabled = enabled; }

    bool GetInputEnabled() const { return input_enabled; }

protected:
    bool input_enabled = true;
};