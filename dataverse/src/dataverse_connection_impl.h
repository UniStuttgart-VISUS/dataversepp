// <copyright file="io_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#include <wininet.h>

#include <wil/resource.h>
#endif /* defined(_WIN32) */


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// The head of an in-flight context of an asynchronous I/O operation.
    /// </summary>
    struct dataverse_connection_impl final {

        std::vector<char_type> api_key;
        std::basic_string<char_type> base_path;
        bool tls;

#if defined(_WIN32)
        wil::unique_hinternet connection;
        wil::unique_hinternet session;
#endif /* defined(_WIN32) */

        inline dataverse_connection_impl(void) : tls(false) { }
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
