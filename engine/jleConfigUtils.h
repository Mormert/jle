// Copyright (c) 2023. Johan Lind

#pragma once

#include "json.hpp"
#include "plog/Log.h"
#include <fstream>

namespace cfg {
template <typename T>
inline void loadEngineConfig(const std::string &cfgName, T &cfgJson) {
    std::ifstream i(cfgName);
    if (i.good()) {
        nlohmann::json j;
        i >> j;

        from_json(j, cfgJson);
    }
    else {
        LOG_ERROR << "Could not load engine config: " << cfgName;
    }
}

inline void saveEngineConfig(const std::string &cfgName, nlohmann::json &j) {
    std::ofstream i(cfgName);
    if (i.good()) {
        i << j.dump(4);
    }
    else {
        LOG_ERROR << "Could not save engine config: " << cfgName;
    }
}
} // namespace cfg
