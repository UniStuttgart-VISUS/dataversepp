// <copyright file="convert.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/convert.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <system_error>


/*
 * isus::dataverse::default_code_page
 */
const visus::dataverse::code_page_type visus::dataverse::default_code_page
#if defined(_WIN32)
    = CP_OEMCP;
#else /* defined(_WIN32) */
    = nullptr
#endif /* defined(_WIN32) */


#if defined(_WIN32)
#include <Windows.h>
#else /* defined(_WIN32) */
#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/uloc.h>
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#endif /* defined(_WIN32) */


/*
 * visus::dataverse::convert
 */
std::size_t visus::dataverse::convert(
        _Out_writes_to_opt_(cnt_dst, return) char16_t *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const char *src,
        _In_ const std::size_t cnt_src,
        _In_ const code_page_type code_page) {
    // Sanity checks.
    if (src == nullptr) {
        throw std::invalid_argument("The string to convert cannot be null.");
    }

#if defined(_WIN32)
    _try{
        // Determine the size of the buffer in bytes we need for the UTF-16
        // string.
        auto retval = ::MultiByteToWideChar(code_page, 0,
            src, static_cast<int>(cnt_src),
            reinterpret_cast<wchar_t *>(dst), static_cast<int>(cnt_dst));

        if (retval <= 0) {
            throw std::system_error(::GetLastError(), std::system_category());
        }

        return retval;
    } _except(EXCEPTION_EXECUTE_HANDLER) {
        throw std::system_error(ERROR_NOACCESS, std::system_category());
    }
#else /* defined(_WIN32) */
    throw "TODO: implement convert for linux";

    UErrorCode status = U_ZERO_ERROR;
    auto conv = ::ucnv_open(code_page, &status);
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::convert
 */
std::size_t visus::dataverse::convert(
        _Out_writes_to_opt_(cnt_dst, return) char *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const char16_t *src,
        _In_ const std::size_t cnt_src,
        _In_ const code_page_type code_page) {
    // Sanity checks.
    if (src == nullptr) {
        throw std::invalid_argument("The string to convert cannot be null.");
    }

#if defined(_WIN32)
    _try {
        BOOL replacement = FALSE;
        auto r = ((code_page != CP_UTF7) && (code_page != CP_UTF8))
            ? &replacement
            : nullptr;

        auto retval = ::WideCharToMultiByte(code_page, 0,
            reinterpret_cast<const wchar_t *>(src), static_cast<int>(cnt_src),
            dst, static_cast<int>(cnt_dst),
            nullptr, r);

        if (retval <= 0) {
            throw std::system_error(::GetLastError(), std::system_category());
        }

        if (replacement) {
            // The input cannot be represented in the requested code page.
            throw std::system_error(ERROR_INVALID_PARAMETER,
                std::system_category());
        }

        return retval;
    } _except(EXCEPTION_EXECUTE_HANDLER) {
        throw std::system_error(ERROR_NOACCESS, std::system_category());
    }

#else /* defined(_WIN32) */
    throw "TODO: implement convert for linux";
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::convert
 */
std::size_t visus::dataverse::convert(
        _Out_writes_to_opt_(cnt_dst, return) char *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const char16_t *src,
        _In_ const std::size_t cnt_src) {
#if defined(_WIN32)
    return convert(dst, cnt_dst, src, cnt_src, CP_ACP);
#else /* defined(_WIN32) */
    throw "TODO: implement convert for linux";
#endif /* defined(_WIN32) */
}


#if (defined(__cplusplus) && (__cplusplus >= 202000))
/*
 * visus::dataverse::convert
 */
std::size_t visus::dataverse::convert(
        _Out_writes_to_opt_(cnt_dst, return) char8_t *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const char16_t *src,
        _In_ const std::size_t cnt_src) {
#if defined(_WIN32)
    return convert(reinterpret_cast<char *>(dst), cnt_dst, src, cnt_src,
        CP_UTF8);
#else /* defined(_WIN32) */
    throw "TODO: implement convert for linux";
#endif /* defined(_WIN32) */
}
#endif /* (defined(__cplusplus) && (__cplusplus >= 202000)) */


/*
 * visus::dataverse::to_ascii
 */
std::string visus::dataverse::to_ascii(_In_z_ const char *src,
        _In_ const code_page_type code_page) {
    if (src == nullptr) {
        throw std::invalid_argument("The string to convert cannot be null.");
    }

    // Simply check if any of the input characters exceeds the ASCII range.
    const auto invalid = std::any_of(src, src + ::strlen(src),
        [](const char c) {return (c > 127); });
    if (invalid) {
        throw std::system_error(ERROR_INVALID_PARAMETER,
            std::system_category());
    }

    return src;
}
