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
visus::dataverse::detail::tls_context::tls_context(void) {
//: _context(::SSL_CTX_new(::TLS_client_method())
#if defined(_WIN32)
    {
        SCHANNEL_CRED cred;
        ::ZeroMemory(&cred, sizeof(cred));
        cred.dwVersion = SCHANNEL_CRED_VERSION;
        cred.dwFlags = SCH_USE_STRONG_CRYPTO
            | SCH_CRED_AUTO_CRED_VALIDATION
            | SCH_CRED_NO_DEFAULT_CREDS;
        cred.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT
            | SP_PROT_TLS1_3_CLIENT;

        auto status = ::AcquireCredentialsHandle(nullptr,
            UNISP_NAME,
            SECPKG_CRED_OUTBOUND,
            nullptr,
            &cred,
            nullptr,
            nullptr,
            this->_handle.addressof(),
            nullptr);
        if (status != SEC_E_OK) {
            throw std::system_error(status, com_category());
        }
    }


#else /* defined(_WIN32) */
#endif /* defined(_WIN32) */
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
