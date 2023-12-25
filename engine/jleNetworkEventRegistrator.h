// Copyright (c) 2023. Johan Lind

#pragma once

#include <cereal/types/polymorphic.hpp>

#include <array>
#include <string_view>

// Use this macro to register networked events, and shorten the serialized name for the event using hashing.
// This works for registering both client to server events, and server to client events as well as any other networked event.
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
    namespace cereal                                                                                                   \
    {                                                                                                                  \
    namespace detail                                                                                                   \
    {                                                                                                                  \
    template <>                                                                                                        \
    struct PolymorphicRelation<jleNetworkEvent, T> {                                                                   \
        static void                                                                                                    \
        bind()                                                                                                         \
        {                                                                                                              \
            if constexpr (std::is_base_of<jleClientToServerEvent, T>::value) {                                         \
                RegisterPolymorphicCaster<jleClientToServerEvent, T>::bind();                                          \
            } else if constexpr (std::is_base_of<jleServerToClientEvent, T>::value) {                                  \
                RegisterPolymorphicCaster<jleServerToClientEvent, T>::bind();                                          \
            } else {                                                                                                   \
                RegisterPolymorphicCaster<jleNetworkEvent, T>::bind();                                               \
            }                                                                                                          \
        }                                                                                                              \
    };                                                                                                                 \
    }                                                                                                                  \
    }

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
