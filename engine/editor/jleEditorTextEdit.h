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

#ifndef JLEEDITORTEXTEDIT_H
#define JLEEDITORTEXTEDIT_H

#include "core/jleCommon.h"

#if JLE_BUILD_EDITOR


#include "3rdparty/ImGuiColorTextEdit/TextEditor.h"
#include "jleEditorImGuiWindowInterface.h"
#include <core/jleResourceRef.h>

#include <unordered_map>
#include <memory>

class jleEditorTextEdit : public jleEditorWindowInterface
{
public:
    explicit jleEditorTextEdit(const std::string &window_name);

    void renderUI();

    void open(const jlePath &path);

    void reloadIfOpened(const jlePath& path);

private:
    std::unordered_map<jlePath, std::unique_ptr<TextEditor>> _textEditorsMap;

    ImFont* font;
};

#endif // JLE_BUILD_EDITOR

#endif // JLEEDITORTEXTEDIT_H
