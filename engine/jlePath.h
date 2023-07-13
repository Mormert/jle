// Copyright (c) 2023. Johan Lind

#pragma once

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
    std::string save_minimal(Archive const & ) const
    {
        return _virtualPath;
    }

    template <class Archive>
    void load_minimal( Archive const &, std::string const & value )
    {
        fixSlashes(_virtualPath);
        _realPath = findRealPathFromVirtualPath(_virtualPath);
        _virtualPath = value;
    }

    jlePath(const std::string &path, bool virtualPath = true);

    // Returns the drive, like "GR:"
    [[nodiscard]] std::string getPathPrefix() const;

    [[nodiscard]] std::string getVirtualPath() const;
    [[nodiscard]] std::string getVirtualPath();

    [[nodiscard]] std::string getRealPath() const;
    [[nodiscard]] std::string getRealPath();

    bool isEmpty();

    std::string getFileEnding() const;

    bool operator==(const jlePath &other) const;

    friend class std::hash<jlePath>;

    friend std::ostream& operator<< (std::ostream& stream, const jlePath& path);

    // Note: should actually be private!
    // Don't modify!
    std::string _virtualPath;
private:
    std::string _realPath;

private:
    static std::string findVirtualPathFromRealPath(const std::string &realPath);
    static std::string findRealPathFromVirtualPath(const std::string &virtualPath);

    static void fixSlashes(std::string& str);
};

namespace std
{
template <>
struct hash<jlePath> {
    std::size_t
    operator()(const jlePath &path) const
    {
        return hash<std::string>()(path._virtualPath);
    }
};
} // namespace std
