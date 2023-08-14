// <copyright file="tls_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "tls_context.h"

#include <cassert>

#include "com_error_category.h"
#include "io_completion_port.h"
#include "openssl_error_category.h"


/*
 * visus::dataverse::detail::tls_context::tls_context
 */
visus::dataverse::detail::tls_context::tls_context(
        _Inout_ credentials_handle_type&& handle,
        _Inout_ security_context_type&& context)
    : _handle(std::move(handle)), _context(std::move(context)) { }


/*
 * visus::dataverse::detail::tls_context::shutdown
 */
void visus::dataverse::detail::tls_context::shutdown(
        _In_ dataverse_connection *connection) {
    assert(connection != nullptr);
#if defined(_WIN32)
    DWORD message = SCHANNEL_SHUTDOWN;

    SecBuffer in_buffer;
    in_buffer.BufferType = SECBUFFER_TOKEN;
    in_buffer.pvBuffer = &message;
    in_buffer.cbBuffer = sizeof(message);

    SecBufferDesc in_desc;
    in_desc.ulVersion = SECBUFFER_VERSION;
    in_desc.cBuffers = 1;
    in_desc.pBuffers = &in_buffer;

    {
        auto status = ::ApplyControlToken(this->_context.addressof(), &in_desc);
        if (status < 0) {
            throw std::system_error(status, com_category());
        }
    }

    SecBuffer out_buffer = { 0 };
    out_buffer.BufferType = SECBUFFER_TOKEN;
    assert(out_buffer.pvBuffer == nullptr);

    SecBufferDesc out_desc;
    out_desc.ulVersion = SECBUFFER_VERSION;
    out_desc.cBuffers = 1;
    out_desc.pBuffers = &out_buffer;

    DWORD flags = ISC_REQ_ALLOCATE_MEMORY
        | ISC_REQ_CONFIDENTIALITY
        | ISC_REQ_REPLAY_DETECT
        | ISC_REQ_SEQUENCE_DETECT
        | ISC_REQ_STREAM;

    auto free_output_buffer = wil::scope_exit([out_buffer](void) {
        if (out_buffer.pvBuffer != nullptr) {
            ::FreeContextBuffer(out_buffer.pvBuffer);
        }
    });

    {
        auto status = ::InitializeSecurityContextW(this->_handle.addressof(),
            this->_context.addressof(),
            nullptr,
            flags,
            0,
            0,
            &in_desc,
            0,
            nullptr,
            &out_desc,
            &flags,
            nullptr);
        if (status == SEC_E_OK) {
            // Send the final package, but we don't care about when it arrives
            // or whether it arrives at all.
            io_completion_port::instance().send(
                connection,
                out_buffer.pvBuffer,
                out_buffer.cbBuffer,
                nullptr,
                nullptr,
                nullptr,
                nullptr);
        } else if (status < 0) {
            throw std::system_error(status, com_category());
        }
    }
#endif /* defined(_WIN32) */
}
