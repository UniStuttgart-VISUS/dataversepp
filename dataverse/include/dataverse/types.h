// <copyright file="types.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>
#include <cstdlib>

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#include <tchar.h>
#endif /* defined(_WIN32) */


namespace visus {
namespace dataverse {

#if defined(_WIN32)
    /// <summary>
    /// The type of a character.
    /// </summary>
    typedef wchar_t char_type;

    /// <summary>
    /// The type of a native API error code.
    /// </summary>
    typedef DWORD system_error_code;

#else /* defined(_WIN32) */
    typedef char char_type;

    typedef int system_error_code;
#endif /* defined(_WIN32) */

} /* namespace dataverse */
} /* namespace visus */


#if (defined(__cplusplus) && (__cplusplus >= 202000))
/// <summary>
/// Defines a UTF-8 string literal.
/// </summary>
#define DATAVERSE_UTF8_LITERAL(s) u8##s
#else /* (defined(__cplusplus) && (__cplusplus >= 202000)) */
#define DATAVERSE_UTF8_LITERAL(s) s
#endif /* (defined(__cplusplus) && (__cplusplus >= 202000)) */


#if defined(_WIN32)
/// <summary>
/// Defines a string literal expected by the library's API.
/// </summary>
#define DATAVERSE_STRING_LITERAL(s) L##s
#else /* defined(_WIN32) */
#define DATAVERSE_STRING_LITERAL(s) s
#endif /* defined(_WIN32) */


/// <summary>
/// Defines a string literal expected by the library's API.
/// </summary>
#define DVSL(s) DATAVERSE_STRING_LITERAL(s)
