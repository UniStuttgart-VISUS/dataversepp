// <copyright file="tls_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "tls_context.h"

#include "openssl_error_category.h"


/*
 * visus::dataverse::detail::tls_context::tls_context
 */
visus::dataverse::detail::tls_context::tls_context(void)
//: _context(::SSL_CTX_new(::TLS_client_method())
{
    //if (this->_context == nullptr) {
    //    throw std::system_error(::ERR_get_error(), openssl_category());
    //}
}


/*
 * visus::dataverse::detail::tls_context::~tls_context
 */
visus::dataverse::detail::tls_context::~tls_context(void) {
    //if (this->_context != nullptr) {
    //    ::SSL_CTX_free(this->_context);
    //}
}
