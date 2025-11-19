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

#include <cereal/cereal.hpp>
#include <string>
#include <utility>

// A class that holds paths such as for example "ER:SomeFolder/SomeFile.txt", that
// is actually located in the "EngineResources" folder, that can be located at
// different places, depending on build configuration, etc

class jleRealPath;

class jleVirtualPath {
public:
    explicit jleVirtualPath(const char* path) : _virtualPath(path) {}
    explicit jleVirtualPath(std::string path) : _virtualPath(std::move(path)) {}

    std::string* operator->() { return &_virtualPath; }
    const std::string* operator->() const { return &_virtualPath; }
    bool operator==(const jleVirtualPath& other) const { return _virtualPath == other._virtualPath; }

    [[nodiscard]] const std::string& str() const { return _virtualPath; }

    [[nodiscard]] jleRealPath getRealPath() const;

private:
    std::string _virtualPath;
};

class jleRealPath {
public:
    explicit jleRealPath(const char* path) : _realPath(path) {}
    explicit jleRealPath(std::string path) : _realPath(std::move(path)) {}

    std::string* operator->() { return &_realPath; }
    const std::string* operator->() const { return &_realPath; }
    bool operator==(const jleRealPath& other) const { return _realPath == other._realPath; }

    [[nodiscard]] const std::string& str() const { return _realPath; }

    [[nodiscard]] jleVirtualPath getVirtualPath() const;
private:
    std::string _realPath;
};

using jlePathHash = uint64_t;

constexpr jlePathHash jlePathHashingFunc(std::string_view virtualPath) {
    // fnv1a_hash
    uint64_t hash = 14695981039346656037ull;
    for (const char c : virtualPath) {
        hash = (hash ^ static_cast<uint8_t>(c)) * 1099511628211ull;
    }
    return hash;
}

#define JLE_PATH_HASH(virtualPath) jlePath{jlePathHashingFunc(virtualPath)}

class jlePath
{
public:
    jlePath() = default; // Default initialization contains an empty path

    explicit jlePath(jlePathHash pathHash);
    explicit jlePath(const jleVirtualPath& virtualPath);
    explicit jlePath(const jleRealPath& realPath);

    template <class Archive>
    auto save_minimal(const Archive&) const -> std::enable_if_t<cereal::traits::is_text_archive<Archive>::value, std::string> { return getVirtualPath().str(); }

    template <class Archive>
    auto save_minimal(const Archive&) const -> std::enable_if_t<!cereal::traits::is_text_archive<Archive>::value, jlePathHash> { return _hash; }

    template <class Archive>
    auto load_minimal(const Archive&, const std::string& value) -> std::enable_if_t<cereal::traits::is_text_archive<Archive>::value> { _hash = jlePathHashingFunc(value); }

    template <class Archive>
    auto load_minimal(Archive const &, const jlePathHash& value) -> std::enable_if_t<!cereal::traits::is_text_archive<Archive>::value> { _hash = value; }

    // Returns the drive, like "GR:"
    [[nodiscard]] std::string getPathVirtualDrive() const;

    [[nodiscard]] const jleVirtualPath&  getVirtualPath() const;
    [[nodiscard]] const jleRealPath&     getRealPath() const;
    [[nodiscard]] jlePathHash getHash() const { return _hash; }

    [[nodiscard]] std::string getVirtualFolder() const;

    [[nodiscard]] bool isEmpty() const { return _hash == 0; }

    [[nodiscard]] std::string getFileEnding() const;
    [[nodiscard]] std::string getFileNameNoEnding() const;

    bool operator==(const jlePath &other) const { return _hash == other._hash; }
    bool operator<(const jlePath& other) const { return _hash < other._hash; }

private:
    jlePathHash _hash{0};
    friend class std::hash<jlePath>;
};

namespace std
{
template <>
struct hash<jlePath> {
    size_t operator()(const jlePath &path) const noexcept {
        return path._hash;
    }
};

template <>
struct hash<jleVirtualPath> {
    size_t operator()(const jleVirtualPath& path) const noexcept {
        return hash<std::string>()(path.str());
    }
};

template <>
struct hash<jleRealPath> {
    size_t operator()(const jleRealPath& path) const noexcept {
        return hash<std::string>()(path.str());
    }
};
} // namespace std
