/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleInputModuleEditor.h"

#include <ImGui/imgui.h>

bool
jleInputModuleEditor::isKeyPressed(jleKey key)
{
    if (!_isInputEnabled || !_gameWindowFocused) {
        return false;
    }
    
    int imguiKey = jleKeyToImGuiKey(key);
    return imguiKey != -1 && ImGui::IsKeyPressed(imguiKey);
}

bool
jleInputModuleEditor::isKeyReleased(jleKey key)
{
    if (!_isInputEnabled || !_gameWindowFocused) {
        return false;
    }
    
    int imguiKey = jleKeyToImGuiKey(key);
    return imguiKey != -1 && ImGui::IsKeyReleased(imguiKey);
}

bool
jleInputModuleEditor::isKeyDown(jleKey key)
{
    if (!_isInputEnabled || !_gameWindowFocused) {
        return false;
    }
    
    int imguiKey = jleKeyToImGuiKey(key);
    return imguiKey != -1 && ImGui::IsKeyDown(imguiKey);
}

int32_t
jleInputModuleEditor::getMouseX()
{
    auto mousePos = ImGui::GetMousePos();
    return static_cast<int32_t>(mousePos.x - _screenBeginX);
}

int32_t
jleInputModuleEditor::getMouseY()
{
    auto mousePos = ImGui::GetMousePos();
    return static_cast<int32_t>(mousePos.y - _screenBeginY);
}

float
jleInputModuleEditor::getScrollX()
{
    auto& io = ImGui::GetIO();
    return io.MouseWheelH;
}

float
jleInputModuleEditor::getScrollY()
{
    auto& io = ImGui::GetIO();
    return io.MouseWheel;
}

bool
jleInputModuleEditor::getMouseClick(jleButton button)
{
    if (!_isInputEnabled || !_gameWindowFocused) {
        return false;
    }
    
    int imguiButton = jleButtonToImGuiButton(button);
    return imguiButton != -1 && ImGui::IsMouseClicked(imguiButton);
}

