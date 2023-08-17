// <copyright file="types.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>
#include <cstdlib>
#include <fstream>

#if defined(_WIN32)
#include <Windows.h>
#else /* defined(_WIN32) */
#define CALLBACK
#endif /* defined(_WIN32) */


namespace visus {
namespace dataverse {

#if defined(_WIN32)
    /// <summary>
    /// The type of a character.
    /// </summary>
    typedef wchar_t char_type;
#else /* defined(_WIN32) */
    typedef char char_type;
#endif /* defined(_WIN32) */

} /* namespace dataverse */
} /* namespace visus */


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
