// Copyright (c) 2023. Johan Lind


#include "jleInput.h"
#include "jleKeyboardInput.h"
#include "jleMouseInput.h"

jleInput::jleInput(const std::shared_ptr<jleWindow> &window)
    : keyboard{std::make_unique<jleKeyboardInput>(window)}, mouse{std::make_unique<jleMouseInput>(window)}
{
}

void
jleInput::setInputEnabled(bool isEnabled)
{
    _isInputEnabled = isEnabled;
    keyboard->isEnabled(isEnabled);
    mouse->isEnabled(isEnabled);
}

bool
jleInput::getInputEnabled() const
{
    return _isInputEnabled;
}
