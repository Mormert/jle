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

#ifndef JLE_SCENE
#define JLE_SCENE

#include <memory>
#include <vector>

#include "jlePath.h"
#include "jleResource.h"
#include "jleSerializedResource.h"
#include "jleTypeReflectionUtils.h"

#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

class jleObject;

class jleScene : public jleSerializedOnlyResource, public std::enable_shared_from_this<jleScene>
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleScene, "scn")

    jleScene();

    void
    saveToFile() override
    {
        if (path.isEmpty()) {
            path = jlePath{"GR:scenes/" + sceneName + getDotPrimaryFileExtension()};
        }
        std::ofstream save{path.getRealPath()};
        cereal::JSONOutputArchive outputArchive(save);
        std::shared_ptr<jleSerializedOnlyResource> thiz = shared_from_this();
        outputArchive(thiz);
    };

    explicit jleScene(const std::string &sceneName);

    ~jleScene() override = default;

    template <class Archive>
    void serialize(Archive &archive);

    template <typename T>
    std::shared_ptr<T> spawnObject();

    std::shared_ptr<jleObject> spawnObjectFromTemplate(const jlePath& path);

    // Spawn a jleObject derived class, similar to spawnObject<>().
    std::shared_ptr<jleObject> spawnObjectTypeByName(const std::string &objName);

    // Spawn a generic jleObject, with specified name
    std::shared_ptr<jleObject> spawnObjectWithName(const std::string &name);

    void spawnObject(const std::shared_ptr<jleObject> &object);

    void startObjects();

    virtual void updateScene(float dt);

    virtual void updateSceneEditor(float dt);

    virtual void onSceneStart();

    virtual void onSceneDestruction();

    virtual void sceneInspectorImGuiRender();

    void destroyScene();

    bool bPendingSceneDestruction = false;

    std::vector<std::shared_ptr<jleObject>> &sceneObjects();

    std::string sceneName;

protected:
    friend class jleObject;

    std::vector<std::shared_ptr<jleObject>> _sceneObjects;
    std::vector<std::shared_ptr<jleObject>> _newSceneObjects;

    virtual void setupObject(const std::shared_ptr<jleObject> &obj);

    void processNewSceneObjects();

private:
    void updateSceneObjects(float dt);

    void updateSceneObjectsEditor(float dt);

    void startObject(jleObject *o);

    static int _scenesCreatedCount;
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleScene)

CEREAL_REGISTER_TYPE(jleScene)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleScene)

#include "jleScene.inl"

#endif