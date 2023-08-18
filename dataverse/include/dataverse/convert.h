// <copyright file="convert.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#endif /* defined(_WIN32) */

#include "dataverse/narrow_string.h"


namespace visus {
namespace dataverse {

    /// <summary>
    /// Converts a multi-byte string of the given encoding into an UTF-16
    /// string.
    /// </summary>
    /// <param name="dst">A buffer to receive the output that can hold at least
    /// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
    /// <paramref name="cnt_dst" /> is zero.</param>
    /// <param name="cnt_dst">The number of elements that can be stored to
    /// <paramref name="dst" />.</param>
    /// <param name="src">The source string.</param>
    /// <param name="cnt_src">The length of the source string, which can be zero
    /// if <paramref name="src" /> is null-terminated.</param>
    /// <param name="code_page">The code page used for the narrow string.</param>
    /// <returns>The number of characters required for the output string,
    /// including the terminating zero.</returns>
    /// <exception cref="std::invalid_argument">If <paramref name="src" /> is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    extern std::size_t DATAVERSE_API convert(
        _Out_writes_to_opt_(cnt_dst, return) wchar_t *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const char *src,
        _In_ const std::size_t cnt_src,
        _In_ const narrow_string::code_page_type code_page);

    /// <summary>
    /// Converts a UTF-16 string to a string of the given encoding.
    /// </summary>
    /// <param name="dst">A buffer to receive the output that can hold at least
    /// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
    /// <paramref name="cnt_dst" /> is zero.</param>
    /// <param name="cnt_dst">The number of elements that can be stored to
    /// <paramref name="dst" />.</param>
    /// <param name="src">The source string.</param>
    /// <param name="cnt_src">The length of the source string, which can be zero
    /// if <paramref name="src" /> is null-terminated.</param>
    /// <param name="code_page">The code page used for the narrow string.</param>
    /// <returns>The number of characters required for the output string,
    /// including the terminating zero.</returns>
    /// <exception cref="std::invalid_argument">If <paramref name="src" /> is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    extern std::size_t DATAVERSE_API convert(
        _Out_writes_to_opt_(cnt_dst, return) char *dst,
        _In_ std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const wchar_t *src,
        _In_ const std::size_t cnt_src,
        _In_ const narrow_string::code_page_type code_page);

    /// <summary>
    /// Performs a trivial conversion (copy).
    /// </summary>
    /// <typeparam name="TChar"></typeparam>
    /// <param name="dst">A buffer to receive the output that can hold at least
    /// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
    /// <paramref name="cnt_dst" /> is zero.</param>
    /// <param name="cnt_dst">The number of elements that can be stored to
    /// <paramref name="dst" />.</param>
    /// <param name="src">The source string.</param>
    /// <param name="cnt_src">The length of the source string, which can be zero
    /// if <paramref name="src" /> is null-terminated.</param>
    /// <param name="code_page">This parameter is ignored.</param>
    /// <returns>The number of characters required for the output string,
    /// including the terminating zero.</returns>
    /// <exception cref="std::invalid_argument">If <paramref name="src" /> is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    template<class TChar> std::size_t convert(
        _Out_writes_to_opt_(cnt_dst, return) TChar *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const TChar *src,
        _In_ std::size_t cnt_src,
        _In_ const narrow_string::code_page_type code_page);

    /// <summary>
    /// Converts a C-style string into an STL string.
    /// </summary>
    /// <typeparam name="TDstChar"></typeparam>
    /// <typeparam name="TSrcChar"></typeparam>
    /// <param name="src"></param>
    /// <param name="cnt_src"></param>
    /// <param name="code_page"></param>
    /// <returns></returns>
    template<class TDstChar, class TSrcChar>
    inline std::basic_string<TDstChar> convert(
            _In_reads_or_z_(cnt_src) const TSrcChar *src,
            _In_ const std::size_t cnt_src,
            _In_ const narrow_string::code_page_type code_page) {
        std::vector<TDstChar> buffer(convert(
            static_cast<TDstChar *>(nullptr), 0,
            src, cnt_src,
            code_page));
        auto cnt = convert(buffer.data(), buffer.size(),
            src, cnt_src,
            code_page);
        return std::basic_string<TDstChar>(buffer.data(),
            buffer.data() + cnt - 1);
    }

    /// <summary>
    /// Converts a <see cref="narrow_string" /> or
    /// <see cref="const_narrow_string" /> into an STL string.
    /// </summary>
    /// <typeparam name="TDstChar"></typeparam>
    /// <param name="src"></param>
    /// <param name="cnt_src"></param>
    /// <returns></returns>
    template<class TDstChar>
    inline std::basic_string<TDstChar> convert(
            _In_ const const_narrow_string& src,
            _In_ const std::size_t cnt_src = 0) {
        return convert<TDstChar>(src.value(), cnt_src, src.code_page());
    }

    /// <summary>
    /// Converts a C-style string into an STL string.
    /// </summary>
    /// <typeparam name="TChar">The type of the characters.</typeparam>
    /// <param name="src">The source string.</param>
    /// <param name="cnt_src">The number of characters in the source string to
    /// convert or zero for converting a full null-terminated string.</param>
    /// <param name="code_page">This parameter is unused and is only for API
    /// compatibility.</param>
    /// <returns>The converted string.</returns>
    template<class TChar> inline std::basic_string<TChar> convert(
            _In_reads_or_z_(cnt_src) const TChar *src,
            _In_ const std::size_t cnt_src,
            _In_ const narrow_string::code_page_type) {
        return (cnt_src > 0)
            ? std::basic_string<TChar>(src, src + cnt_src)
            : std::basic_string<TChar>(src);
    }

    /// <summary>
    /// Converts between two STL strings.
    /// </summary>
    /// <typeparam name="TDstChar"></typeparam>
    /// <typeparam name="TSrcChar"></typeparam>
    /// <typeparam name="TSrcTraits"></typeparam>
    /// <typeparam name="TSrcAlloc"></typeparam>
    /// <param name="src"></param>
    /// <param name="code_page"></param>
    /// <returns></returns>
    template<class TDstChar, class TSrcChar, class TSrcTraits, class TSrcAlloc>
    inline std::basic_string<TDstChar> convert(
            _In_ const std::basic_string<TSrcChar, TSrcTraits, TSrcAlloc>& src,
            _In_ const narrow_string::code_page_type code_page) {
        return convert<TDstChar>(src.c_str(), 0, code_page);
    }

    /// <summary>
    /// Convert the given UTF-8 encoded string to the specified character type.
    /// </summary>
    /// <typeparam name="TDstChar"></typeparam>
    /// <param name="src"></param>
    /// <returns></returns>
    template<class TDstChar>
    inline std::basic_string<TDstChar> from_utf8(_In_z_ const char *src) {
#if defined(_WIN32)
        return convert<TDstChar>(src, CP_UTF8);
#else /* defined(_WIN32) */
        return convert<TDstChar>(src, "UTF-8");
#endif /* defined(_WIN32) */
    }

    /// <summary>
    /// Convert the given UTF-8 encoded string to the specified character type.
    /// </summary>
    /// <typeparam name="TDstChar"></typeparam>
    /// <typeparam name="TSrcChar"></typeparam>
    /// <typeparam name="TSrcTraits"></typeparam>
    /// <typeparam name="TSrcAlloc"></typeparam>
    /// <param name="src"></param>
    /// <returns></returns>
    template<class TDstChar, class TSrcChar, class TSrcTraits, class TSrcAlloc>
    inline std::basic_string<TDstChar> from_utf8(_In_ const std::basic_string<
            TSrcChar, TSrcTraits, TSrcAlloc>& src) {
        return from_utf8<TDstChar>(src.c_str());
    }

    /// <summary>
    /// Converts a narrow string using the specified code page to ASCII.
    /// </summary>
    /// <param name="dst">A buffer to receive the output that can hold at least
    /// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
    /// <paramref name="cnt_dst" /> is zero.</param>
    /// <param name="cnt_dst">The number of elements that can be stored to
    /// <paramref name="dst" />.</param>
    /// <param name="src">The source string.</param>
    /// <param name="cnt_src">The length of the source string, which can be zero
    /// if <paramref name="src" /> is null-terminated.</param>
    /// <param name="code_page">The code page used for the narrow string.</param>
    /// <returns>The number of characters required for the output string,
    /// not including the terminating zero.</returns>
    /// <exception cref="std::invalid_argument">If <paramref name="src" /> is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    extern std::size_t DATAVERSE_API to_ascii(
        _Out_writes_to_opt_(cnt_dst, return) char *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const char *src,
        _In_ const std::size_t cnt_src,
        _In_ const narrow_string::code_page_type code_page);

    /// <summary>
    /// Converts a narrow string using the specified code page to ASCII.
    /// </summary>
    /// <param name="src">The source string to be converted.</param>
    /// <param name="code_page">The code page used for <see cref="src" />.
    /// </param>
    /// <returns>The ASCII version of the input.</returns>
    /// <exception cref="std::invalid_argument">If the input is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    std::string to_ascii(_In_z_ const char *src,
        _In_ const narrow_string::code_page_type code_page);

    /// <summary>
    /// Converts a narrow string using the specified code page to ASCII.
    /// </summary>
    /// <param name="src">The source string to be converted.</param>
    /// <returns>The ASCII version of the input.</returns>
    /// <exception cref="std::invalid_argument">If the input is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    inline std::string to_ascii(_In_z_ const const_narrow_string& src) {
        return to_ascii(src, src.code_page());
    }

    /// <summary>
    /// Converts a Unicode string to ASCII.
    /// </summary>
    /// <param name="dst">A buffer to receive the output that can hold at least
    /// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
    /// <paramref name="cnt_dst" /> is zero.</param>
    /// <param name="cnt_dst">The number of elements that can be stored to
    /// <paramref name="dst" />.</param>
    /// <param name="src">The source string.</param>
    /// <param name="cnt_src">The length of the source string, which can be zero
    /// if <paramref name="src" /> is null-terminated.</param>
    /// <returns>The number of characters required for the output string,
    /// not including the terminating zero.</returns>
    /// <exception cref="std::invalid_argument">If <paramref name="src" /> is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    inline std::size_t DATAVERSE_API to_ascii(
            _Out_writes_to_opt_(cnt_dst, return) char *dst,
            _In_ const std::size_t cnt_dst,
            _In_reads_or_z_(cnt_src) const wchar_t *src,
            _In_ const std::size_t cnt_src) {
#if defined(_WIN32)
        return convert(dst, cnt_dst, src, cnt_src, CP_ACP);
#else /* defined(_WIN32) */
        return convert(dst, cnt_dst, src, cnt_src, "ASCII");
#endif /* defined(_WIN32) */
    }

    /// <summary>
    /// Converts a wide string using the platform's default code page.
    /// </summary>
    /// <param name="src">The source string to be converted.</param>
    /// <returns>The ASCII version of the input.</returns>
    /// <exception cref="std::invalid_argument">If the input is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    inline std::string to_ascii(_In_z_ const wchar_t *src) {
