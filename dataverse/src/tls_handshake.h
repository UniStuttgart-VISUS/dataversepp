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

        tls_handshake(void);

        std::future<tls_context> operator ()(
            _In_ dataverse_connection *connection);

    private:

        static void on_network_disconnected(
            _In_ dataverse_connection *connection,
            _In_opt_ void *context);

        static void on_network_failed(_In_ dataverse_connection *connection,
            _In_ const std::system_error& error, _In_opt_ void *context);

        static void on_network_received(_In_ dataverse_connection *connection,
            _In_reads_bytes_(cnt) const io_context::byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ void *context);

        static void on_network_sent(_In_ dataverse_connection *connection,
            _In_opt_ void *context);

        static constexpr const std::size_t max_packet_size = (16384 + 512);

#if defined(_WIN32)
        SECURITY_STATUS create_client_context(
            _In_opt_ const void *input = nullptr,
            _In_ const std::size_t cnt = 0);

        void handshake(_In_ dataverse_connection *connection,
            _In_opt_ const void *input = nullptr,
            _In_ const std::size_t cnt = 0);
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        void free_output_buffer(void);
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        tls_context::security_context_type _context;
        DWORD _flags;
        tls_context::credentials_handle_type _handle;
        std::wstring _hostname;
        std::vector<io_context::byte_type> _input_buffer;
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
