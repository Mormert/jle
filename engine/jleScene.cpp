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

#include "jleScene.h"
#include "jleGameEngine.h"
#include "jleObject.h"
#include "jleProfiler.h"
#include "jlePhysics.h"

#include <filesystem>
#include <iostream>

JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleScene)

int jleScene::_scenesCreatedCount{0};

jleScene::
jleScene()
{
    _physics = std::make_unique<jlePhysics>();

    sceneName = "Scene_" + std::to_string(_scenesCreatedCount);
    _scenesCreatedCount++;
}

jleScene::~jleScene() = default;

void
jleScene::updateSceneObjects(jleEngineModulesContext& ctx)
{
    JLE_SCOPE_PROFILE_CPU(jleScene_updateSceneObjects)
    for (int32_t i = _sceneObjects.size() - 1; i >= 0; i--) {
        if (_sceneObjects[i]->_pendingKill) {
            _sceneObjects[i]->propagateDestroy();
            _sceneObjects.erase(_sceneObjects.begin() + i);
            continue;
        }

        _sceneObjects[i]->updateComponents(ctx);
        _sceneObjects[i]->updateChildren(ctx);
    }
}

void
jleScene::updateSceneObjectsEditor(jleEngineModulesContext& ctx)
{
    JLE_SCOPE_PROFILE_CPU(jleScene_updateSceneObejctsEditor)
    for (int32_t i = _sceneObjects.size() - 1; i >= 0; i--) {
        if (_sceneObjects[i]->_pendingKill) {
            _sceneObjects.erase(_sceneObjects.begin() + i);
            continue;
        }

        _sceneObjects[i]->updateComponentsEditor(ctx);
        _sceneObjects[i]->updateChildrenEditor(ctx);
    }
}

void
jleScene::processNewSceneObjects(jleEngineModulesContext& ctx)
{
    JLE_SCOPE_PROFILE_CPU(jleScene_processNewSceneObjects)
    if (!_newSceneObjects.empty()) {
        for (const auto &newObject : _newSceneObjects) {
            if (!newObject->_isStarted) {
                if (!gEngine->isGameKilled()) {
                    newObject->startComponents(ctx);
                }
                newObject->_isStarted = true;
            }

            // Only push back objects existing directly in the scene into scene
            // objects The object can be placed as a child object in another
            // object, and thus no longer existing directly in the scene
            if (newObject->_parentObject == nullptr) {
                _sceneObjects.push_back(newObject);
            }
        }

        _newSceneObjects.clear();
    }
}

void
jleScene::destroyScene()
{
    bPendingSceneDestruction = true;
    onSceneDestruction();
}

jlePhysics &
jleScene::getPhysics()
{
    return *_physics;
}

void
jleScene::setupObject(const std::shared_ptr<jleObject> &obj)
{
    obj->_containedInScene = this;
    obj->_instanceName = std::string{obj->objectNameVirtual()} + "_" + std::to_string(obj->_instanceID);

    obj->replaceChildrenWithTemplate();
    _newSceneObjects.push_back(obj);
}

void
jleScene::startObjects(jleEngineModulesContext& ctx)
{
    for (auto &&o : _sceneObjects) {
        startObject(&*o, ctx);
    }
}

void
jleScene::startObject(jleObject *o, jleEngineModulesContext& ctx)
{
    if (!o->_isStarted) {
        o->startComponents(ctx);
        o->_isStarted = true;
        for (auto &&c : o->__childObjects) {
            startObject(&*c, ctx);
        }
    }
}

void
jleScene::spawnObject(const std::shared_ptr<jleObject> &object)
{
    setupObject(object);
}

std::shared_ptr<jleObject>
jleScene::spawnObjectFromTemplate(const jlePath &path, jleResources& resources)
{
    if (const jleResourceRef<jleObject> templateObject{path, resources}) {
        std::shared_ptr<jleObject> copyBasedOnTemplate = templateObject->duplicateTemplate();
        copyBasedOnTemplate->__templatePath = path;
        spawnObject(copyBasedOnTemplate);

        return copyBasedOnTemplate;
    }

    LOGE << "Failed loading template object with path " << path.getVirtualPath();
    return nullptr;
}

std::shared_ptr<jleObject>
jleScene::spawnObjectWithName(const std::string &name)
{
    auto obj = spawnObject<jleObject>();
    obj->_instanceName = name;
    return obj;
}

void
jleScene::updateScene(jleEngineModulesContext& ctx)
{
    const auto dt = ctx.frameInfo.getDeltaTime();
    getPhysics().step(dt);

    processNewSceneObjects(ctx);
    updateSceneObjects(ctx);
}

void
jleScene::updateSceneEditor(jleEngineModulesContext& ctx)
{
    processNewSceneObjects(ctx);
    updateSceneObjectsEditor(ctx);
}

void
jleScene::onSceneStart()
{
}

void
jleScene::onSceneDestruction()
{
}

void
jleScene::sceneInspectorImGuiRender()
{
}
