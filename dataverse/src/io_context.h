// <copyright file="io_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <functional>
#include <system_error>

#include "dataverse/api.h"
#include "dataverse/dataverse_connection.h"
#include "dataverse/types.h"

#include "io_operation.h"



namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// The head of an in-flight context of an asynchronous I/O operation.
    /// </summary>
    struct DATAVERSE_API io_context final {

        /// <summary>
        /// The type used to represent a single byte.
        /// </summary>
        typedef std::uint8_t byte_type;

       /// <summary>
        /// Allocates the memory for a context including its payload.
        /// </summary>
        /// <param name="header_size"></param>
        /// <param name="body_size"></param>
        /// <returns></returns>
        static _Ret_ void *operator new(_In_ const std::size_t header_size,
            _In_ const std::size_t body_size = 0);

        /// <summary>
        /// Release the memory of a context.
        /// </summary>
        /// <param name="ptr"></param>
        static void operator delete(_In_opt_ void *ptr);

#if defined(_WIN32)
        /// <summary>
        /// The overlapped structure.
        /// </summary>
        /// <remarks>
        /// <b>This must be the first member of the structure on Windows!</b>
        /// </remarks>
        OVERLAPPED overlapped;
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        /// <summary>
        /// The WSA buffer for this context.
        /// </summary>
        /// <remarks>
        /// Besides the data, the buffer structure must also live until a send
        /// or receive operation is completed.
        /// </remarks>
        WSABUF buffer;
#endif /* defined(_WIN32) */

        /// <summary>
        /// The user data to be passed to the final callback.
        /// </summary>
        void *client_data;

        /// <summary>
        /// The handler to be invoked if a graceful disconnect was detected.
        /// </summary>
        std::function<void(_In_ dataverse_connection *,
            _In_ io_context *)> on_disconnected;

        /// <summary>
        /// The user-defined handler to be invoked in case of an I/O error.
        /// </summary>
        std::function<void(_In_ dataverse_connection *,
            _In_ const std::system_error&,
            _In_ io_context *)> on_failed;

        /// <summary>
        /// The opration-specific handler for a successful operation.
        /// </summary>
        /// <remarks>
        /// The current state of this unrestricted union is tracked by the
        /// <see cref="operation" /> field. Make sure to keep both fields
        /// consistent all the time. See
        /// https://learn.microsoft.com/en-us/cpp/cpp/unions?view=msvc-160.
        /// This is achieved by setting the operation and callback using
        /// <see cref="operation_receive" /> etc.
        /// </remarks>
        union {
            std::function<std::size_t(_In_ dataverse_connection *,
                _In_ const std::size_t cnt,
                _In_ io_context *)> on_received;
            std::function<void(_In_ dataverse_connection *,
                _In_ io_context *)> on_sent;
        };

        /// <summary>
        /// The requested kind of I/O operation.
        /// </summary>
        /// <remarks>
        /// <b>Do not modify this field directly as it is used to track the
        /// contents of <see cref="on_succeeded" />!</b>
        /// </remarks>
        io_operation operation;

        /// <summary>
        /// The actual size of the payload buffer directly following this
        /// structure, in bytes.
        /// </summary>
        std::size_t size;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        io_context(_In_ const std::size_t size = 0) noexcept;

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~io_context(void) noexcept;

        /// <summary>
        /// Invoke <see cref="on_disconnected" /> if it is set.
        /// </summary>
        void invoke_on_disconnected(_In_ dataverse_connection *connection);

        /// <summary>
        /// Invoke <see cref="on_failed" /> if it is set.
        /// </summary>
        void invoke_on_failed(_In_ dataverse_connection *connection,
            _In_ const std::system_error& ex);

        /// <summary>
        /// Invoke <see cref="on_succeeded::received" /> if it is set.
        /// </summary>
        std::size_t invoke_on_received(_In_ dataverse_connection *connection,
            _In_ const std::size_t cnt);

        /// <summary>
        /// Invoke <see cref="on_succeeded::sent" /> if it is set.
        /// </summary>
        void invoke_on_sent(_In_ dataverse_connection *connection);

        /// <summary>
        /// Mark the context as a receive context and set the specified
        /// callback, which may be invalid.
        /// </summary>
        void operation_receive(
            _In_ const decltype(io_context::on_received)& on_received);

        /// <summary>
        /// Mark the context as a send context and set the specified
        /// callback, which may be invalid.
        /// </summary>
        void operation_send(_In_ const decltype(io_context::on_sent)& on_sent);

        /// <summary>
        /// Resets the operation to <see cref="io_operation::unknown" /> and
        /// clears <see cref="on_succeeded" />.
        /// </summary>
        void operation_unknown(void) noexcept;

        /// <summary>
        /// Answer a pointer to the begin of the payload after the object.
        /// </summary>
        /// <returns>A pointer to the begin of the payload. If the payload
        /// size is zero, the method returns <c>nullptr</c>.</returns>
        _Ret_maybenull_ std::uint8_t *payload(void) noexcept;

        /// <summary>
        /// Answer a pointer to the begin of the payload after the object.
        /// </summary>
        /// <returns>A pointer to the begin of the payload. If the payload
        /// size is zero, the method returns <c>nullptr</c>.</returns>
        _Ret_maybenull_ const std::uint8_t *payload(void) const noexcept;

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
