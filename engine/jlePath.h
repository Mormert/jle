// Copyright (c) 2022. Johan Lind

#ifndef JLEPATH_H
#define JLEPATH_H

#include <string>

class jleRelativePath;

class jleAbsolutePath;

// A class that has paths such as for example "ER:SomeFolder/SomeFile.txt", that
// is actually located in the "EngineResources" folder, that can be located at
// different places, depending on build configuration, etc
class jleRelativePath {
public:
    friend class jleAbsolutePath;

    // Constructs a relative path directly
    explicit jleRelativePath(const std::string& relativePath);

    // Constructs a relative path from an absolute path
    explicit jleRelativePath(const jleAbsolutePath& absolutePath);

    // Returns the drive, like "GR:"
    const std::string GetPathPrefix() const;

    [[nodiscard]] std::string GetRelativePathStr() const;

    [[nodiscard]] std::string GetAbsolutePathStr() const;

private:
    std::string mRelativePath;
};

// A class that contains the absolute path to a resource, for example
// "C:dev/jle/EngineResources/SomeFolder/SomeFile.txt" and can be used to find
// the relative path "ER:SomeFolder/SomeFile.txt".
class jleAbsolutePath {
public:
    friend class jleRelativePath;

    // Constructs an absolute path directly
    explicit jleAbsolutePath(const std::string& absoultePath);

    // Constructs an absolute path from a relative path
    explicit jleAbsolutePath(const jleRelativePath& relativePath);

    [[nodiscard]] std::string GetRelativePathStr() const;

    [[nodiscard]] std::string GetAbsolutePathStr() const;

private:
    std::string mAbsolutePath;
};

#endif // JLEPATH_H
