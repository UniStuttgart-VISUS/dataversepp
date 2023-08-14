// <copyright file="tls_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <functional>

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
    /// A RAII container for a TLS connection.
    /// </summary>
    class tls_context final {

    public:

#if defined(_WIN32)
        /// <summary>
        /// The type to store an SChannel credentials handle.
        /// </summary>
        typedef wil::unique_struct<CredHandle,
            decltype(&::FreeCredentialsHandle),
            ::FreeCredentialsHandle> credentials_handle_type;

        /// <summary>
        /// The type to store an SChannel security context.
        /// </summary>
        typedef wil::unique_struct<CtxtHandle,
            decltype(&::DeleteSecurityContext),
            ::DeleteSecurityContext> security_context_type;
#else /* defined(_WIN32) */
        //SSL_CTX *_context;
#endif /* defined(_WIN32) */

        tls_context(void) = default;

        tls_context(_Inout_ credentials_handle_type&& handle,
            _Inout_ security_context_type&& context);

    private:

#if defined(_WIN32)
        security_context_type _context;
        credentials_handle_type _handle;
#else /* defined(_WIN32) */
        //SSL_CTX *_context;
#endif /* defined(_WIN32) */

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
