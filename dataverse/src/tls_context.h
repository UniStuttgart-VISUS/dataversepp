// <copyright file="tls_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#include <schannel.h>
#define SECURITY_WIN32
#include <sspi.h>

#include <wil/resource.h>
#else /* defined(_WIN32) */
#endif /* defined(_WIN32) */

#include "dataverse/convert.h"
#include "dataverse/types.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// A RAII container for OpenSSL.
    /// </summary>
    class tls_context final {

    public:

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        tls_context(void);

        /// <summary>
        /// Finalises the method.
        /// </summary>
        ~tls_context(void);

    private:

#if defined(_WIN32)
        wil::unique_struct<CtxtHandle, decltype(&::DeleteSecurityContext),
            ::DeleteSecurityContext> _context;
        wil::unique_struct<CredHandle, decltype(&::FreeCredentialsHandle),
            ::FreeCredentialsHandle> _handle;
#else /* defined(_WIN32) */
        //SSL_CTX *_context;
#endif /* defined(_WIN32) */

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
