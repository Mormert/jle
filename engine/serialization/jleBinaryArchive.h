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

// Based on cereal's binary archives

#pragma once

#include <cereal/cereal.hpp>
#include <sstream>

#include "jleSerialization.h"

// ######################################################################
//! An output archive designed to save data in a compact binary representation
/*! This archive outputs data to a stream in an extremely compact binary
    representation with as little extra metadata as possible.

    This archive does nothing to ensure that the endianness of the saved
    and loaded data is the same.  If you need to have portability over
    architectures with different endianness, use cereal::PortableBinaryOutputArchive.

    When using a binary archive and a file stream, you must use the
    std::ios::binary format flag to avoid having your data altered
    inadvertently.

    \ingroup Archives */
class jleBinaryOutputArchive : public jleSerializationArchive, public cereal::OutputArchive<jleBinaryOutputArchive, cereal::AllowEmptyClassElision>
{
public:
    //! Construct, outputting to the provided stream
    /*! @param stream The stream to output to.  Can be a stringstream, a file stream, or
                      even cout! */
    jleBinaryOutputArchive(std::ostream &stream, jleSerializationContext ctx)
        : jleSerializationArchive(ctx), OutputArchive<jleBinaryOutputArchive, cereal::AllowEmptyClassElision>(this), itsStream(stream)
    {
    }

    ~jleBinaryOutputArchive() CEREAL_NOEXCEPT = default;

    //! Writes size bytes of data to the output stream
    void
    saveBinary(const void *data, std::streamsize size)
    {
        auto const writtenSize = itsStream.rdbuf()->sputn(reinterpret_cast<const char *>(data), size);

        if (writtenSize != size)
            throw cereal::Exception("Failed to write " + std::to_string(size) + " bytes to output stream! Wrote " +
                            std::to_string(writtenSize));
    }

private:
    std::ostream &itsStream;
};

// ######################################################################
//! An input archive designed to load data saved using jleBinaryOutputArchive
/*  This archive does nothing to ensure that the endianness of the saved
    and loaded data is the same.  If you need to have portability over
    architectures with different endianness, use PortableBinaryOutputArchive.

    When using a binary archive and a file stream, you must use the
    std::ios::binary format flag to avoid having your data altered
    inadvertently.

    \ingroup Archives */
class jleBinaryInputArchive : public jleSerializationArchive, public cereal::InputArchive<jleBinaryInputArchive, cereal::AllowEmptyClassElision>
{
public:
    //! Construct, loading from the provided stream
    jleBinaryInputArchive(std::istream &stream, jleSerializationContext ctx)
        : jleSerializationArchive(ctx), InputArchive<jleBinaryInputArchive, cereal::AllowEmptyClassElision>(this), itsStream(stream)
    {
    }

    ~jleBinaryInputArchive() CEREAL_NOEXCEPT = default;

    //! Reads size bytes of data from the input stream
    void
    loadBinary(void *const data, std::streamsize size)
    {
        auto const readSize = itsStream.rdbuf()->sgetn(reinterpret_cast<char *>(data), size);

        if (readSize != size)
            throw cereal::Exception("Failed to read " + std::to_string(size) + " bytes from input stream! Read " +
                            std::to_string(readSize));
    }

private:
    std::istream &itsStream;
};

// ######################################################################
// Common BinaryArchive serialization functions

//! Saving for POD types to binary
template <class T>
inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
CEREAL_SAVE_FUNCTION_NAME(jleBinaryOutputArchive &ar, T const &t)
{
    ar.saveBinary(std::addressof(t), sizeof(t));
}

//! Loading for POD types from binary
template <class T>
inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME(jleBinaryInputArchive &ar, T &t)
{
    ar.loadBinary(std::addressof(t), sizeof(t));
}

//! Serializing NVP types to binary
template <class Archive, class T>
inline CEREAL_ARCHIVE_RESTRICT(jleBinaryInputArchive, jleBinaryOutputArchive)
    CEREAL_SERIALIZE_FUNCTION_NAME(Archive &ar, cereal::NameValuePair<T> &t)
{
    ar(t.value);
}

//! Serializing SizeTags to binary
template <class Archive, class T>
inline CEREAL_ARCHIVE_RESTRICT(jleBinaryInputArchive, jleBinaryOutputArchive)
    CEREAL_SERIALIZE_FUNCTION_NAME(Archive &ar, cereal::SizeTag<T> &t)
{
    ar(t.size);
}

//! Saving binary data
template <class T>
inline void
CEREAL_SAVE_FUNCTION_NAME(jleBinaryOutputArchive &ar, cereal::BinaryData<T> const &bd)
{
    ar.saveBinary(bd.data, static_cast<std::streamsize>(bd.size));
}

//! Loading binary data
template <class T>
inline void
CEREAL_LOAD_FUNCTION_NAME(jleBinaryInputArchive &ar, cereal::BinaryData<T> &bd)
{
    ar.loadBinary(bd.data, static_cast<std::streamsize>(bd.size));
}

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE(jleBinaryOutputArchive)
CEREAL_REGISTER_ARCHIVE(jleBinaryInputArchive)

// tie input and output archives together
CEREAL_SETUP_ARCHIVE_TRAITS(jleBinaryInputArchive, jleBinaryOutputArchive)
