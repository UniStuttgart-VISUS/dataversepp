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

#include "dataverse/api.h"
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
    /// <param name="cnt_src">The length of the source string, which can be -1
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
        _In_ const int cnt_src,
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
    /// <param name="cnt_src">The length of the source string, which can be -1
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
        _In_ const int cnt_src,
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
    /// <param name="cnt_src">The length of the source string, which can be -1
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
        _In_ int cnt_src,
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
            _In_ const int cnt_src,
            _In_ const narrow_string::code_page_type code_page) {
        typedef std::basic_string<TDstChar> retval_type;
        std::vector<TDstChar> buffer(convert(
            static_cast<TDstChar *>(nullptr), 0,
            src, cnt_src,
            code_page));
        auto cnt = convert(buffer.data(), buffer.size(),
            src, cnt_src,
            code_page);
        return (cnt > 0)
            ? retval_type(buffer.data(), buffer.data() + cnt - 1)
            : retval_type();
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
            _In_ const std::size_t cnt_src = -1) {
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
        typedef std::basic_string<TDstChar> retval_type;
        return (cnt_src >= 0)
            ? retval_type(src, src + cnt_src)
            : retval_type(src);
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
        return convert<TDstChar, TSrcChar>(src.c_str(), -1, code_page);
    }

    /// <summary>
    /// Convert the given UTF-8 encoded string to the specified character type.
    /// </summary>
    /// <typeparam name="TDstChar"></typeparam>
    /// <param name="src"></param>
    /// <returns></returns>
    template<class TDstChar>
    inline std::basic_string<TDstChar> from_utf8(_In_z_ const char *src) {
        return convert<TDstChar>(src, -1, utf8_code_page);
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
    /// <param name="cnt_src">The length of the source string, which can be -1
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
        _In_ int cnt_src,
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
    inline std::string to_ascii(_In_z_ const char *src,
            _In_ const narrow_string::code_page_type code_page) {
        std::vector<char> buffer(to_ascii(nullptr, 0, src, -1, code_page));
        auto cnt = to_ascii(buffer.data(), buffer.size(), src, -1, code_page);
        return std::string(buffer.data(), buffer.data() + cnt);
    }

    /// <summary>
    /// Converts a narrow string using the specified code page to ASCII.
    /// </summary>
    /// <param name="src">The source string to be converted.</param>
    /// <returns>The ASCII version of the input.</returns>
    /// <exception cref="std::invalid_argument">If the input is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    inline std::string to_ascii(_In_ const const_narrow_string& src) {
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
    /// <param name="cnt_src">The length of the source string, which can be -1
    /// if <paramref name="src" /> is null-terminated.</param>
    /// <returns>The number of characters required for the output string,
    /// not including the terminating zero.</returns>
    /// <exception cref="std::invalid_argument">If <paramref name="src" /> is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    std::size_t DATAVERSE_API to_ascii(
        _Out_writes_to_opt_(cnt_dst, return) char *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const wchar_t *src,
        _In_ const int cnt_src);

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
        std::vector<char> buffer(to_ascii(nullptr, 0, src, -1));
        auto cnt = to_ascii(buffer.data(), buffer.size(), src, -1);
        return std::string(buffer.data(), buffer.data() + cnt);
    }

    /// <summary>
    /// Converts a narrow string using the platform's default code page.
    /// </summary>
    /// <typeparam name="TSrcChar"></typeparam>
    /// <typeparam name="TTraits"></typeparam>
    /// <typeparam name="TAlloc"></typeparam>
    /// <param name="src">The source string to be converted.</param>
    /// <returns>The ASCII version of the input.</returns>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    template<class TSrcTraits, class TSrcAlloc>
    inline std::string to_ascii(_In_ const std::basic_string<wchar_t,
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
        return convert<char>(convert<wchar_t>(src, -1, code_page),
            ansi_code_page);
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
    inline std::string to_ansi(_In_ const const_narrow_string& src) {
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
        return convert<char>(src, -1, ansi_code_page);
    }

    /// <summary>
    /// Converts the given string to ANSI.
    /// </summary>
    /// <typeparam name="TTraits"></typeparam>
    /// <typeparam name="TAlloc"></typeparam>
    /// <param name="src"></param>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    template<class TSrcTraits, class TSrcAlloc>
    inline std::string to_ansi(_In_ const std::basic_string<wchar_t,
            TSrcTraits, TSrcAlloc>& src) {
        return to_ansi(src.c_str());
    }

    /// <summary>
    /// Converts the given string to ANSI.
    /// </summary>
    /// <typeparam name="TTraits"></typeparam>
    /// <typeparam name="TAlloc"></typeparam>
    /// <param name="src"></param>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    template<class TSrcTraits, class TSrcAlloc>
    inline std::string to_ansi(_In_ const std::basic_string<char,
            TSrcTraits, TSrcAlloc>& src,
            _In_ const narrow_string::code_page_type code_page) {
        return to_ansi(src.c_str(), code_page);
    }

    /// <summary>
    /// Converts the given narrow string string to UTF-8.
    /// </summary>
    /// <param name="src"></param>
    /// <param name="code_page"></param>
    /// <returns></returns>
    inline std::string to_utf8(_In_z_ const char *src,
            _In_ const narrow_string::code_page_type code_page) {
        return convert<char>(convert<wchar_t>(src, -1, code_page),
            utf8_code_page);
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
    /// <param name="dst">A buffer to receive the output that can hold at least
    /// <paramref name="cnt_dst" /> characters. This can be <c>nullptr</c> if
    /// <paramref name="cnt_dst" /> is zero.</param>
    /// <param name="cnt_dst">The number of elements that can be stored to
    /// <paramref name="dst" />.</param>
    /// <param name="src">The source string.</param>
    /// <param name="cnt_src">The length of the source string, which can be -1
    /// if <paramref name="src" /> is null-terminated.</param>
    /// <returns>The number of characters required for the output string,
    /// including the terminating zero.</returns>
    /// <exception cref="std::invalid_argument">If <paramref name="src" /> is
    /// <c>nullptr</c>.</exception>
    /// <exception cref="std::system_error">If the conversion failed.
    /// </exception>
    inline std::size_t to_utf8(_Out_writes_to_opt_(cnt_dst, return) char *dst,
            _In_ std::size_t cnt_dst,
            _In_reads_or_z_(cnt_src) const wchar_t *src,
            _In_ const int cnt_src) {
        return convert(dst, cnt_dst, src, cnt_src, utf8_code_page);
    }

    /// <summary>
    /// Converts the given UTF-16 string to UTF-8.
    /// </summary>
    /// <param name="src"></param>
    /// <returns></returns>
    inline std::string to_utf8(_In_z_ const wchar_t *src) {
        return convert<char>(src, -1, utf8_code_page);
    }

    /// <summary>
    /// Converts the given string to UTF-8.
    /// </summary>
    /// <typeparam name="TSrcTraits">The traits of the string.</typeparam>
    /// <typeparam name="TSrcAlloc">The allocator of the string.</typeparam>
    /// <param name="src">The string to be converted.</param>
    /// <returns>The string in UTF-8 encoding.</returns>
    template<class TSrcTraits, class TSrcAlloc>
    inline std::string to_utf8(_In_ const std::basic_string<wchar_t,
            TSrcTraits, TSrcAlloc>& src) {
        return to_utf8(src.c_str());
    }

} /* namespace dataverse */
} /* namespace visus */

#include "dataverse/convert.inl"
