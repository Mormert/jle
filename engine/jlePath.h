// Copyright (c) 2023. Johan Lind

#pragma once

#include <string>

#include <cereal/cereal.hpp>

class jleRelativePath;
class jleAbsolutePath;

// A class that has paths such as for example "ER:SomeFolder/SomeFile.txt", that
// is actually located in the "EngineResources" folder, that can be located at
// different places, depending on build configuration, etc
class jleRelativePath
{
public:
    friend class jleAbsolutePath;

    jleRelativePath() = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        ar(CEREAL_NVP(_relativePath));
    }

    // Constructs a relative path directly
    explicit jleRelativePath(const std::string &relativePath);

    // Constructs a relative path from an absolute path
    explicit jleRelativePath(const jleAbsolutePath &absolutePath);

    // Returns the drive, like "GR:"
    const std::string pathPrefix() const;

    [[nodiscard]] std::string relativePathStr() const;

    [[nodiscard]] std::string absolutePathStr() const;

    bool
    isEmpty()
    {
        return _relativePath.empty();
    }

    bool
    operator==(const jleRelativePath &other) const
    {
        return (_relativePath == other._relativePath);
    }

    friend class std::hash<jleRelativePath>;

private:
    std::string _relativePath;
};

namespace std
{

template <>
struct hash<jleRelativePath> {
    std::size_t
    operator()(const jleRelativePath &path) const
    {
        return hash<std::string>()(path._relativePath);
    }
};

} // namespace std

// A class that contains the absolute path to a resource, for example
// "C:dev/jle/EngineResources/SomeFolder/SomeFile.txt" and can be used to find
// the relative path "ER:SomeFolder/SomeFile.txt".
class jleAbsolutePath
{
public:
    friend class jleRelativePath;

    // Constructs an absolute path directly
    explicit jleAbsolutePath(const std::string &absoultePath);

    // Constructs an absolute path from a relative path
    explicit jleAbsolutePath(const jleRelativePath &relativePath);

    [[nodiscard]] std::string relativePathStr() const;

    [[nodiscard]] std::string absolutePathStr() const;

private:
    std::string _absolutePath;
};