#if defined(_WIN32)
        return convert<char>(src, 0, CP_ACP);
#else /* defined(_WIN32) */
        return convert<char>(src, 0, "ASCII");
#endif /* defined(_WIN32) */
    }

    /// <summary>
    /// Converts a narrow string using the platform's default code page.
    /// </summary>
    /// <typeparam name="TSrcChar"></typeparam>
    /// <typeparam name="TSrcTraits"></typeparam>
    /// <typeparam name="TSrcAlloc"></typeparam>
    /// <param name="src">The source string to be converted.</param>
    /// <returns>The ASCII version of the input.</returns>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    template<class TSrcChar, class TSrcTraits, class TSrcAlloc>
    inline std::string to_ascii(_In_ const std::basic_string<TSrcChar,
            TSrcTraits, TSrcAlloc>& src) {
        return to_ascii(src.c_str());
    }

    /// <summary>
    /// Converts a narrow string using the specified code page to ANSI.
    /// </summary>
    /// <param name="src">The source string to be converted.</param>
    /// <param name="code_page">The code page used for <see cref="src" />.
    /// </param>
    /// <returns>The ANSI version of the input.</returns>
    /// <exception cref="std::invalid_argument">If the input is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    inline std::string to_ansi(_In_z_ const char *src,
            _In_ const narrow_string::code_page_type code_page) {
#if defined(_WIN32)
        return convert<char>(convert<wchar_t>(src, 0, code_page), CP_ACP);
#else /* defined(_WIN32) */
        return convert<char>(convert<wchar_t>(src, 0, code_page), "CP1252");
#endif /* defined(_WIN32) */
    }

    /// <summary>
    /// Converts a narrow string using the specified code page to ANSI.
    /// </summary>
    /// <param name="src">The source string to be converted.</param>
    /// <returns>The ANSI version of the input.</returns>
    /// <exception cref="std::invalid_argument">If the input is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    inline std::string to_ansi(_In_z_ const const_narrow_string& src) {
        return to_ansi(src, src.code_page());
    }

    /// <summary>
    /// Converts a wide string to ANSI.
    /// </summary>
    /// <param name="src">The source string toe b converted.</param>
    /// <returns>The ANSI version of the input.</returns>
    /// <exception cref="std::invalid_argument">If the input is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    inline std::string to_ansi(_In_z_ const wchar_t *src) {
