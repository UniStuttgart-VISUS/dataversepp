// <copyright file="tls_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

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

        //SSL_CTX *_context;
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
