// Copyright (c) 2023. Johan Lind

#ifndef JLE_SCENE
#define JLE_SCENE

#include <memory>
#include <vector>

#include "jleJson.h"
#include "jlePath.h"
#include <json.hpp>

// #include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

class jleObject;

class jleScene : public jleJsonInterface<nlohmann::json>, public std::enable_shared_from_this<jleScene>
{
public:
    jleScene();

    explicit jleScene(const std::string &sceneName);

    virtual ~jleScene() = default;

    template <class Archive>
    void serialize(Archive &archive);

    template <typename T>
    std::shared_ptr<T> spawnObject();

    std::shared_ptr<jleObject> spawnObject(const std::string &objName);

    std::shared_ptr<jleObject> spawnObject(const nlohmann::json &j_in);

    std::shared_ptr<jleObject> spawnTemplateObject(const jleRelativePath &templatePath);

    void updateSceneObjects(float dt);

    void updateSceneObejctsEditor(float dt);

    void processNewSceneObjects();

    void startObjects();

    static std::shared_ptr<jleScene> loadScene(const std::string &scenePath);

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

    void toJson(nlohmann::json &j_out) override;

    void fromJson(const nlohmann::json &j_in) override;

    bool bPendingSceneDestruction = false;

    std::vector<std::shared_ptr<jleObject>> &sceneObjects();

    std::string sceneName;

    uint32_t getNextInstanceId();

protected:
    friend class jleObject;

    std::vector<std::shared_ptr<jleObject>> _sceneObjects;
    std::vector<std::shared_ptr<jleObject>> _newSceneObjects;

    friend void to_json(nlohmann::json &j, jleScene &s);

    friend void from_json(const nlohmann::json &j, jleScene &s);

private:
    void startObject(jleObject *o);

    static int _scenesCreatedCount;

    uint32_t _objectsInstantiatedCounter{0};

    void configurateSpawnedObject(const std::shared_ptr<jleObject> &obj);
};

void to_json(nlohmann::json &j, jleScene &s);

void from_json(const nlohmann::json &j, jleScene &s);

#include "jleScene.inl"

#endif