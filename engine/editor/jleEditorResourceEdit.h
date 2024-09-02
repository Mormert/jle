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

#ifndef JLE_EDITORRESOURCEEDIT_H
#define JLE_EDITORRESOURCEEDIT_H

#include "jleBuildConfig.h"

#if JLE_BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"
#include <jleResourceRef.h>

#include <memory>
#include <unordered_map>

class jleSerializationContext;

class jleEditorResourceEdit : public jleEditorWindowInterface
{
public:
    explicit jleEditorResourceEdit(const std::string &window_name);

    void renderUI(jleEditorModulesContext& ctx);

    bool tryOpen(const jlePath &path, jleResources& resources);

private:

    std::unordered_map<jlePath, std::shared_ptr<jleSerializedResource>> _resources;

};

#endif // JLE_BUILD_EDITOR

#endif // JLE_EDITORRESOURCEEDIT_H
