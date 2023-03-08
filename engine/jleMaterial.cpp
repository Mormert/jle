// Copyright (c) 2023. Johan Lind

#include "jleMaterial.h"
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>
#include <plog/Log.h>

jleMaterial::~jleMaterial() = default;

bool
jleMaterial::loadFromFile(const std::string &path)
{

    try {
        std::ifstream i(path);
        cereal::JSONInputArchive iarchive{i};
        iarchive(*this);
    } catch (std::exception &e) {
        LOGE << "Failed loading material: " << e.what();
    }

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
        albedoTexture = gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{_albedoPath});
    }

    if (!_normalPath.empty()) {
        normalTexture = gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{_normalPath});
    }

    if (!_metallicPath.empty()) {
        metallicTexture = gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{_metallicPath});
    }

    if (!_roughnessPath.empty()) {
        roughnessTexture = gCore->resources().loadResourceFromFile<jleTexture>(jleRelativePath{_roughnessPath});
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

void
jleMaterial::saveToFile()
{
    std::ofstream materialSave{filepath};
    cereal::JSONOutputArchive outputArchive(materialSave);
    this->serialize(outputArchive);
}
