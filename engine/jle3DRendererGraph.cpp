// Copyright (c) 2023. Johan Lind


#include "jle3DRendererGraph.h"
void
jle3DRendererGraph::sendMesh(std::shared_ptr<jleMesh> &mesh,
                             std::shared_ptr<jleMaterial> &material,
                             const glm::mat4 &transform,
                             int instanceId,
                             bool castShadows)
{
    _meshes.emplace_back(jle3DRendererQueuedMesh{mesh, material, transform, instanceId, castShadows});
}

void
jle3DRendererGraph::sendLineStrip(const std::vector<jle3DLineVertex> &lines)
{
    _lineStrips.emplace_back(lines);
}

void
jle3DRendererGraph::sendLines(const std::vector<jle3DLineVertex> &lines)
{
    _lines.insert(std::end(_lines), std::begin(lines), std::end(lines));
}

void
jle3DRendererGraph::sendLine(const jle3DLineVertex &from, const jle3DLineVertex &to)
{
    _lines.emplace_back(from);
    _lines.emplace_back(to);
}

void
jle3DRendererGraph::sendLight(const glm::vec3 &position, const glm::vec3 &color)
{
    _lights.emplace_back(jle3DRendererLight{position, color});
}
