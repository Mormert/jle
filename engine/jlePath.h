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

#include "jleBuildConfig.h"

#include <cereal/cereal.hpp>
#include <string>

// A class that holds paths such as for example "ER:SomeFolder/SomeFile.txt", that
// is actually located in the "EngineResources" folder, that can be located at
// different places, depending on build configuration, etc

class jlePath
{
public:
    jlePath() = default;

    template <class Archive>
    std::string
    save_minimal(Archive const &) const
    {
        return _virtualPath;
    }

    template <class Archive>
    void
    load_minimal(Archive const &, std::string const &value)
    {
        _virtualPath = value;
        fixSlashes(_virtualPath);
        _realPath = findRealPathFromVirtualPath(_virtualPath);
        _hash = std::hash<std::string>()(_virtualPath);
    }

    jlePath(const char* virtualPath);

    explicit jlePath(const std::string &virtualPath);
    explicit jlePath(const std::string &path, bool virtualPath);

    // Returns the drive, like "GR:"
    [[nodiscard]] std::string getPathVirtualDrive() const;

    [[nodiscard]] std::string getVirtualPath() const;
    [[nodiscard]] std::string getVirtualPath();

    [[nodiscard]] std::string getRealPath() const;
    [[nodiscard]] std::string getRealPath();

    [[nodiscard]] std::string getRealPathConst() const;
    [[nodiscard]] std::string getVirtualPathConst() const;

    [[nodiscard]] std::string getVirtualFolder() const;

    bool isEmpty();

    std::string getFileEnding() const;

    std::string getFileNameNoEnding() const;

    bool operator==(const jlePath &other) const;

    bool operator<(const jlePath& other) const;

    friend class std::hash<jlePath>;

    // Below operator broke Lua bindings for some reason:
    // friend std::ostream &operator<<(std::ostream &stream, const jlePath &path);

    // Note: should actually be private!
    // Don't modify!
    std::string _virtualPath;

    size_t hash() const;

private:
    mutable std::string _realPath;

private:
    static std::string findVirtualPathFromRealPath(const std::string &realPath);
    static std::string findRealPathFromVirtualPath(const std::string &virtualPath);

    static void fixSlashes(std::string &str);

    size_t _hash;
};

namespace std
{
template <>
struct hash<jlePath> {
    std::size_t
    operator()(const jlePath &path) const
    {
        return path._hash;
    }
};
} // namespace std
