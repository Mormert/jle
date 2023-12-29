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

#ifndef JLE_PHYSICSDEBUGDRAWER_H
#define JLE_PHYSICSDEBUGDRAWER_H

#include <LinearMath/btIDebugDraw.h>
#include <glm/glm.hpp>
#include <vector>


class jlePhysicsDebugDrawer : public btIDebugDraw
{
public:

    ~jlePhysicsDebugDrawer() override;

    virtual DefaultColors getDefaultColors() const override;
    ///the default implementation for setDefaultColors has no effect. A derived class can implement it and store the colors.
    void setDefaultColors(const DefaultColors& colors) override;

    void drawLine(const btVector3& from1, const btVector3& to1, const btVector3& color1) override;

    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;

    void reportErrorWarning(const char* warningString) override;

    void draw3dText(const btVector3& location, const char* textString) override;

    void setDebugMode(int debugMode) override;

    int getDebugMode() const override;

    void flushLines() override;

};

#endif // JLE_PHYSICSDEBUGDRAWER_H
