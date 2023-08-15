// <copyright file="tls_handshake.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>
#include <future>
#include <mutex>
#include <string>
#include <system_error>
#include <vector>

#include "dataverse/dataverse_connection.h"

#include "io_context.h"
#include "tls_context.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// The context holding all the data to perform a TLS handshake and create
    /// a <see cref="tls_context" /> from it.
    /// </summary>
    class tls_handshake final {

    public:

        /// <summary>
        /// The type of a single byte.
        /// </summary>
        typedef io_context::byte_type byte_type;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name=""></param>
        tls_handshake(void);

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~tls_handshake(void);

        /// <summary>
        /// Performs the handshake on the given connection.
        /// </summary>
        /// <param name="connection"></param>
        /// <returns></returns>
        std::future<tls_context> operator ()(
            _In_ dataverse_connection *connection);

        std::future<tls_context> operator ()(
            _Inout_ tls_context&& context,
            _In_ dataverse_connection *connection,
            _In_reads_(cnt) const void *data,
            _In_ const std::size_t cnt);

    private:

        static void on_network_disconnected(
            _In_ dataverse_connection *connection,
            _In_opt_ io_context *context);

        static void on_network_failed(_In_ dataverse_connection *connection,
            _In_ const std::system_error& error,
            _In_opt_ io_context *context);

        static void on_network_received(_In_ dataverse_connection *connection,
            _In_ const std::size_t cnt,
            _In_opt_ io_context *context);

        static void on_network_sent(_In_ dataverse_connection *connection,
            _In_opt_ io_context *context);

#if defined(_WIN32)
        static const DWORD default_flags;
#endif /* defined(_WIN32) */

        static constexpr const std::size_t max_packet_size = (16384 + 512);

#if defined(_WIN32)
        /// <summary>
        /// This is a wrapper around <see cref="InitializeSecurityContextW" />
        /// that prepares the input and output buffers from the state of the
        /// object and checks for fatal errors.
        /// </summary>
        SECURITY_STATUS create_client_context(
            _In_opt_ const void *input = nullptr,
            _In_ const std::size_t cnt = 0);
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        void handshake(_In_ dataverse_connection *connection,
            _In_opt_ const void *input = nullptr,
            _In_ const std::size_t cnt = 0);
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        /// <summary>
        /// Releases <see cref="_output_buffer" /> if it is valid.
        /// </summary>
        void free_output_buffer(void);
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        tls_context::security_context_type _context;
        DWORD _flags;
        tls_context::credentials_handle_type _handle;
        std::wstring _hostname;
        std::vector<byte_type> _input_buffer;
        SecBuffer _output_buffer;
        SecBufferDesc _output_desc;
#else /* defined(_WIN32) */
#endif /* defined(_WIN32) */

        std::mutex _lock;
        std::promise<tls_context> _promise;

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
