// <copyright file="tls_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "tls_context.h"

#include <algorithm>
#include <cassert>
#include <iterator>

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
 * visus::dataverse::detail::tls_context::message_size
 */
std::size_t visus::dataverse::detail::tls_context::message_size(void) const {
#if defined(_WIN32)
    return this->_sizes.cbMaximumMessage;
#else /* defined(_WIN32) */
    throw "TODO";
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::detail::tls_context::receive
 */
void visus::dataverse::detail::tls_context::receive(
        _In_ dataverse_connection *connection,
        _In_ const std::size_t size,
        _In_ const decltype(io_context::on_received) on_received,
        _In_ const decltype(io_context::on_failed) on_failed,
        _In_opt_ void *client_data) {
    assert(connection != nullptr);

    auto response_handler = [this, on_received, on_failed](
            _In_ dataverse_connection *c,
            _In_ const std::size_t cnt,
            _In_ detail::io_context *i) {
        try {
            auto response = this->decrypt(i->payload(), cnt);
            if (response != nullptr) {
                response->on_failed = on_failed;
                response->operation_receive(on_received);
                response->client_data = i->client_data;
                if (!this->_pending_decrypt.empty()) {
                    // We need more data to decrypt the pending input.
                    return this->message_size();
                } else {
                    auto retval = response->invoke_on_received(c,
                        response->buffer.len);
                    io_completion_port::instance().recycle(std::move(response));
                }
                // TODO: Can we recycle the response somehow?
            } else {
                // If we have no response and decrypt did not throw, we need
                // more data from the server, so request another receive.
                return this->message_size();
            }

        } catch (std::system_error ex) {
            i->invoke_on_failed(c, ex);
        }

        // If we are here, decrypt failed or the connection was terminated, so
        // we do not want the completion port to post another receive.
        return static_cast<std::size_t>(0);
    };

    io_completion_port::instance().receive(connection, this->message_size(),
        response_handler, on_failed, client_data);
}


/*
 * visus::dataverse::detail::tls_context::send
 */
void visus::dataverse::detail::tls_context::send(
        _In_ dataverse_connection *connection,
        _In_reads_bytes_(size) const void *data,
        _In_ const std::size_t size,
        _In_ const decltype(io_context::on_sent) on_sent,
        _In_ const decltype(io_context::on_failed) on_failed,
        _In_opt_ void *client_data) {
    assert(connection != nullptr);
    assert(data != nullptr);
    assert(size <= ULONG_MAX);
    auto cur = static_cast<const io_context::byte_type *>(data);
    auto rem = static_cast<unsigned long>(size);

    while (rem > 0) {
        auto payload = (std::min)(rem, this->_sizes.cbMaximumMessage);
        auto packet = io_completion_port::instance().context(
            this->_sizes.cbHeader + payload + this->_sizes.cbTrailer,
            on_failed,
            client_data);
        packet->operation_send(on_sent);

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

        io_completion_port::instance().send(connection, std::move(packet));

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
        _In_ const SecBuffer& buffer) {
    if (buffer.BufferType == SECBUFFER_EMPTY) {
        // Buffer is semantically empty.
        return std::vector<byte_type>();
    }

    if (buffer.pvBuffer == nullptr) {
        // Buffer is actually empty.
        return std::vector<byte_type>();
    }

 
    // The normal case.
    return make_vector(buffer.pvBuffer, buffer.cbBuffer);
}


/*
 * visus::dataverse::detail::tls_context::decrypt
 */
std::unique_ptr<visus::dataverse::detail::io_context>
visus::dataverse::detail::tls_context::decrypt(
        _In_reads_bytes_(size) const void *data,
        _In_ const std::size_t size) {
    std::unique_ptr<visus::dataverse::detail::io_context> retval;

    // Cf. https://gist.github.com/mmozeiko/c0dfcc8fec527a90a02145d2cc0bfb6d
    SecBuffer buffers[4] = { 0 };
    buffers[0].BufferType = SECBUFFER_DATA;
    buffers[1].BufferType = SECBUFFER_EMPTY;
    buffers[2].BufferType = SECBUFFER_EMPTY;
    buffers[3].BufferType = SECBUFFER_EMPTY;

    if (this->_pending_decrypt.empty()) {
        buffers[0].pvBuffer = const_cast<void *>(data);
        buffers[0].cbBuffer = static_cast<unsigned long>(size);
    } else {
        auto d = static_cast<const byte_type *>(data);
        auto s = this->_pending_decrypt.size() + size;
        this->_pending_decrypt.reserve(s);
        std::copy(d, d + size, std::back_inserter(this->_pending_decrypt));
        buffers[0].pvBuffer = this->_pending_decrypt.data();
        buffers[0].cbBuffer = static_cast<unsigned long>(s);
    }

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
            retval = io_completion_port::instance().context(buffers[1].cbBuffer,
                nullptr,
                nullptr);
            ::memcpy(retval->payload(),
                buffers[1].pvBuffer,
                buffers[1].cbBuffer);
            assert(retval->buffer.len == buffers[1].cbBuffer);
            this->_pending_decrypt = make_vector(buffers[3]);
            break;

        case SEC_I_CONTEXT_EXPIRED:
            // Connection is being torn down, but not yet completely closed.
            throw std::system_error(status, com_category());

        case SEC_E_INCOMPLETE_MESSAGE:
            // Cannot decrypt, but need more input.
            this->_pending_decrypt = make_vector(data, size);
            break;

        case SEC_I_RENEGOTIATE:
            // Server wants to renegotiate.
            // Cf. https://learn.microsoft.com/en-us/windows/win32/secauthn/recognizing-a-request-to-renegotiate-a-connection
            throw std::logic_error("Renegotiation is not implemented.");

        default:
            throw std::system_error(status, com_category());
    }

    return retval;
}
