// Copyright (c) 2022. Johan Lind

#ifndef JLE_TRANSFORM
#define JLE_TRANSFORM


#include <glm/glm.hpp>
#include "jleExternalSerialization.h"

class jleObject;

class jleTransform
{
public:
    explicit jleTransform(jleObject *owner);

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_local));
    }

    void setWorldPosition(const glm::vec3 &position);

    void setWorldMatrix(const glm::mat4 &matrix);

    void setLocalPosition(const glm::vec3 &position);

    void addLocalTranslation(const glm::vec3& position);

    [[nodiscard]] glm::vec3 getLocalPosition();

    [[nodiscard]] glm::vec3 getWorldPosition();

    [[nodiscard]] const glm::mat4 &getWorldMatrix();

    [[nodiscard]] const glm::mat4 &getLocalMatrix();

private:
    friend class jleObject;
    const glm::mat4 &getParentWorld();

    void propagateMatrix();

    jleObject *_owner;

    glm::mat4 _local{1.f};
    glm::mat4 _world{1.f};
};

#endif // JLE_TRANSFORM