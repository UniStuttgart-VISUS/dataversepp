// <copyright file="hash.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "dataverse/api.h"
#include "dataverse/narrow_string.h"


namespace visus {
namespace dataverse {

    /// <summary>
    /// Computes the specified hash of the specified file.
    /// </summary>
    /// <param name="dst">A buffer of at least <paramref name="cnt" /> bytes to
    /// receive the hash.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> in bytes.</param>
    /// <param name="path">The path to the file to hash.</param>
    /// <param name="algorithm">The hash algorithm to use. On Windows, you can
    /// use the <c>BCRYPT_*_ALGORITHM</c> constants.</param>
    /// <returns>The required size for storing the hash.</returns>
    /// <exception cref="std::system_error">If the operation failed.</exception>
    extern std::size_t DATAVERSE_API hash(
        _Out_writes_to_opt_(cnt, return) std::uint8_t *dst,
        _In_ const std::size_t cnt,
        _In_z_ const wchar_t *path,
        _In_z_ const wchar_t *algorithm = L"MD5");

    /// <summary>
    /// Computes the specified hash of the specified file.
    /// </summary>
    /// <param name="dst">A buffer of at least <paramref name="cnt" /> bytes to
    /// receive the hash.</param>
    /// <param name="cnt">The size of <paramref name="dst" /> in bytes.</param>
    /// <param name="path">The path to the file to hash.</param>
    /// <param name="algorithm">The hash algorithm to use. On Windows, you can
    /// use the <c>BCRYPT_*_ALGORITHM</c> constants.</param>
    /// <returns>The required size for storing the hash.</returns>
    /// <exception cref="std::system_error">If the operation failed.</exception>
    extern std::size_t DATAVERSE_API hash(
        _Out_writes_to_opt_(cnt, return) std::uint8_t *dst,
        _In_ const std::size_t cnt,
        _In_ const const_narrow_string& path,
        _In_ const const_narrow_string& algorithm);

    /// <summary>
    /// Converts the given binary data to a base64-encoded string.
    /// </summary>
    /// <param name="dst">A character buffer of at least
    /// <paramref name="cnt_dst" /> characters.</param>
    /// <param name="cnt_dst">The size of the output buffer in number of
    /// characters.</param>
    /// <param name="src">The source to be converted.</param>
    /// <param name="cnt_src">The length of the source data in bytes.</param>
    /// <returns>The required output buffer size in number of characters.
    /// </returns>
    /// <exception cref="std::system_error">If the operation failed.</exception>
    extern std::size_t DATAVERSE_API to_base64(
        _Out_writes_to_opt_(cnt_dst, return) wchar_t *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_(cnt_src) const std::uint8_t *src,
        _In_ const std::size_t cnt_src);

    /// <summary>
    /// Converts the given binary data to a base64-encoded string.
    /// </summary>
    /// <remarks>
    /// As the base64 alphabet fits into 7-bit ASCII, the output buffer does not
    /// have an encoding attached. Callers should assume it to be ASCII.
    /// </remarks>
    /// <param name="dst">A character buffer of at least
    /// <paramref name="cnt_dst" /> characters.</param>
    /// <param name="cnt_dst">The size of the output buffer in number of
    /// characters.</param>
    /// <param name="src">The source to be converted.</param>
    /// <param name="cnt_src">The length of the source data in bytes.</param>
    /// <returns>The required output buffer size in number of characters.
    /// </returns>
    /// <exception cref="std::system_error">If the operation failed.</exception>
    extern std::size_t DATAVERSE_API to_base64(
        _Out_writes_to_opt_(cnt_dst, return) char *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_(cnt_src) const std::uint8_t *src,
        _In_ const std::size_t cnt_src);

    /// <summary>
    /// Measures the buffer size required to store the given data in base64
    /// encoding.
    /// </summary>
    /// <param name="dst"><c>nullptr</c>.</param>
    /// <param name="cnt_dst">This parameter is ignored.</param>
    /// <param name="src">The source to be converted.</param>
    /// <param name="cnt_src">The length of the source data in bytes.</param>
    /// <returns>The required output buffer size in number of characters.
    /// </returns>
    /// <exception cref="std::system_error">If the operation failed.</exception>
    extern std::size_t DATAVERSE_API to_base64(
        _In_opt_ std::nullptr_t dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_(cnt_src) const std::uint8_t *src,
        _In_ const std::size_t cnt_src);

} /* namespace dataverse */
} /* namespace visus */