int
jleInputModuleEditor::jleKeyToImGuiKey(jleKey key)
{
    switch (key) {
        case jleKey::SPACE: return ImGuiKey_Space;
        case jleKey::APOSTROPHE: return ImGuiKey_Apostrophe;
        case jleKey::COMMA: return ImGuiKey_Comma;
        case jleKey::MINUS: return ImGuiKey_Minus;
        case jleKey::PERIOD: return ImGuiKey_Period;
        case jleKey::SLASH: return ImGuiKey_Slash;
        case jleKey::NUM_0: return ImGuiKey_0;
        case jleKey::NUM_1: return ImGuiKey_1;
        case jleKey::NUM_2: return ImGuiKey_2;
        case jleKey::NUM_3: return ImGuiKey_3;
        case jleKey::NUM_4: return ImGuiKey_4;
        case jleKey::NUM_5: return ImGuiKey_5;
        case jleKey::NUM_6: return ImGuiKey_6;
        case jleKey::NUM_7: return ImGuiKey_7;
        case jleKey::NUM_8: return ImGuiKey_8;
        case jleKey::NUM_9: return ImGuiKey_9;
        case jleKey::SEMICOLON: return ImGuiKey_Semicolon;
        case jleKey::EQUAL: return ImGuiKey_Equal;
        case jleKey::A: return ImGuiKey_A;
        case jleKey::B: return ImGuiKey_B;
        case jleKey::C: return ImGuiKey_C;
        case jleKey::D: return ImGuiKey_D;
        case jleKey::E: return ImGuiKey_E;
        case jleKey::F: return ImGuiKey_F;
        case jleKey::G: return ImGuiKey_G;
        case jleKey::H: return ImGuiKey_H;
        case jleKey::I: return ImGuiKey_I;
        case jleKey::J: return ImGuiKey_J;
        case jleKey::K: return ImGuiKey_K;
        case jleKey::L: return ImGuiKey_L;
        case jleKey::M: return ImGuiKey_M;
        case jleKey::N: return ImGuiKey_N;
        case jleKey::O: return ImGuiKey_O;
        case jleKey::P: return ImGuiKey_P;
        case jleKey::Q: return ImGuiKey_Q;
        case jleKey::R: return ImGuiKey_R;
        case jleKey::S: return ImGuiKey_S;
        case jleKey::T: return ImGuiKey_T;
        case jleKey::U: return ImGuiKey_U;
        case jleKey::V: return ImGuiKey_V;
        case jleKey::W: return ImGuiKey_W;
        case jleKey::X: return ImGuiKey_X;
        case jleKey::Y: return ImGuiKey_Y;
        case jleKey::Z: return ImGuiKey_Z;
        case jleKey::LEFT_BRACKET: return ImGuiKey_LeftBracket;
        case jleKey::BACKSLASH: return ImGuiKey_Backslash;
        case jleKey::RIGHT_BRACKET: return ImGuiKey_RightBracket;
        case jleKey::GRAVE_ACCENT: return ImGuiKey_GraveAccent;
        case jleKey::ESCAPE: return ImGuiKey_Escape;
        case jleKey::ENTER: return ImGuiKey_Enter;
        case jleKey::TAB: return ImGuiKey_Tab;
        case jleKey::BACKSPACE: return ImGuiKey_Backspace;
        case jleKey::INSERT: return ImGuiKey_Insert;
        case jleKey::DEL: return ImGuiKey_Delete;
        case jleKey::RIGHT: return ImGuiKey_RightArrow;
        case jleKey::LEFT: return ImGuiKey_LeftArrow;
        case jleKey::DOWN: return ImGuiKey_DownArrow;
        case jleKey::UP: return ImGuiKey_UpArrow;
        case jleKey::PAGE_UP: return ImGuiKey_PageUp;
        case jleKey::PAGE_DOWN: return ImGuiKey_PageDown;
        case jleKey::HOME: return ImGuiKey_Home;
        case jleKey::END: return ImGuiKey_End;
        case jleKey::CAPS_LOCK: return ImGuiKey_CapsLock;
        case jleKey::SCROLL_LOCK: return ImGuiKey_ScrollLock;
        case jleKey::NUM_LOCK: return ImGuiKey_NumLock;
        case jleKey::PRINT_SCREEN: return ImGuiKey_PrintScreen;
        case jleKey::PAUSE: return ImGuiKey_Pause;
        case jleKey::F1: return ImGuiKey_F1;
        case jleKey::F2: return ImGuiKey_F2;
        case jleKey::F3: return ImGuiKey_F3;
        case jleKey::F4: return ImGuiKey_F4;
        case jleKey::F5: return ImGuiKey_F5;
        case jleKey::F6: return ImGuiKey_F6;
        case jleKey::F7: return ImGuiKey_F7;
        case jleKey::F8: return ImGuiKey_F8;
        case jleKey::F9: return ImGuiKey_F9;
        case jleKey::F10: return ImGuiKey_F10;
        case jleKey::F11: return ImGuiKey_F11;
        case jleKey::F12: return ImGuiKey_F12;
        case jleKey::KP_0: return ImGuiKey_Keypad0;
        case jleKey::KP_1: return ImGuiKey_Keypad1;
        case jleKey::KP_2: return ImGuiKey_Keypad2;
        case jleKey::KP_3: return ImGuiKey_Keypad3;
        case jleKey::KP_4: return ImGuiKey_Keypad4;
        case jleKey::KP_5: return ImGuiKey_Keypad5;
        case jleKey::KP_6: return ImGuiKey_Keypad6;
        case jleKey::KP_7: return ImGuiKey_Keypad7;
        case jleKey::KP_8: return ImGuiKey_Keypad8;
        case jleKey::KP_9: return ImGuiKey_Keypad9;
        case jleKey::KP_DECIMAL: return ImGuiKey_KeypadDecimal;
        case jleKey::KP_DIVIDE: return ImGuiKey_KeypadDivide;
        case jleKey::KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case jleKey::KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
        case jleKey::KP_ADD: return ImGuiKey_KeypadAdd;
        case jleKey::KP_ENTER: return ImGuiKey_KeypadEnter;
        case jleKey::KP_EQUAL: return ImGuiKey_KeypadEqual;
        case jleKey::LEFT_SHIFT: return ImGuiKey_LeftShift;
        case jleKey::LEFT_CONTROL: return ImGuiKey_LeftCtrl;
        case jleKey::LEFT_ALT: return ImGuiKey_LeftAlt;
        case jleKey::LEFT_SUPER: return ImGuiKey_LeftSuper;
        case jleKey::RIGHT_SHIFT: return ImGuiKey_RightShift;
        case jleKey::RIGHT_CONTROL: return ImGuiKey_RightCtrl;
        case jleKey::RIGHT_ALT: return ImGuiKey_RightAlt;
        case jleKey::RIGHT_SUPER: return ImGuiKey_RightSuper;
        case jleKey::MENU: return ImGuiKey_Menu;
        default: return -1;
    }
}

int
jleInputModuleEditor::jleButtonToImGuiButton(jleButton button)
{
    switch (button) {
        case jleButton::BUTTON_LEFT: return ImGuiMouseButton_Left;
        case jleButton::BUTTON_RIGHT: return ImGuiMouseButton_Right;
        case jleButton::BUTTON_MIDDLE: return ImGuiMouseButton_Middle;
        case jleButton::BUTTON_4: return 3; // ImGui supports up to 5 buttons (0-4)
        case jleButton::BUTTON_5: return 4;
        default: return -1;
    }
}