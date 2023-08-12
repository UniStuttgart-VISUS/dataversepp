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
    typedef char16_t char_type;

    /// <summary>
    /// The type of a native API error code.
    /// </summary>
    typedef DWORD system_error_code;

#else /* defined(_WIN32) */
    typedef char8_t char_type;
    typedef int system_error_code;
#endif /* defined(_WIN32) */

} /* namespace dataverse */
} /* namespace visus */
