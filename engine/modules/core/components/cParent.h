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

class cParent{
public:
    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_parentIndex),
           CEREAL_NVP(_recycleCounter));
    }

    [[nodiscard]] uint16_t getParentIndex() const{ return _parentIndex; }
    jlECS::ObjectRef getParentRef(jlECS::ECS& ecs) { return jlECS::ObjectRef{_parentIndex, _recycleCounter, &ecs}; }

    void setParent(jlECS::ObjectRef& parentRef) {
        _parentIndex = parentRef.objectIndex();
        _recycleCounter = parentRef.recycleCounter();
    }
private:
    uint16_t _parentIndex;
    uint16_t _recycleCounter;
};