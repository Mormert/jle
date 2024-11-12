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

#include "jleBuildConfig.h"

#if JLE_BUILD_EDITOR

#include "core/jlECS/jlECS.h"
#include "jleEditorImGuiWindowInterface.h"
#include <modules/jleEditorModulesContext.h>
#include <optional>

namespace jlECS
{
class ECS;
}

class jleECSEditorWindow : public jleEditorWindowInterface
{
public:
    explicit jleECSEditorWindow(const std::string &window_name);

    void renderUI(jleEditorModulesContext &ctx);

    void update(jleResourceHolder &resourcesModule);

private:
    std::optional<jlECS::ObjectRef> _selectedObject;
    jlECS::ECS *_ecs;
};

#endif // JLE_BUILD_EDITOR
