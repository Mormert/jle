// Copyright (c) 2023. Johan Lind

#include "jlePhysicsDebugDrawer.h"
#include "jleCore.h"
#include "jleRendering.h"
#include "jle3DRenderer.h"

void
jlePhysicsDebugDrawer::setDebugMode(int debugMode)
{
}

int
jlePhysicsDebugDrawer::getDebugMode() const
{
    int mode = 0;
    mode |= btIDebugDraw::DBG_DrawWireframe;
    mode |= btIDebugDraw::DBG_DrawAabb;
    mode |= btIDebugDraw::DBG_DrawNormals;
    mode |= btIDebugDraw::DBG_DrawContactPoints;
    mode |= btIDebugDraw::DBG_DrawFrames;
    return mode;
}

void
jlePhysicsDebugDrawer::flushLines()
{
    if(_linePoints.size() >= 2)
    {
        gCore->rendering().rendering3d().sendLines(_linePoints, glm::vec3(_lastLinesColor.x(), _lastLinesColor.y(), _lastLinesColor.z()));
    }

    _linePoints.clear();
}

void
jlePhysicsDebugDrawer::draw3dText(const btVector3 &location, const char *textString)
{
}

void
jlePhysicsDebugDrawer::reportErrorWarning(const char *warningString)
{
}

void
jlePhysicsDebugDrawer::drawContactPoint(
    const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{
    drawLine(PointOnB, PointOnB + normalOnB * distance, color);
    btVector3 ncolor(0, 0, 0);
    drawLine(PointOnB, PointOnB + normalOnB * 0.01, ncolor);
}

void
jlePhysicsDebugDrawer::drawLine(const btVector3 &from1, const btVector3 &to1, const btVector3 &color1)
{

    if(color1 != _lastLinesColor || _linePoints.size() >= JLE_LINE_DRAW_BATCH_SIZE)
    {
        flushLines();
    }

    _linePoints.push_back(glm::vec3(from1.x(), from1.y(), from1.z()));
    _linePoints.push_back(glm::vec3(to1.x(), to1.y(), to1.z()));

    _lastLinesColor = color1;
}

void
jlePhysicsDebugDrawer::setDefaultColors(const btIDebugDraw::DefaultColors &colors)
{

}

btIDebugDraw::DefaultColors
jlePhysicsDebugDrawer::getDefaultColors() const
{
    return btIDebugDraw::getDefaultColors();
}

jlePhysicsDebugDrawer::~jlePhysicsDebugDrawer() {}