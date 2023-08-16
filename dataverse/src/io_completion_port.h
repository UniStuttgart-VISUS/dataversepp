// <copyright file="io_completion_port.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#if defined(_WIN32)
#include <WinSock2.h>   // Important!
#include <wil/resource.h>
#endif /* defined(_WIN32) */

#include "dataverse/dataverse_connection.h"

#include "io_context.h"
#include "io_operation.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// Manages an I/O completion port for processing asynchronous send and
    /// receive operations in a thread pool.
    /// </summary>
    class io_completion_port final {

    public:

        /// <summary>
        /// The type of the context used for asynchronous requests.
        /// </summary>
        typedef std::unique_ptr<io_context> context_type;

        /// <summary>
        /// Gets the only instance of the I/O completion port.
        /// </summary>
        /// <returns></returns>
        static io_completion_port& instance(void);

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~io_completion_port(void);

        /// <summary>
        /// Associates the socket of the given
        /// <see cref="dataverse_connection" /> with the I/O completion port,
        /// which causes all asynchronous I/O operations of the port to feed
        /// into one of the pool threads of the I/O completion port.
        /// </summary>
        /// <param name="connection"></param>
        void associate(_In_ dataverse_connection& connection);

        /// <summary>
        /// Allocates or reuses an I/O context for the specified operation.
        /// </summary>
        /// <remarks>
        /// <para>The caller is responsible for setting the appropriate success
        /// handler matching <paramref name="operation" />.</para>
        /// </remarks>
        /// <param name="size"></param>
        /// <param name="on_failed"></param>
        /// <param name="on_disconnected"></param>
        /// <param name="client_data"></param>
        /// <returns></returns>
        context_type context(
            _In_ const std::size_t size,
            _In_ const decltype(io_context::on_failed) on_failed,
            _In_opt_ void *client_data);

        /// <summary>
        /// Asynchronously receives data into the payload buffer of the given
        /// context.
        /// </summary>
        /// <param name="socket"></param>
        /// <param name="context"></param>
        void receive(_In_ socket& socket, _Inout_ context_type&& context);

        /// <summary>
        /// Asynchronously receives data into the payload buffer of the given
        /// context.
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="context"></param>
        void receive(_In_ dataverse_connection *connection,
            _Inout_ context_type&& context);

        /// <summary>
        /// Asynchronously receives data from the given connection.
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="size"></param>
        /// <param name="on_received"></param>
        /// <param name="on_failed"></param>
        /// <param name="client_data"></param>
        void receive(_In_ dataverse_connection *connection,
            _In_ const std::size_t size,
            _In_ const decltype(io_context::on_received) on_received,
            _In_ const decltype(io_context::on_failed) on_failed,
            _In_opt_ void *client_data = nullptr);

        /// <summary>
        /// Recycle the given <see cref="io_context" /> in the pool.
        /// </summary>
        /// <param name="context"></param>
        void recycle(_Inout_ context_type&& context);

        /// <summary>
        /// Asynchronously sends the data in the given context.
        /// </summary>
        /// <param name="socket"></param>
        /// <param name="context"></param>
        void send(_In_ socket& socket, _Inout_ context_type&& context);

        /// <summary>
        /// Asynchronously sends the data in the given context.
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="context"></param>
        void send(_In_ dataverse_connection *connection,
            _Inout_ context_type&& context);

        /// <summary>
        /// Asynchronously sends the given data via the given connection.
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="data"></param>
        /// <param name="size"></param>
        /// <param name="on_sent"></param>
        /// <param name="on_failed"></param>
        /// <param name="client_data"></param>
        void send(_In_ dataverse_connection *connection,
            _In_reads_bytes_(size) const void *data,
            _In_ const std::size_t size,
            _In_ const decltype(io_context::on_sent) on_sent,
            _In_ const decltype(io_context::on_failed) on_failed,
            _In_opt_ void *client_data = nullptr);

        /// <summary>
        /// Stops all I/O worker threads.
        /// </summary>
        void stop(void);

    private:

#if defined(_WIN32)
        /// <summary>
        /// Extracts our custom context from the given overlapped structure.
        /// </summary>
        static context_type context(_In_ OVERLAPPED *overlapped);
#endif /* defined(_WIN32) */

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        io_completion_port(void);

        /// <summary>
        /// Retrieve or create an <see cref="io_context" /> for at least the
        /// specified amount of payload.
        /// </summary>
        context_type context(_In_ const std::size_t size);

        /// <summary>
        /// Performs the work of a pool thread processing I/O completions.
        /// </summary>
        void pool_thread(void);

#if defined(_WIN32)
        /// <summary>
        /// Posts a status update to the I/O completion port.
        /// </summary>
        void post_status(_In_ const DWORD cnt, _In_ const ULONG_PTR key,
            _In_ LPOVERLAPPED overlapped);
#endif /* defined(_WIN32) */

        std::mutex _contexts_lock;
        std::vector<std::unique_ptr<io_context>> _contexts;
#if defined(_WIN32)
        wil::unique_handle _handle;
#endif /* defined(_WIN32) */
        std::vector<std::thread> _threads;

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
