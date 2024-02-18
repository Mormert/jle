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

#include "jleCommon.h"

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/anim.h>

struct jleAnimationKeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct jleAnimationKeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct jleAnimationKeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

class jleAnimationBone
{
public:
    jleAnimationBone(const std::string& name, int id, const aiNodeAnim* channel);

    void update(float time);

    glm::mat4 getLocalTransform();

    [[nodiscard]] const std::string& getName() const;

    int getBoneId();

    int getPositionIndex(float animTime);

    int getRotationIndex(float animTime);

    int getScaleIndex(float animTime);

private:
    std::vector<jleAnimationKeyPosition> _positions;
    std::vector<jleAnimationKeyRotation> _rotations;
    std::vector<jleAnimationKeyScale> _scales;

    glm::mat4 _localTransform;
    std::string _name;
    int _id;

    float getScaleFactor(float lastTime, float nextTime, float animTime);

    glm::mat4 lerpPosition(float animTime);

    glm::mat4 slerpRotation(float animTime);

    glm::mat4 lerpScale(float animTime);

};

