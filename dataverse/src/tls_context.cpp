// <copyright file="tls_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "tls_context.h"

#include <algorithm>
#include <cassert>

#include "com_error_category.h"
#include "io_completion_port.h"
#include "openssl_error_category.h"
#include "tls_handshake.h"


/*
 * visus::dataverse::detail::tls_context::tls_context
 */
visus::dataverse::detail::tls_context::tls_context(void) {
#if defined(_WIN32)
    this->_flags = 0;
    ::ZeroMemory(&this->_sizes, sizeof(this->_sizes));
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::detail::tls_context::decrypt
 */
visus::dataverse::detail::tls_context::decrypted_type
visus::dataverse::detail::tls_context::decrypt(
        _In_reads_bytes_(size) const void *data,
        _In_ const std::size_t size) {
    assert(data != nullptr);

    // Cf. https://gist.github.com/mmozeiko/c0dfcc8fec527a90a02145d2cc0bfb6d
    SecBuffer buffers[4] = { 0 };
    buffers[0].BufferType = SECBUFFER_DATA;
    buffers[0].pvBuffer = const_cast<void *>(data);
    buffers[0].cbBuffer = static_cast<unsigned long>(size);
    buffers[1].BufferType = SECBUFFER_EMPTY;
    buffers[2].BufferType = SECBUFFER_EMPTY;
    buffers[3].BufferType = SECBUFFER_EMPTY;

    SecBufferDesc desc;
    desc.ulVersion = SECBUFFER_VERSION;
    desc.cBuffers = ARRAYSIZE(buffers);
    desc.pBuffers = buffers;
    assert(this->_sizes.cBuffers == desc.cBuffers);

    auto status = ::DecryptMessage(this->_context.addressof(), &desc, 0,
        nullptr);
    switch (status) {
        case SEC_E_OK:
            assert(buffers[0].BufferType == SECBUFFER_STREAM_HEADER);
            assert(buffers[1].BufferType == SECBUFFER_DATA);
            assert(buffers[2].BufferType == SECBUFFER_STREAM_TRAILER);
            return decrypted_type(make_vector(buffers[1]),
                make_vector(buffers[1], &buffers[3]));

        case SEC_I_CONTEXT_EXPIRED:
            // Connection is being torn down, but not yet completely closed.
            return decrypted_type();

        case SEC_E_INCOMPLETE_MESSAGE:
            // Cannot decrypt, but need more input.
            return decrypted_type(make_vector(), make_vector(data, size));

        case SEC_I_RENEGOTIATE:
            // Server wants to renegotiate.
            // Cf. https://learn.microsoft.com/en-us/windows/win32/secauthn/recognizing-a-request-to-renegotiate-a-connection
            return decrypted_type(make_vector(buffers[1]),
                make_vector(buffers[3]), this);

        default:
            throw std::system_error(status, com_category());
    }
}


/*
 * visus::dataverse::detail::tls_context::send
 */
void visus::dataverse::detail::tls_context::send(
        _In_ dataverse_connection *connection,
        _In_reads_bytes_(size) const void *data,
        _In_ const std::size_t size,
        _In_ const decltype(io_context::on_succeded.sent) on_sent,
        _In_ const decltype(io_context::on_failed) on_failed,
        _In_ const decltype(io_context::on_disconnected) on_disconnected,
        _In_opt_ void *context) {
    assert(connection != nullptr);
    assert(data != nullptr);
    assert(size <= ULONG_MAX);
    auto& ioc = io_completion_port::instance();
    auto cur = static_cast<const io_context::byte_type *>(data);
    auto rem = static_cast<unsigned long>(size);

    while (rem > 0) {
        auto payload = (std::min)(rem, this->_sizes.cbMaximumMessage);
        auto packet = ioc.context(io_operation::send,
            this->_sizes.cbHeader + payload + this->_sizes.cbTrailer,
            on_failed,
            on_disconnected,
            context);
        packet->on_succeded.sent = on_sent;

        // Cf. https://gist.github.com/mmozeiko/c0dfcc8fec527a90a02145d2cc0bfb6d
        SecBuffer buffers[3];
        buffers[0].BufferType = SECBUFFER_STREAM_HEADER;
        buffers[0].pvBuffer = packet->payload();
        buffers[0].cbBuffer = this->_sizes.cbHeader;
        buffers[1].BufferType = SECBUFFER_DATA;
        buffers[1].pvBuffer = packet->payload() + this->_sizes.cbHeader;
        buffers[1].cbBuffer = payload;
        buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;
        buffers[2].pvBuffer = packet->payload() + this->_sizes.cbHeader
            + payload;
        buffers[2].cbBuffer = this->_sizes.cbTrailer;

        ::memcpy(buffers[1].pvBuffer, cur, payload);

        SecBufferDesc desc;
        desc.ulVersion = SECBUFFER_VERSION;
        desc.cBuffers = ARRAYSIZE(buffers);
        desc.pBuffers = buffers;

        {
            auto status = ::EncryptMessage(this->_context.addressof(), 0,
                &desc, 0);
            if (status < 0) {
                throw std::system_error(status, com_category());
            }
        }

        ioc.send(connection, std::move(packet));

        cur += payload;
        rem -= payload;
    }
}


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

    auto free_output_buffer = wil::scope_exit([out_buffer](void) {
        if (out_buffer.pvBuffer != nullptr) {
            ::FreeContextBuffer(out_buffer.pvBuffer);
        }
    });

    {
        auto status = ::InitializeSecurityContextW(this->_handle.addressof(),
            this->_context.addressof(),
            nullptr,
            this->_flags,
            0,
            0,
            &in_desc,
            0,
            nullptr,
            &out_desc,
            &this->_flags,
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


/*
 * visus::dataverse::detail::tls_context::make_vector
 */
std::vector<visus::dataverse::detail::tls_context::byte_type>
visus::dataverse::detail::tls_context::make_vector(
        _In_ const SecBuffer& buffer, _In_ const SecBuffer *extra) {
    if (buffer.BufferType == SECBUFFER_EMPTY) {
        // Buffer is semantically empty.
        return make_vector();
    }

    if (buffer.pvBuffer == nullptr) {
        // Buffer is actually empty.
        return make_vector();
    }

    if (extra != nullptr) {
        // The remainder was requested rather than the output.
        if ((extra->BufferType == SECBUFFER_EXTRA)
                && (buffer.cbBuffer > extra->cbBuffer)) {
            auto d = static_cast<const byte_type *>(buffer.pvBuffer);
            return std::vector<byte_type>(
                d + buffer.cbBuffer - extra->cbBuffer,
                d + buffer.cbBuffer);

        } else {
            return make_vector();
        }


    } else {
        // The normal case.
        return make_vector(buffer.pvBuffer, buffer.cbBuffer);
    }
}
