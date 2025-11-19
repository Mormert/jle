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

#include <core/serialization/jleExternalSerialization.h>

#include "jlECS/jlECS.h"

namespace jleHierarchyFuncs {
    bool setParent(jlECS::ObjectRef& object, std::optional<jlECS::ObjectRef>& newParentOptional);
}

class cParent{
public:
    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_parentIndex),
           CEREAL_NVP(_recycleCounter));
    }

    [[nodiscard]] uint16_t getParentIndex() const{ return _parentIndex; }
    [[nodiscard]] jlECS::ObjectRef getParentRef(jlECS::ECS& ecs) const { return jlECS::ObjectRef{_parentIndex, _recycleCounter, &ecs}; }

private:
    uint16_t _parentIndex = 0;
    uint16_t _recycleCounter = std::numeric_limits<uint16_t>::max();

    friend bool jleHierarchyFuncs::setParent(jlECS::ObjectRef& object, std::optional<jlECS::ObjectRef>& newParentOptional);
};