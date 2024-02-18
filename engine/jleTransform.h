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

#ifndef JLE_TRANSFORM
#define JLE_TRANSFORM

#include "jleCommon.h"

#include "jleCompileHelper.h"
#include "jleExternalSerialization.h"
#include <glm/glm.hpp>

class jleObject;

class jleTransform
{
public:
    explicit jleTransform(jleObject *owner);

    template <class Archive>
    void serialize(Archive &ar);

    void setWorldPosition(const glm::vec3 &position);

    void setWorldMatrix(const glm::mat4 &matrix);

    void setLocalPosition(const glm::vec3 &position);

    void setLocalMatrix(const glm::mat4 &matrix);

    void addLocalTranslation(const glm::vec3 &position);

    void removeRotations();

    void rotateTowardsPoint(const glm::vec3 &position, const glm::vec3 &up = {0.0f, 1.0f, 0.0f});

    glm::vec3 getForward();

    glm::vec3 getUp();

    glm::vec3 getRight();

    [[nodiscard]] glm::vec3 getLocalPosition();

    [[nodiscard]] glm::vec3 getWorldPosition();

    [[nodiscard]] const glm::mat4 &getWorldMatrix();

    [[nodiscard]] const glm::mat4 &getLocalMatrix();

private:
    friend class jleObject;
    const glm::mat4 &getParentWorld();

    void propagateMatrixFromObjectSerialization();

    void propagateMatrix();
    void propagateMatrixChildren();

    jleObject *_owner;

    glm::mat4 _local{1.f};
    glm::mat4 _world{1.f};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleTransform)

#endif // JLE_TRANSFORM