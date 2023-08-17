// <copyright file="io_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#else /* defined(_WIN32) */
#define CALLBACK
#endif /* defined(_WIN32) */

#include "dataverse/blob.h"
#include "dataverse/dataverse_connection.h"

#include "dataverse_connection_impl.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// The head of an in-flight context of an asynchronous I/O operation.
    /// </summary>
    struct io_context final {

        /// <summary>
        /// The type used to represent a single byte.
        /// </summary>
        typedef dataverse_connection::byte_type byte_type;

        static _Ret_valid_ std::unique_ptr<io_context> create(void);

        static void recycle(_Inout_ std::unique_ptr<io_context> &&context);

        /// <summary>
        /// The I/O callback passed to cURL for writing the response to our
        /// buffer.
        /// </summary>
        static std::size_t CALLBACK write_response(
            _In_reads_bytes_(cnt *element_size) const void *data,
            _In_ const std::size_t cnt,
            _In_ const std::size_t size,
            _In_ void *context);

        /// <summary>
        /// The user data to be passed to the final callback.
        /// </summary>
        void *client_data;

        dataverse_connection_impl::curlm_type curl;
        dataverse_connection_impl::string_list_type headers;
        dataverse_connection_impl::mime_type form;

        /// <summary>
        /// Receives the response from cURL.
        /// </summary>
        blob response;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        io_context(void);

    private:

        static std::vector<std::unique_ptr<io_context>> cache;
        static std::mutex lock;

        ///// <summary>
        ///// The user-defined handler to be invoked in case of an I/O error.
        ///// </summary>
        //std::function<void(_In_ dataverse_connection *,
        //    _In_ const std::system_error&,
        //    _In_ io_context *)> on_failed;

        ///// <summary>
        ///// The opration-specific handler for a successful operation.
        ///// </summary>
        ///// <remarks>
        ///// The current state of this unrestricted union is tracked by the
        ///// <see cref="operation" /> field. Make sure to keep both fields
        ///// consistent all the time. See
        ///// https://learn.microsoft.com/en-us/cpp/cpp/unions?view=msvc-160.
        ///// This is achieved by setting the operation and callback using
        ///// <see cref="operation_receive" /> etc.
        ///// </remarks>
        //union {
        //    std::function<std::size_t(_In_ dataverse_connection *,
        //        _In_ const std::size_t cnt,
        //        _In_ io_context *)> on_received;
        //    std::function<void(_In_ dataverse_connection *,
        //        _In_ io_context *)> on_sent;
        //};

        ///// <summary>
        ///// The requested kind of I/O operation.
        ///// </summary>
        ///// <remarks>
        ///// <b>Do not modify this field directly as it is used to track the
        ///// contents of <see cref="on_succeeded" />!</b>
        ///// </remarks>
        //io_operation operation;

        ///// <summary>
        ///// The actual size of the payload buffer directly following this
        ///// structure, in bytes.
        ///// </summary>
        //std::size_t size;

        ///// <summary>
        ///// Initialises a new instance.
        ///// </summary>
        //io_context(_In_ const std::size_t size = 0) noexcept;

        ///// <summary>
        ///// Finalises the instance.
        ///// </summary>
        //~io_context(void) noexcept;

        ///// <summary>
        ///// Invoke <see cref="on_failed" /> if it is set.
        ///// </summary>
        //void invoke_on_failed(_In_ dataverse_connection *connection,
        //    _In_ const std::system_error& ex);

        ///// <summary>
        ///// Invoke <see cref="on_succeeded::received" /> if it is set.
        ///// </summary>
        //std::size_t invoke_on_received(_In_ dataverse_connection *connection,
        //    _In_ const std::size_t cnt);

        ///// <summary>
        ///// Invoke <see cref="on_succeeded::sent" /> if it is set.
        ///// </summary>
        //void invoke_on_sent(_In_ dataverse_connection *connection);

        ///// <summary>
        ///// Mark the context as a receive context and set the specified
        ///// callback, which may be invalid.
        ///// </summary>
        //void operation_receive(
        //    _In_ const decltype(io_context::on_received)& on_received);

        ///// <summary>
        ///// Mark the context as a send context and set the specified
        ///// callback, which may be invalid.
        ///// </summary>
        //void operation_send(_In_ const decltype(io_context::on_sent)& on_sent);

        ///// <summary>
        ///// Resets the operation to <see cref="io_operation::unknown" /> and
        ///// clears <see cref="on_succeeded" />.
        ///// </summary>
        //void operation_unknown(void) noexcept;

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
