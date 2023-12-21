// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleCompileHelper.h"
#include <memory>

struct jleNetworkEvent
{
    template <class Archive>
    void serialize(Archive &archive);

    virtual ~jleNetworkEvent() = default;

    virtual void onReceiveFromClient(int32_t senderId) = 0;
};

template <typename Event>
std::unique_ptr<Event> jleMakeNetEvent()
{
    static_assert(std::is_base_of<jleNetworkEvent, Event>::value, "Event must derive from jleNetworkEvent");
    return std::make_unique<Event>();
}

JLE_EXTERN_TEMPLATE_CEREAL_H(jleNetworkEvent)
