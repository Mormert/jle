// Copyright (c) 2023. Johan Lind

#ifndef JLE_3DRENDERERGRAPH_H
#define JLE_3DRENDERERGRAPH_H

#include "jleMaterial.h"
#include "jleMesh.h"

struct jle3DRendererLight {
    glm::vec3 position;
    glm::vec3 color;
};

struct jle3DLineVertex {
    glm::vec3 position;
    glm::vec3 color;
    // {1.f, 0.f, 0.f} means no attenuation (constant)
    glm::vec3 attenuation;
};

class jle3DRendererGraph
{
private:
    friend class jle3DRenderer;
    struct jle3DRendererQueuedMesh {
        std::shared_ptr<jleMesh> mesh;
        std::shared_ptr<jleMaterial> material;
        glm::mat4 transform;
        int instanceId;
        bool castShadows;
    };

public:
    void sendMesh(std::shared_ptr<jleMesh> &mesh,
                  std::shared_ptr<jleMaterial> &material,
                  const glm::mat4 &transform,
                  int instanceId,
                  bool castShadows);

    // Line strips will always connect each lines, from start to end.
    void sendLineStrip(const std::vector<jle3DLineVertex> &lines);

    // Lines needs to come in pairs in the points, two for each line.
    void sendLines(const std::vector<jle3DLineVertex> &lines);

    void sendLine(const jle3DLineVertex &from, const jle3DLineVertex &to);

    void sendLight(const glm::vec3 &position, const glm::vec3 &color);

private:
    std::vector<jle3DRendererQueuedMesh> _meshes;
    std::vector<std::vector<jle3DLineVertex>> _lineStrips;
    std::vector<jle3DLineVertex> _lines;
    std::vector<jle3DRendererLight> _lights;
};

#endif // JLE_3DRENDERERGRAPH_H
