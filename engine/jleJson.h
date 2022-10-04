// Copyright (c) 2022. Johan Lind

#pragma once

#include <json.hpp>
#include <string>

#define JLE_FROM_JSON_WITH_DEFAULT(JSON, VAL, STR, DEF)                        \
    {                                                                          \
        const auto& jsonFindValue = (JSON).find(STR);                          \
        if (jsonFindValue != (JSON).end()) {                                   \
            (VAL) = jsonFindValue.value();                                     \
        }                                                                      \
        else {                                                                 \
            (VAL) = DEF;                                                       \
        }                                                                      \
    }
// Example usage: JLE_FROM_JSON_WITH_DEFAULT(j_in, x, "x", 0.f);

#define JLE_FROM_JSON_IF_EXISTS(JSON, VAL, STR)                                \
    {                                                                          \
        const auto& jsonFindValue = (JSON).find(STR);                          \
        if (jsonFindValue != (JSON).end()) {                                   \
            (VAL) = jsonFindValue.value();                                     \
        }                                                                      \
    }
// Example usage: JLE_FROM_JSON_IF_EXISTS(j_in, x, "x");

template <class JSON>
class jleJsonInterface {
public:
    virtual ~jleJsonInterface() = default;

    virtual void toJson(JSON& j_out) = 0;

    virtual void fromJson(const JSON& j_in) = 0;
};

// JSON simple data wrappers used in arrays (wrapping as an object)

struct jleJsonString {
    std::string _string;
};

inline void from_json(const nlohmann::json& j, jleJsonString& s) {
    JLE_FROM_JSON_WITH_DEFAULT(j, s._string, "_string", "");
}

inline void to_json(nlohmann::json& j, const jleJsonString& s) {
    j["_string"] = s._string;
}

struct jleJsonInt {
    int _int;
};

inline void from_json(const nlohmann::json& j, jleJsonInt& i) {
    JLE_FROM_JSON_WITH_DEFAULT(j, i._int, "_int", 0);
}

inline void to_json(nlohmann::json& j, const jleJsonInt& i) {
    j["_int"] = i._int;
}

struct jleJsonFloat {
    float _float;
};

inline void from_json(const nlohmann::json& j, jleJsonFloat& f) {
    JLE_FROM_JSON_WITH_DEFAULT(j, f._float, "_float", 0);
}

inline void to_json(nlohmann::json& j, const jleJsonFloat& f) {
    j["_float"] = f._float;
}