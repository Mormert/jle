// Copyright (c) 2023. Johan Lind

#ifndef JLE_SCENE
#define JLE_SCENE

#include <memory>
#include <vector>

#include "jlePath.h"
#include "jleResourceInterface.h"
#include <json.hpp>
#include "jleResource.h"

#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

class jleObject;

class jleScene : public jleResourceInterface, public std::enable_shared_from_this<jleScene>
{
public:
    jleScene();

    void
    saveToFile() override
    {
        if (filepath.empty()) {
            jlePath path{"GR:scenes/" + sceneName + ".scn"};
            filepath = path.getRealPath();
        }
        std::ofstream save{filepath};
        cereal::JSONOutputArchive outputArchive(save);
        std::shared_ptr<jleResourceInterface> thiz = shared_from_this();
        outputArchive(thiz);
    };

    jleLoadFromFileSuccessCode
    loadFromFile(const jlePath &path) override
    {
        return jleLoadFromFileSuccessCode::IMPLEMENT_POLYMORPHIC_CEREAL;
    };

    explicit jleScene(const std::string &sceneName);

    ~jleScene() override = default;

    template <class Archive>
    void serialize(Archive &archive);

    template <typename T>
    std::shared_ptr<T> spawnObject();

    std::shared_ptr<jleObject> spawnObject(const std::string &objName);

    void spawnObject(std::shared_ptr<jleObject> object);

    // std::shared_ptr<jleObject> spawnObject(const nlohmann::json &j_in);

    //  std::shared_ptr<jleObject> spawnTemplateObject(const jlePath &templatePath);

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
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleResourceInterface, jleScene)

#include "jleScene.inl"

#endif