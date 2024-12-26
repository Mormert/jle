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

//#include "jleComponent.h"
//#include "jleObject.h"
//
//void
//jleComponent::destroy(jleEngineUpdateContext & ctx)
//{
//    _attachedToObject->destroyComponent(this, ctx);
//}
//
//jleTransform &
//jleComponent::getTransform()
//{
//    return _attachedToObject->getTransform();
//}
//
//jleObject *
//jleComponent::object()
//{
//    return _attachedToObject;
//}
//
//std::shared_ptr<jleObject>
//jleComponent::getObjectSharedPtr()
//{
//    return std::static_pointer_cast<jleObject>(_attachedToObject->shared_from_this());
//}
//
//jleScene *
//jleComponent::scene()
//{
//    return _containedInScene;
//}
//
//bool
//jleComponent::isDestroyed()
//{
//    return _isDestroyed;
//}
//
//int
//jleComponent::parallelUpdateBatchSize()
//{
//    return _parallelUpdateBatchSize;
//}
//
//bool
//jleComponent::parallelUpdateEnabled()
//{
//    return _enableParallelUpdate;
//}
//
//void
//jleComponent::enableParallelUpdate(int batchSize)
//{
//    _enableParallelUpdate = true;
//    _parallelUpdateBatchSize = batchSize;
//}
//
