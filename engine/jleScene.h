// Copyright (c) 2023. Johan Lind

#ifndef JLE_SCENE
#define JLE_SCENE

#include <memory>
#include <vector>

#include "jleTypeReflectionUtils.h"
#include "jlePath.h"
#include "jleSerializedResource.h"
#include "jleResource.h"

#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

class jleObject;

class jleScene : public jleSerializedResource, public std::enable_shared_from_this<jleScene>
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleScene, scn)

    jleScene();

    void
    saveToFile() override
    {
        if (filepath.empty()) {
            jlePath path{"GR:scenes/" + sceneName + getDotFileExtension()};
            filepath = path.getRealPath();
        }
        std::ofstream save{filepath};
        cereal::JSONOutputArchive outputArchive(save);
        std::shared_ptr<jleSerializedResource> thiz = shared_from_this();
        outputArchive(thiz);
    };

    explicit jleScene(const std::string &sceneName);

    ~jleScene() override = default;

    template <class Archive>
    void serialize(Archive &archive);

    template <typename T>
    std::shared_ptr<T> spawnObject();

    std::shared_ptr<jleObject> spawnObject(const std::string &objName);

    void spawnObject(std::shared_ptr<jleObject> object);

    void updateSceneObjects(float dt);

    void updateSceneObejctsEditor(float dt);

    void processNewSceneObjects();

    void startObjects();

    void saveScene();

    virtual void
    updateScene()
    {
    }

    virtual void
    updateSceneEditor()
    {
    }

    virtual void
    onSceneCreation()
    {
    }

    virtual void
    onSceneDestruction()
    {
    }

    void destroyScene();

    bool bPendingSceneDestruction = false;

    std::vector<std::shared_ptr<jleObject>> &sceneObjects();

    std::string sceneName;

    uint32_t getNextInstanceId();

protected:
    friend class jleObject;

    std::vector<std::shared_ptr<jleObject>> _sceneObjects;
    std::vector<std::shared_ptr<jleObject>> _newSceneObjects;

private:
    void startObject(jleObject *o);

    static int _scenesCreatedCount;

    uint32_t _objectsInstantiatedCounter{0};

    void configurateSpawnedObject(const std::shared_ptr<jleObject> &obj);
};

CEREAL_REGISTER_TYPE(jleScene)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedResource, jleScene)

#include "jleScene.inl"

#endif