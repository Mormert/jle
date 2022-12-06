// Copyright (c) 2022. Johan Lind

#include "jleMaterial.h"
#include <fstream>
#include <plog/Log.h>

jleMaterial::~jleMaterial() = default;

bool
jleMaterial::loadFromFile(const std::string &path)
{
    std::ifstream i(path);
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        from_json(j, *this);
    } else {
        LOG_ERROR << "Could not load scene with path: ";
        return false;
    }

    if (!_albedoPath.empty()) {
        albedoTexture = jleTexture::fromPath(jleRelativePath{_albedoPath});
    }

    if (!_normalPath.empty()) {
        normalTexture = jleTexture::fromPath(jleRelativePath{_normalPath});
    }

    if (!_metallicPath.empty()) {
        metallicTexture = jleTexture::fromPath(jleRelativePath{_metallicPath});
    }

    if (!_roughnessPath.empty()) {
        roughnessTexture = jleTexture::fromPath(jleRelativePath{_roughnessPath});
    }

    return true;
}

void
to_json(nlohmann::json &j, const jleMaterial &m)
{
    j = nlohmann::json{{"albedoPath", m._albedoPath},
                       {"normalPath", m._normalPath},
                       {"metallicPath", m._metallicPath},
                       {"roughnessPath", m._roughnessPath}};
}

void
from_json(const nlohmann::json &j, jleMaterial &m)
{
    m._albedoPath = j["albedoPath"];
    m._normalPath = j["normalPath"];
    m._metallicPath = j["metallicPath"];
    m._roughnessPath = j["roughnessPath"];
}