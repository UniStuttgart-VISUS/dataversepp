// <copyright file="tls_handshake.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "tls_handshake.h"

#include <algorithm>
#include <iterator>

#include "com_error_category.h"
#include "io_completion_port.h"


/*
 * visus::dataverse::detail::tls_handshake::operator ()
 */
std::future<visus::dataverse::detail::tls_context>
visus::dataverse::detail::tls_handshake::operator ()(
        _In_ dataverse_connection *connection) {
#if defined(_WIN32)
    this->_context.reset();
    this->_flags = ISC_REQ_USE_SUPPLIED_CREDS
        | ISC_REQ_ALLOCATE_MEMORY
        | ISC_REQ_CONFIDENTIALITY
        | ISC_REQ_REPLAY_DETECT
        | ISC_REQ_SEQUENCE_DETECT
        | ISC_REQ_STREAM;
    this->_handle.reset();

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

    this->initialise(connection, true);

#else /* defined(_WIN32) */
#endif /* defined(_WIN32) */

    return this->_promise.get_future();
}


/*
 * visus::dataverse::detail::tls_handshake::on_network_disconnected
 */
void visus::dataverse::detail::tls_handshake::on_network_disconnected(
        _In_ dataverse_connection *connection,
        _In_opt_ void *context) {
    auto that = static_cast<tls_handshake *>(context);
    try {
        // TODO: linux
#if (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)))
        ::OutputDebugStringW(L"Server disconnected during TLS handshake.\r\n");
#endif /* (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG))) */
        throw std::system_error(WSAEDISCON, std::system_category());
    } catch (...) {
        that->_promise.set_exception(std::current_exception());
    }
}


/*
 * visus::dataverse::detail::tls_handshake::on_network_failed
 */
void visus::dataverse::detail::tls_handshake::on_network_failed(
        _In_ dataverse_connection *connection,
        _In_ const std::system_error& error,
        _In_opt_ void *context) {
    auto that = static_cast<tls_handshake *>(context);
    try {
#if (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)))
        ::OutputDebugStringW(L"Communication error during TLS handshake.\r\n");
#endif /* (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG))) */
        throw error;
    } catch (...) {
        that->_promise.set_exception(std::current_exception());
    }
}


/*
 * visus::dataverse::detail::tls_handshake::on_network_received
 */
void visus::dataverse::detail::tls_handshake::on_network_received(
        _In_ dataverse_connection *connection,
        _In_reads_bytes_(cnt) const io_context::byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ void *context) {
    auto that = static_cast<tls_handshake *>(context);
    try {
#if (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)))
        ::OutputDebugStringW(L"Received data for TLS handshake.\r\n");
#endif /* (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG))) */
        that->_input_buffer.reserve(that->_input_buffer.size() + cnt);
        std::copy(data, data + cnt, std::back_inserter(that->_input_buffer));
        that->initialise(connection, false);
    } catch (...) {
        that->_promise.set_exception(std::current_exception());
    }
}


/*
 * visus::dataverse::detail::tls_handshake::on_network_sent
 */
void visus::dataverse::detail::tls_handshake::on_network_sent(
        _In_ dataverse_connection *connection,
        _In_opt_ void *context) {
    auto that = static_cast<tls_handshake *>(context);
    try {
#if (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)))
        ::OutputDebugStringW(L"Sent data for TLS handshake.\r\n");
#endif /* (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG))) */
        that->initialise(connection, false);
    } catch (...) {
        that->_promise.set_exception(std::current_exception());
    }
}


/*
 * visus::dataverse::detail::tls_handshake::initialise
 */
void visus::dataverse::detail::tls_handshake::initialise(
        _In_ dataverse_connection *connection,
        _In_ const bool first) {
    std::lock_guard<decltype(this->_lock)> l(this->_lock);

    // From https://gist.github.com/mmozeiko/c0dfcc8fec527a90a02145d2cc0bfb6d
    constexpr auto max_packet_size = (16384 + 512);

    SecBuffer in_buffers[2] = { 0 };
    in_buffers[0].BufferType = SECBUFFER_TOKEN;
    in_buffers[0].pvBuffer = this->_input_buffer.data();
    in_buffers[0].cbBuffer = static_cast<DWORD>(this->_input_buffer.size());
    in_buffers[1].BufferType = SECBUFFER_EMPTY;

    SecBufferDesc in_desc;
    in_desc.ulVersion = SECBUFFER_VERSION;
    in_desc.cBuffers = ARRAYSIZE(in_buffers);
    in_desc.pBuffers = in_buffers;

    SecBuffer out_buffers[1] = { 0 };
    out_buffers[0].BufferType = SECBUFFER_TOKEN;

    auto clean_out_buffers = wil::scope_exit([&out_buffers] {
        if (out_buffers[0].pvBuffer != nullptr) {
            ::FreeContextBuffer(out_buffers[0].pvBuffer);
        }
    });

    SecBufferDesc out_desc;
    out_desc.ulVersion = SECBUFFER_VERSION;
    out_desc.cBuffers = ARRAYSIZE(out_buffers);
    out_desc.pBuffers = out_buffers;

    auto hostname = reinterpret_cast<SEC_WCHAR *>(&this->_hostname[0]);
    auto status = ::InitializeSecurityContextW(
        this->_handle.addressof(),
        !first ? this->_context.addressof() : nullptr,
        !first ? nullptr : hostname,
        this->_flags,
        0,
        0,
        !first ? &in_desc : nullptr,
        0,
        !first ? nullptr : this->_context.addressof(),
        &out_desc,
        &this->_flags,
        nullptr);

    switch (status) {
        case SEC_E_OK:
            // Handshake complete, fulfil the promise.
#if (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)))
            ::OutputDebugStringW(L"TLS handshake complete.\r\n");
#endif /* (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG))) */
            this->_promise.set_value(tls_context(
                std::move(this->_handle),
                std::move(this->_context)));
            break;

        case SEC_I_INCOMPLETE_CREDENTIALS:
            // Server asked for a client certificate, which we currently do
            // not implement.
#if (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)))
            ::OutputDebugStringW(L"Server needs client certificate.\r\n");
#endif /* (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG))) */
            throw std::system_error(E_NOTIMPL, com_category());

        case SEC_I_CONTINUE_NEEDED:
            // Send data to the server and continue later.
            if (out_buffers[0].pvBuffer != nullptr) {
#if (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)))
                ::OutputDebugStringW(L"Sending TLS data.\r\n");
#endif /* (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG))) */
                io_completion_port::instance().send(connection,
                    out_buffers[0].pvBuffer,
                    out_buffers[0].cbBuffer,
                    &on_network_sent,
                    &on_network_failed,
                    &on_network_disconnected,
                    this);
            }
            break;

        case SEC_E_INCOMPLETE_MESSAGE:
            // Read more input from the server and continue later.
#if (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)))
            ::OutputDebugStringW(L"Receiving TLS data.\r\n");
#endif /* (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG))) */
            io_completion_port::instance().receive(connection,
                max_packet_size,
                &on_network_received,
                &on_network_failed,
                &on_network_disconnected,
                this);
            break;

        default:
            throw std::system_error(status, com_category());
    }

    if (in_buffers[1].BufferType == SECBUFFER_EXTRA) {
        // Copy leftover data to the input buffer.
        this->_input_buffer.resize(in_buffers[1].cbBuffer);
        auto e = static_cast<io_context::byte_type *>(in_buffers[1].pvBuffer);
        std::copy(e, e + in_buffers[1].cbBuffer, this->_input_buffer.begin());

    } else {
        this->_input_buffer.clear();
    }
}
