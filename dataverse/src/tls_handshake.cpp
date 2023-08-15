// <copyright file="tls_handshake.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "tls_handshake.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>

#include "com_error_category.h"
#include "io_completion_port.h"

// https://gist.github.com/odzhan/1b5836c4c8b02d4d9cb9ec574432432c
// https://learn.microsoft.com/en-us/windows/win32/secauthn/using-sspi-with-a-windows-sockets-client


/*
 * visus::dataverse::detail::tls_handshake::tls_handshake
 */
visus::dataverse::detail::tls_handshake::tls_handshake(void) {
#if defined(_WIN32)
    ::ZeroMemory(&this->_output_buffer, sizeof(this->_output_buffer));
    this->_output_desc.ulVersion = SECBUFFER_VERSION;
    this->_output_desc.cBuffers = 1;
    this->_output_desc.pBuffers = &this->_output_buffer;
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::detail::tls_handshake::~tls_handshake
 */
visus::dataverse::detail::tls_handshake::~tls_handshake(void) {
#if defined(_WIN32)
    // Note: the output buffer should have been cleaned directly after it was
    // used.
    assert(this->_output_buffer.pvBuffer == nullptr);
    this->free_output_buffer();
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::detail::tls_handshake::operator ()
 */
std::future<visus::dataverse::detail::tls_context>
visus::dataverse::detail::tls_handshake::operator ()(
        _In_ dataverse_connection *connection) {
    assert(connection != nullptr);

#if defined(_WIN32)
    std::lock_guard<decltype(this->_lock)> l(this->_lock);

    this->_context.reset();
    this->_flags = default_flags;
    this->_handle.reset();

    {
        SCHANNEL_CRED cred;
        ::ZeroMemory(&cred, sizeof(cred));
        cred.dwVersion = SCHANNEL_CRED_VERSION;
        cred.dwFlags = SCH_USE_STRONG_CRYPTO
            | SCH_CRED_AUTO_CRED_VALIDATION
            | SCH_CRED_NO_DEFAULT_CREDS;
        cred.dwFlags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK
            | SCH_CRED_IGNORE_REVOCATION_OFFLINE;
        cred.dwFlags |= SCH_CRED_NO_SERVERNAME_CHECK;
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

    this->handshake(connection);
#else /* defined(_WIN32) */
#endif /* defined(_WIN32) */

    return this->_promise.get_future();
}


/*
 * visus::dataverse::detail::tls_handshake::operator ()
 */
std::future<visus::dataverse::detail::tls_context>
visus::dataverse::detail::tls_handshake::operator ()(
        _Inout_ tls_context&& context,
        _In_ dataverse_connection *connection,
        _In_reads_(cnt) const void *data,
        _In_ const std::size_t cnt) {
    assert(connection != nullptr);

#if defined(_WIN32)
    std::lock_guard<decltype(this->_lock)> l(this->_lock);

    this->_context = std::move(context._context);
    this->_flags = default_flags;
    this->_handle = std::move(context._handle);

    this->handshake(connection, data, cnt);
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
        std::lock_guard<decltype(that->_lock)> l(that->_lock);
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
        std::lock_guard<decltype(that->_lock)> l(that->_lock);
        that->_promise.set_exception(std::current_exception());
    }
}


/*
 * visus::dataverse::detail::tls_handshake::on_network_received
 */
void visus::dataverse::detail::tls_handshake::on_network_received(
        _In_ dataverse_connection *connection,
        _In_reads_bytes_(cnt) const byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ void *context) {
    auto that = static_cast<tls_handshake *>(context);
    try {
#if defined(_WIN32)
#if (defined(DEBUG) || defined(_DEBUG))
        ::OutputDebugStringW(L"Received data for TLS handshake.\r\n");
#endif /* (defined(DEBUG) || defined(_DEBUG)) */
        std::lock_guard<decltype(that->_lock)> l(that->_lock);
        that->handshake(connection, data, cnt);
#endif /* defined(_WIN32) */
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
#if defined(_WIN32)
#if (defined(DEBUG) || defined(_DEBUG))
        ::OutputDebugStringW(L"Sent data for TLS handshake.\r\n");
#endif /* (defined(DEBUG) || defined(_DEBUG)) */
        //std::lock_guard<decltype(that->_lock)> l(that->_lock);
        //::OutputDebugStringW(L"Waiting for subsequent TLS message.\r\n");
        //io_completion_port::instance().receive(connection,
        //    max_packet_size,
        //    &on_network_received,
        //    &on_network_failed,
        //    &on_network_disconnected,
        //    that);
#endif /* defined(_WIN32) */

    } catch (...) {
        that->_promise.set_exception(std::current_exception());
    }
}


#if defined(_WIN32)
/*
 * visus::dataverse::detail::tls_handshake::default_flags
 */
const DWORD visus::dataverse::detail::tls_handshake::default_flags
    = ISC_REQ_USE_SUPPLIED_CREDS
    | ISC_REQ_ALLOCATE_MEMORY
    | ISC_REQ_CONFIDENTIALITY
    | ISC_REQ_REPLAY_DETECT
    | ISC_REQ_SEQUENCE_DETECT
    | ISC_REQ_STREAM;
#endif /* defined(_WIN32) */


/*
 * visus::dataverse::detail::tls_handshake::max_packet_size
 */
const std::size_t visus::dataverse::detail::tls_handshake::max_packet_size;


#if defined(_WIN32)
/*
 * visus::dataverse::detail::tls_handshake::create_client_context
 */
SECURITY_STATUS visus::dataverse::detail::tls_handshake::create_client_context(
        _In_opt_ const void *input, _In_ const std::size_t cnt) {
    // Reset the output buffer.
    this->_output_buffer.BufferType = SECBUFFER_TOKEN;
    this->_output_buffer.pvBuffer = nullptr;
    this->_output_buffer.cbBuffer = 0;

    auto hostname = this->_hostname.empty()
        ? nullptr
        : reinterpret_cast<SEC_WCHAR *>(&this->_hostname[0]);
    auto status = SEC_E_OK;

    if (input == nullptr) {
        // This is the first call when we do not have any data from the server.
        status = ::InitializeSecurityContextW(
            this->_handle.addressof(),
            nullptr,
            hostname,
            this->_flags,
            0,
            0,
            nullptr,
            0,
            this->_context.addressof(),
            &this->_output_desc,
            &this->_flags,
            nullptr);

    } else {
        // If we received a message from the server, create the input buffer
        // descriptor, too.
        SecBuffer in_buffers[2];
        in_buffers[0].BufferType = SECBUFFER_TOKEN;
        in_buffers[0].pvBuffer = const_cast<void *>(input);
        in_buffers[0].cbBuffer = static_cast<DWORD>(cnt);

        in_buffers[1].BufferType = SECBUFFER_EMPTY;
        in_buffers[1].pvBuffer = nullptr;
        in_buffers[1].cbBuffer = 0;

        // Cf. https://learn.microsoft.com/en-us/windows/win32/secauthn/initializesecuritycontext--schannel
        // On calls to this function after the initial call, there must be two
        // buffers. The first has type SECBUFFER_TOKEN and contains the token
        // received from the server. The second buffer has type SECBUFFER_EMPTY;
        // set both the pvBuffer and cbBuffer members to zero.
        SecBufferDesc in_desc;
        in_desc.ulVersion = SECBUFFER_VERSION;
        in_desc.cBuffers = ARRAYSIZE(in_buffers);
        in_desc.pBuffers = in_buffers;

#if (defined(DEBUG) || defined(_DEBUG))
        {
            std::wstringstream str;
            str << L"Processing " << in_buffers[0].cbBuffer << L" bytes "
                << L"of TLS input." << std::endl;
            auto msg = str.str();
            ::OutputDebugStringW(msg.c_str());
        }
#endif /* (defined(DEBUG) || defined(_DEBUG)) */

        status = ::InitializeSecurityContextW(
            this->_handle.addressof(),
            this->_context.addressof(),
            hostname,
            this->_flags,
            0,
            0,
            &in_desc,
            0,
            this->_context.addressof(),
            &this->_output_desc,
            &this->_flags,
            nullptr);

        if (in_buffers[1].BufferType == SECBUFFER_EXTRA) {
            // This looks weird, but it seems to be the way things work, albeit
            // the documentation on MSDN does not say so. If Schannel did not
            // consume all of the input, it returns the remaining data in
            // in_buffers[1], but only the offsets, not the actual data.
            // Therefore, we need to use the size of in_buffers[1] to preserve
            // the data from in_buffers[0] for a future call to the API.
#if (defined(DEBUG) || defined(_DEBUG))
            {
                std::wstringstream str;
                str << L"Preserving " << in_buffers[1].cbBuffer << L" bytes "
                    << L"of unused TLS input." << std::endl;
                auto msg = str.str();
                ::OutputDebugStringW(msg.c_str());
            }
#endif /* (defined(DEBUG) || defined(_DEBUG)) */
            const auto cnt = in_buffers[1].cbBuffer;
            const auto off = in_buffers[0].cbBuffer - cnt;
            const auto beg = static_cast<const byte_type *>(input) + off;

            if (status != SEC_E_INCOMPLETE_MESSAGE) {
                this->_input_buffer.clear();
            }

            this->_input_buffer.reserve(this->_input_buffer.size() + cnt);
            std::copy(beg, beg + cnt, std::back_inserter(this->_input_buffer));

        } else if (status != SEC_E_INCOMPLETE_MESSAGE) {
            // Everything has been consumed.
#if (defined(DEBUG) || defined(_DEBUG))
            ::OutputDebugString(_T("Clearing processed TLS input.\r\n"));
#endif /* (defined(DEBUG) || defined(_DEBUG)) */
            this->_input_buffer.clear();
        }
    }

#if (defined(DEBUG) || defined(_DEBUG))
    {
        std::wstringstream str;
        str << L"TLS API call result 0x" << std::hex << status << std::endl;
        auto msg = str.str();
        ::OutputDebugStringW(msg.c_str());
    }
#endif /* (defined(DEBUG) || defined(_DEBUG)) */

    switch (status) {
        case SEC_I_COMPLETE_NEEDED:
            // The client must finish building the message and then call the
            // CompleteAuthToken function.
            __fallthrough;
        case SEC_I_COMPLETE_AND_CONTINUE:
            // The client must call CompleteAuthToken and then pass the output
            // to the server. The client then waits for a returned token and
            // passes it, in another call, to InitializeSecurityContext.
            ::OutputDebugStringW(L"Completing TLS token.\r\n");
            status = ::CompleteAuthToken(this->_context.addressof(),
                &this->_output_desc);
            break;
    }

    switch (status) {
        case SEC_I_CONTINUE_NEEDED:
            // The client must send the output token to the server and wait for
            // a return token. The returned token is then passed in another call
            // to InitializeSecurityContext. The output token can be empty.
            __fallthrough;
        case SEC_I_COMPLETE_AND_CONTINUE:
            // See above.
            __fallthrough;
        case SEC_E_INCOMPLETE_MESSAGE:
            // Data for the whole message was not read from the wire.
            // When this value is returned, the pInput buffer contains a
            // SecBuffer structure with a BufferType member of
            // SECBUFFER_MISSING. The cbBuffer member of SecBuffer contains a
            // value that indicates the number of additional bytes that the
            // function must read from the client before this function succeeds.
            // While this number is not always accurate, using it can help
            // improve performance by avoiding multiple calls to this function.
            return status;

        case SEC_E_OK: {
            // The security context was successfully initialized. There is no
            // need for another InitializeSecurityContext call. If the function
            // returns an output token, that is, if the SECBUFFER_TOKEN in
            // pOutput is of nonzero length, that token must be sent to the
            // server.
            ::OutputDebugStringW(L"TLS handshake complete.\r\n");
            tls_context retval;
            retval._context = std::move(this->_context);
            retval._flags = this->_flags & ~ISC_REQ_USE_SUPPLIED_CREDS;
            retval._handle = std::move(this->_handle);

            {
                auto status = ::QueryContextAttributes(
                    retval._context.addressof(),
                    SECPKG_ATTR_STREAM_SIZES,
                    &retval._sizes);
                if (status < 0) {
                    throw std::system_error(status, com_category());
                }
            }

            this->_promise.set_value(std::move(retval));
            }
            return status;

        default:
            // This is fatal.
            throw std::system_error(status, com_category());
    }
}


/*
 * visus::dataverse::detail::tls_handshake::handshake
 */
void visus::dataverse::detail::tls_handshake::handshake(
        _In_ dataverse_connection *connection,
        _In_opt_ const void *input,
        _In_ const std::size_t cnt) {
    // Schedule free of output buffer, as we use ISC_REQ_ALLOCATE_MEMORY.
    auto free_output_buffer = wil::scope_exit([this](void) {
        this->free_output_buffer();
    });

    if (input != nullptr) {
        this->_input_buffer.reserve(this->_input_buffer.size() + cnt);
        std::copy(static_cast<const byte_type *>(input),
            static_cast<const byte_type *>(input) + cnt,
            std::back_inserter(this->_input_buffer));
    }

    auto have_input = true;
    auto need_input = false;

    while (have_input) {
        // Note: We need to make sure that the first call (if 'input' is null)
        // to create_client_context also passes null, not an empty array.
        auto i = ((input == nullptr) && this->_input_buffer.empty())
            ? nullptr
            : this->_input_buffer.data();
        const auto status = this->create_client_context(i,
            this->_input_buffer.size());

        if ((this->_output_buffer.pvBuffer != nullptr)
            && (this->_output_buffer.cbBuffer > 0)) {
            // In any case, if there is an output buffer, we must first send
            // this to the server and then potentially wait for the answer.
            ::OutputDebugStringW(L"Sending TLS handshake message.\r\n");
            io_completion_port::instance().send(connection,
                this->_output_buffer.pvBuffer,
                this->_output_buffer.cbBuffer,
                &on_network_sent,
                &on_network_failed,
                &on_network_disconnected,
                this);
        }

        // Determine whether we need more input.
        need_input = (status == SEC_E_INCOMPLETE_MESSAGE)
            || (status == SEC_I_CONTINUE_NEEDED)
            || (status == SEC_I_COMPLETE_AND_CONTINUE);

        // If the message is incomplete, we need to receive again regarless
        // of the buffer state. Otherwise, we request only additional data
        // if we do not have anything left.
        have_input = (status != SEC_E_INCOMPLETE_MESSAGE)
            && !this->_input_buffer.empty();
    } /* while (have_input) */

    if (need_input) {
        ::OutputDebugStringW(L"Waiting for TLS handshake message.\r\n");
        io_completion_port::instance().receive(connection,
            max_packet_size,
            &on_network_received,
            &on_network_failed,
            &on_network_disconnected,
            this);
    }
}
#endif /* defined(_WIN32) */


#if defined(_WIN32)
/*
 * visus::dataverse::detail::tls_handshake::free_output_buffer
 */
void visus::dataverse::detail::tls_handshake::free_output_buffer(void) {
    if (this->_output_buffer.pvBuffer != nullptr) {
        ::FreeContextBuffer(this->_output_buffer.pvBuffer);
        this->_output_buffer.pvBuffer = nullptr;
        this->_output_buffer.cbBuffer = 0;
    }
}
#endif /* defined(_WIN32) */