#if defined(_WIN32)
        return convert<char>(src, 0, CP_ACP);
#else /* defined(_WIN32) */
        return convert<char>(src, 0, "CP1252");
#endif /* defined(_WIN32) */
    }

    /// <summary>
    /// Converts the given string to ANSI.
    /// </summary>
    /// <typeparam name="TSrcChar"></typeparam>
    /// <typeparam name="TSrcTraits"></typeparam>
    /// <typeparam name="TSrcAlloc"></typeparam>
    /// <param name="src"></param>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    template<class TSrcChar, class TSrcTraits, class TSrcAlloc>
    inline std::string to_ansi(
            _In_ const std::basic_string<TSrcChar, TSrcTraits, TSrcAlloc>& src) {
        return to_ansi(src.c_str());
    }

    /// <summary>
    /// Converts the given narrow string string to UTF-8.
    /// </summary>
    /// <param name="src"></param>
    /// <param name="code_page"></param>
    /// <returns></returns>
    inline std::string to_utf8(_In_z_ const char *src,
            _In_ const narrow_string::code_page_type code_page) {
#if defined(_WIN32)
        return convert<char>(convert<wchar_t>(src, 0, code_page), CP_UTF8);
#else /* defined(_WIN32) */
        return convert<char>(convert<wchar_t>(src, 0, code_page), "UTF-8");
#endif /* defined(_WIN32) */
    }

    /// <summary>
    /// Converts the given narrow string string to UTF-8.
    /// </summary>
    /// <param name="src"></param>
    /// <param name="code_page"></param>
    /// <returns></returns>
    inline std::string to_utf8(_In_z_ const const_narrow_string& src) {
        return to_utf8(src, src.code_page());
    }

    /// <summary>
    /// Converts the given UTF-16 string to UTF-8.
    /// </summary>
    /// <param name="src"></param>
    /// <returns></returns>
    inline std::string to_utf8(_In_z_ const wchar_t *src) {
#if defined(_WIN32)
        return convert<char>(src, 0, CP_UTF8);
#else /* defined(_WIN32) */
        return convert<char>(src, 0, "UTF-8");
#endif /* defined(_WIN32) */
    }

    /// <summary>
    /// Converts the given string to UTF-8.
    /// </summary>
    /// <typeparam name="TSrcChar"></typeparam>
    /// <typeparam name="TSrcTraits"></typeparam>
    /// <typeparam name="TSrcAlloc"></typeparam>
    /// <param name="src"></param>
    /// <returns></returns>
    template<class TSrcChar, class TSrcTraits, class TSrcAlloc>
    inline std::string to_utf8(_In_ const std::basic_string<TSrcChar,
            TSrcTraits, TSrcAlloc>& src) {
        return to_utf8<TDstChar>(src.c_str());
    }

} /* namespace dataverse */
} /* namespace visus */

#include "dataverse/convert.inl"
