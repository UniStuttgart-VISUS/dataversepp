// <copyright file="winsock_scope.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "dataverse/api.h"
#include "dataverse/types.h"


#if defined(__cplusplus)
namespace visus {
namespace dataverse {

    /// <summary>
    /// RAII wrapper around Winsock for the calling thread.
    /// </summary>
    class DATAVERSE_API winsock_scope final {

    public:

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name="major">The major version of the WinSock API being
        /// requested, which defaults to 2.</param>
        /// <param name="minor">The minor version of the WinSock API being
        /// requested, which defaults to 2.</param>
        /// <exception cref="std::system_error">In case the library could not
        /// be loaded.</exception>
        winsock_scope(_In_ const std::uint8_t major = 2,
            _In_ const std::uint8_t minor = 2);

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        winsock_scope(const winsock_scope&) = delete;

        ~winsock_scope(void) noexcept;

        winsock_scope& operator =(const winsock_scope&) = delete;
    };

} /* namespace dataverse */
} /* namespace visus */
#endif /* defined(__cplusplus) */
