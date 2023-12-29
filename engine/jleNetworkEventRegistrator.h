/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include <cereal/types/polymorphic.hpp>

#include <array>
#include <string_view>

#ifndef NDEBUG
#include <chrono>
#include <thread>
#define JLE_REGISTER_NET_EVENT_HASH_COLLIDE_CHECK(T)                                                                   \
    static inline jleNetNameTransform::HashCollideCheck T##_checker(#T);
#else
#define JLE_REGISTER_NET_EVENT_HASH_COLLIDE_CHECK(T)
#endif

// Use this macro to register networked events, and shorten the serialized name for the event using hashing.
// This works for registering both client to server events, and server to client events as well as any other networked
// event.
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
                                                                                                                       \
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
                RegisterPolymorphicCaster<jleNetworkEvent, T>::bind();                                                 \
            }                                                                                                          \
        }                                                                                                              \
    };                                                                                                                 \
    }                                                                                                                  \
    }                                                                                                                  \
    JLE_REGISTER_NET_EVENT_HASH_COLLIDE_CHECK(T)

namespace jleNetNameTransform
{

using HashType = std::array<char, 5>;

constexpr uint32_t
djb2(const char *str)
{
    uint64_t hash = 5381;
    while (*str) {
        hash = (hash << 5) + *str++;
    }
    return hash;
}

constexpr std::size_t
fnv1a_hash(const char *str, std::size_t hash = 14695981039346656037ull) noexcept
{
    return (str[0] == '\0') ? hash : fnv1a_hash(&str[1], (hash ^ static_cast<std::size_t>(str[0])) * 1099511628211ull);
}

constexpr HashType
createHash(std::string_view str)
{
    // A function that returns X bytes of a hash from an input string, where the last byte is a null-terminator

    auto hash = fnv1a_hash(str.data());
    HashType ret{};
    for (int i = 0; i < 4; ++i) {
        ret[i] = static_cast<char>((hash >> (i * 8)) & 0xFF);
    }

    // Set null terminator, and make the other bytes depend on last byte to make it significant
    ret[4] = '\0';

    return ret;
}

template <typename T>
struct HashedEventName {
    // This is used to shorten the event name from the original class name "SomeLargeNameForAnEvent"
    // to a short X byte, X-1 character long version, like "+£?" with a null-terminator byte at the end.
    // This is done at compile time.
    static constexpr HashType hash = createHash(T::name);
};

#ifndef NDEBUG

struct HashCollideCheckData {
    std::string hashName;
    HashType hashResult;
};

static inline std::vector<HashCollideCheckData> hashCollideChecker;

struct HashCollideCheck {
    explicit HashCollideCheck(const char *str)
    {
        for (auto &collideCheck : hashCollideChecker) {
            if (collideCheck.hashName == str) {
                // This name is already registered, skipping...
                return;
            }
        }

        auto h = createHash(str);

        for (auto &collideCheck : hashCollideChecker) {
            if (collideCheck.hashResult == h) {
#ifdef WIN32
                __debugbreak();
#endif
                int i = *reinterpret_cast<int *>(&h[0]);
                std::cerr << "Found two identical hashes (" << i << ") for event with name: " << str << ", "
                          << collideCheck.hashName << std::endl;
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(100ms);
                std::exit(1);
            }
        }

        hashCollideChecker.push_back({std::string{str}, h});
    }
};

#endif

} // namespace jleNetNameTransform
