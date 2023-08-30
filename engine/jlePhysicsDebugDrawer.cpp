// Copyright (c) 2023. Johan Lind

#include "jlePhysicsDebugDrawer.h"
#include "jle3DGraph.h"
#include "jleGameEngine.h"

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
    auto from = glm::vec3(from1.x(), from1.y(), from1.z());
    auto to = glm::vec3(to1.x(), to1.y(), to1.z());
    auto color = glm::vec3(color1.x(), color1.y(), color1.z());
    gEngine->renderGraph().sendLine(jle3DLineVertex{from, color, glm::vec3{1.f, 0.f, 0.f}},
                                              {to, color, glm::vec3{1.f, 0.f, 0.f}});
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