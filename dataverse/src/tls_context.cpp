// <copyright file="tls_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "tls_context.h"

#include "com_error_category.h"
#include "openssl_error_category.h"


/*
 * visus::dataverse::detail::tls_context::tls_context
 */
visus::dataverse::detail::tls_context::tls_context(
        _Inout_ credentials_handle_type&& handle,
        _Inout_ security_context_type&& context)
    : _handle(std::move(handle)), _context(std::move(context)) { }
