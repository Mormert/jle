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

#pragma once

#ifdef JLE_BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"

class jleObject;
class jleScene;

class jleEditorSceneObjectsWindow : public jleEditorWindowInterface
{
public:
    explicit jleEditorSceneObjectsWindow(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    std::weak_ptr<jleObject> &GetSelectedObject();

    void SetSelectedObject(std::shared_ptr<jleObject> object);

    std::weak_ptr<jleScene> &GetSelectedScene();

private:

    // Using a static weak_ptr here so that it won't impact deletion
    std::weak_ptr<jleObject> selectedObject;
    std::weak_ptr<jleScene> selectedScene;

    void objectTreeRecursive(std::shared_ptr<jleObject> object);
};

#endif // JLE_BUILD_EDITOR
