// Copyright (c) 2022. Johan Lind

#pragma once

#define JLE_FROM_JSON_WITH_DEFAULT(JSON, VAL, STR, DEF)                             \
{const auto& jsonFindValue = (JSON).find(STR);                                        \
if(jsonFindValue != (JSON).end()) {(VAL) = jsonFindValue.value();} else {(VAL) = DEF;}}
// Example usage: JLE_FROM_JSON_WITH_DEFAULT(j_in, x, "x", 0.f);

#define JLE_FROM_JSON_IF_EXISTS(JSON, VAL, STR)                     \
{const auto& jsonFindValue = (JSON).find(STR);                        \
if(jsonFindValue != (JSON).end()) {(VAL) = jsonFindValue.value();}}
// Example usage: JLE_FROM_JSON_IF_EXISTS(j_in, x, "x");


template<class JSON>
class jleJsonInterface {
public:
    virtual ~jleJsonInterface() = default;

    virtual void ToJson(JSON &j_out) = 0;

    virtual void FromJson(const JSON &j_in) = 0;
};