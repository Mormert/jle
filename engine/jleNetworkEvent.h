// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleCompileHelper.h"

#include <array>
#include <memory>
#include <string_view>

namespace jleNetNameTransform
{

constexpr size_t
djb2(const char *str)
{
    size_t hash = 5381;
    while (*str) {
        hash = (hash << 5) + *str++;
    }
    return hash;
}

constexpr std::array<char, 4>
createHash(std::string_view str)
{
    // A function that returns 4 bytes of a hash from an input string, where the last byte is a null-terminator

    size_t l = djb2(str.data());
    std::array<char, 4> ret{};
    for (int i = 0; i < 3; i++) {
        ret[i] = static_cast<uint8_t>(l & 0xFF);
        l >>= 8;
    }
    ret[3] = '\0';
    return ret;
}

template <typename T>
struct HashedEventName {
    // This is used to shorten the event name from the original class name "SomeLargeNameForAnEvent"
    // to a short 4 byte, 3-character long version, like "+£?" with a null-terminator byte at the end.
    // This is done at compile time.
    static constexpr std::array<char, 4> hash = createHash(T::name);
};

} // namespace jleNetNameTransform

// Use this macro to register net events, and shorten the name for the event using hashing.
#define JLE_REGISTER_NET_EVENT(T)                                                                                      \
    namespace cereal                                                                                                   \
    {                                                                                                                  \
    namespace detail                                                                                                   \
    {                                                                                                                  \
    template <>                                                                                                        \
    struct binding_name<T> {                                                                                           \
        struct HashName {                                                                                              \
            static constexpr std::string_view name = #T;                                                               \
        };                                                                                                             \
        CEREAL_STATIC_CONSTEXPR char const *                                                                           \
        name()                                                                                                         \
        {                                                                                                              \
            constexpr const char *hashedName = jleNetNameTransform::HashedEventName<HashName>::hash.data();            \
            return hashedName;                                                                                         \
        }                                                                                                              \
    };                                                                                                                 \
    }                                                                                                                  \
    }                                                                                                                  \
    CEREAL_BIND_TO_ARCHIVES(T)                                                                                         \
    CEREAL_REGISTER_POLYMORPHIC_RELATION(jleNetworkEvent, T)

struct jleNetworkEvent {
    template <class Archive>
    void serialize(Archive &archive);

    virtual ~jleNetworkEvent() = default;

    virtual void onReceiveFromClient(int32_t senderId) = 0;

protected:
    friend class jleSceneServer;
    jleSceneServer &getSceneServer();

private:
    jleSceneServer *_serverScene;
};

template <typename Event>
std::unique_ptr<Event>
jleMakeNetEvent()
{
    static_assert(std::is_base_of<jleNetworkEvent, Event>::value, "Event must derive from jleNetworkEvent");
    return std::make_unique<Event>();
}

JLE_EXTERN_TEMPLATE_CEREAL_H(jleNetworkEvent)
