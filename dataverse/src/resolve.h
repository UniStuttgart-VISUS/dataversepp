// <copyright file="resolve.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <vector>

#include "dataverse/socket.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// Resolves the specified end point.
    /// </summary>
    /// <param name="host"></param>
    /// <param name="service"></param>
    /// <param name="address_family"></param>
    /// <returns></returns>
    extern std::vector<sockaddr_storage> resolve(_In_z_ const char_type *host,
        _In_z_ const char_type *service,
        _In_ const int address_family = AF_UNSPEC);

    /// <summary>
    /// Resolves the specified end point.
    /// </summary>
    /// <param name="host"></param>
    /// <param name="port"></param>
    /// <param name="address_family"></param>
    /// <returns></returns>
    extern std::vector<sockaddr_storage> resolve(_In_z_ const char_type *host,
        _In_ const std::uint16_t port,
        _In_ const int address_family = AF_UNSPEC);

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
