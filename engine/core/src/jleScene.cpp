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
#include "jleObject.h"
#include "jleProfiler.h"
#include "modules/physics/jlePhysics.h"


#include "serialization/jleBinaryArchive.h"
#include "serialization/jleJSONArchive.h"


JLE_EXTERN_TEMPLATE_CEREAL_CPP(jleScene)

int jleScene::_scenesCreatedCount{0};

/*
template <class Archive>
void
jleScene::serialize(Archive &archive)
{
    jleSerializationContext &ctx = archive.ctx;

    archive(CEREAL_NVP(sceneName), CEREAL_NVP(_sceneObjects));

    for (auto &&object : _sceneObjects) {
        // Replace object with template object, if it is based on one
        if (object->__templatePath.has_value()) {
            auto path = object->__templatePath;
            try {

                auto original = ctx.resources->loadResourceFromFileT<jleObject>(object->__templatePath.value(), ctx);

                auto copy = original->duplicateTemplate();
                object = copy;

                object->__templatePath = path;

            } catch (std::exception &e) {
                LOGE << "Failed to load object template: " << e.what();
            }
        }

        object->replaceChildrenWithTemplate(ctx);

        object->propagateOwnedByScene(this, jleObjectNetworkType::SERVER);
    }
}*/

jleScene::
jleScene()
{
    _physics = std::make_unique<jlePhysics>();

    sceneName = "Scene_" + std::to_string(_scenesCreatedCount);
    _scenesCreatedCount++;
}

jleScene::~jleScene() = default;

void
jleScene::updateSceneObjects(jleEngineUpdateContext & ctx)
{
    JLE_SCOPE_PROFILE_CPU(jleScene_updateSceneObjects)
    for (int32_t i = _sceneObjects.size() - 1; i >= 0; i--) {
        if (_sceneObjects[i]->_pendingKill) {
            _sceneObjects[i]->propagateDestroy(ctx);
            _sceneObjects.erase(_sceneObjects.begin() + i);
            continue;
        }

        _sceneObjects[i]->updateComponents(ctx);
        _sceneObjects[i]->updateChildren(ctx);
    }
}

void
jleScene::updateSceneObjectsEditor(jleEngineUpdateContext & ctx)
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
jleScene::processNewSceneObjects(jleEngineUpdateContext & ctx)
{
    JLE_SCOPE_PROFILE_CPU(jleScene_processNewSceneObjects)
    if (!_newSceneObjects.empty()) {
        for (const auto &newObject : _newSceneObjects) {
            if (!newObject->_isStarted) {
                if (!ctx.gameRuntime.isGameKilled()) {
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
jleScene::setupObject(const std::shared_ptr<jleObject> &obj, jleSerializationContext& ctx)
{
    obj->_containedInScene = this;
    obj->_instanceName = std::string{obj->objectNameVirtual()} + "_" + std::to_string(obj->_instanceID);

    obj->replaceChildrenWithTemplate(ctx);
    _newSceneObjects.push_back(obj);
}

void
jleScene::startObjects(jleEngineUpdateContext & ctx)
{
    for (auto &&o : _sceneObjects) {
        startObject(&*o, ctx);
    }
}

void
jleScene::startObject(jleObject *o, jleEngineUpdateContext & ctx)
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
jleScene::spawnObject(const std::shared_ptr<jleObject> &object, jleSerializationContext& ctx)
{
    setupObject(object, ctx);
}

std::shared_ptr<jleObject>
jleScene::spawnObjectFromTemplate(const jlePath &path, jleSerializationContext& ctx)
{
    if (const jleResourceRef<jleObject> templateObject{path, ctx}) {
        std::shared_ptr<jleObject> copyBasedOnTemplate = templateObject->duplicateTemplate();
        copyBasedOnTemplate->__templatePath = path;
        spawnObject(copyBasedOnTemplate, ctx);

        return copyBasedOnTemplate;
    }

    LOGE << "Failed loading template object with path " << path.getVirtualPath();
    return nullptr;
}

std::shared_ptr<jleObject>
jleScene::spawnObjectWithName(const std::string &name, jleSerializationContext& ctx)
{
    auto obj = spawnObject<jleObject>(ctx);
    obj->_instanceName = name;
    return obj;
}

void
jleScene::updateScene(jleEngineUpdateContext & ctx)
{
    const auto dt = ctx.frameInfo.getDeltaTime();
    getPhysics().step(dt);

    processNewSceneObjects(ctx);
    updateSceneObjects(ctx);
}

void
jleScene::updateSceneEditor(jleEngineUpdateContext & ctx)
{
    processNewSceneObjects(ctx);
    updateSceneObjectsEditor(ctx);
}

void
jleScene::onSceneStart(jleEngineUpdateContext & ctx)
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
