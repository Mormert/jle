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

#include <core/serialization/jleExternalSerialization.h>

#include <cereal/cereal.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class cTransform
{
public:

    cTransform() = default;
    cTransform(const glm::mat4& localMatrix) : _localMatrix(localMatrix) {}

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_localMatrix));
    }

    // Retrieve the full local transformation matrix
    [[nodiscard]] inline const glm::mat4& getLocalMatrix() const
    {
        return _localMatrix;
    }

    // Set the full local transformation matrix directly
    inline void setLocalMatrix(const glm::mat4 &matrix)
    {
        _localMatrix = matrix;
    }

    // Extract position from the matrix (translation is stored in the fourth column)
    [[nodiscard]] inline glm::vec3 getPosition() const
    {
        return {(_localMatrix[3])};
    }

    // Set position: (modifies the fourth matrix column)
    inline void setPosition(const glm::vec3 &pos)
    {
        _localMatrix[3] = glm::vec4(pos, 1.0f);
    }

    // Translation convenience methods
    inline void translate(const glm::vec3 &delta)
    {
        _localMatrix = glm::translate(_localMatrix, delta);
    }

    // Get rotation as a quaternion from the local matrix
    [[nodiscard]] inline glm::quat getRotation() const
    {
        glm::vec3 scale;
        glm::quat rotation;
        decompose(scale, rotation);
        return rotation;
    }

    // Set rotation while preserving current position and scale
    inline void setRotation(const glm::quat &rot)
    {
        glm::vec3 position = getPosition();
        glm::vec3 scale;
        glm::quat unusedRot;
        decompose(scale, unusedRot);
        recompose(position, rot, scale);
    }

    [[nodiscard]] inline glm::vec3 getRight() const
    {
        return glm::normalize(glm::vec3{getLocalMatrix()[0]});
    }

    [[nodiscard]] inline glm::vec3 getUp() const
    {
        return glm::normalize(glm::vec3{getLocalMatrix()[1]});
    }

    [[nodiscard]] inline glm::vec3 getForward() const
    {
        return glm::normalize(glm::vec3{getLocalMatrix()[2]});
    }

    // Rotate by a quaternion increment
    inline void rotate(const glm::quat &deltaRot)
    {
        glm::quat currentRot = getRotation();
        setRotation(deltaRot * currentRot);
    }

    // Rotate around an axis by an angle (in radians)
    inline void rotate(const glm::vec3 &axis, float angleRadians)
    {
        glm::quat deltaRot = glm::angleAxis(angleRadians, glm::normalize(axis));
        rotate(deltaRot);
    }

    // Get uniform scale factor. If non-uniform scaling is present, this returns one representative scale.
    [[nodiscard]] inline glm::vec3 getScale() const
    {
        glm::vec3 scale;
        glm::quat rotation;
        decompose(scale, rotation);
        return scale;
    }

    // Set uniform scale, preserving position and rotation
    inline void setScale(const glm::vec3 &newScale)
    {
        glm::vec3 position = getPosition();
        glm::quat rotation = getRotation();
        recompose(position, rotation, newScale);
    }

    inline void scale(const glm::vec3 &scaleFactor)
    {
        glm::vec3 position = getPosition();
        glm::quat rotation = getRotation();
        glm::vec3 currentScale = getScale();
        recompose(position, rotation, currentScale * scaleFactor);
    }

private:
    glm::mat4 _localMatrix = glm::mat4(1.0f);

    // Decompose the local matrix into position, rotation (quaternion), and scale.
    // Assumes no shear and a valid transform matrix.
    inline void decompose(glm::vec3 &outScale, glm::quat &outRotation) const
    {
        // Extract basis vectors
        glm::vec3 col0(_localMatrix[0][0], _localMatrix[0][1], _localMatrix[0][2]);
        glm::vec3 col1(_localMatrix[1][0], _localMatrix[1][1], _localMatrix[1][2]);
        glm::vec3 col2(_localMatrix[2][0], _localMatrix[2][1], _localMatrix[2][2]);

        // Extract scales
        float scaleX = glm::length(col0);
        float scaleY = glm::length(col1);
        float scaleZ = glm::length(col2);

        // Protect against divide-by-zero in normalization
        if (scaleX > 0.0f) col0 /= scaleX;
        if (scaleY > 0.0f) col1 /= scaleY;
        if (scaleZ > 0.0f) col2 /= scaleZ;

        // Reconstruct rotation matrix
        glm::mat3 rotMat(col0, col1, col2);
        outRotation = glm::quat_cast(rotMat);
        outScale = glm::vec3(scaleX, scaleY, scaleZ);
    }

    // Rebuild the local matrix from position, rotation, and scale
    inline void recompose(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
    {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 r = glm::toMat4(rotation);
        glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

        _localMatrix = t * r * s;
    }
};
