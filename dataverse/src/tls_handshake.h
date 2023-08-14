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

        void initialise(_In_ dataverse_connection *connection,
            _In_ const bool first);

#if defined(_WIN32)
        tls_context::security_context_type _context;
        DWORD _flags;
        tls_context::credentials_handle_type _handle;
        std::wstring _hostname;
        std::vector<std::uint8_t> _input_buffer;
        std::vector<std::uint8_t> _output_buffer;
#else /* defined(_WIN32) */
#endif /* defined(_WIN32) */

        std::mutex _lock;
        std::promise<tls_context> _promise;

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
