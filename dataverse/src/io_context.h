// <copyright file="io_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

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
        /// The overlapped structure, which must be the first in the context
        /// on Windows.
        /// </summary>
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
        /// The handler to be invoked if a graceful disconnect was detected.
        /// </summary>
        void (*on_disconnected)(_In_ dataverse_connection *,
            _In_opt_ void *);

        /// <summary>
        /// The user-defined handler to be invoked in case of an I/O error.
        /// </summary>
        void (*on_failed)(_In_ dataverse_connection *,
            _In_ const std::system_error&,
            _In_opt_ void *);

        /// <summary>
        /// The opration-specific handler for a successful operation.
        /// </summary>
        union {
            void (*received)(_In_ dataverse_connection *,
                _In_reads_bytes_(cnt) const byte_type *,
                _In_ const std::size_t cnt,
                _In_opt_ void *) ;
            void (*sent)(_In_ dataverse_connection *,
                _In_opt_ void *);
        } on_succeded;

        /// <summary>
        /// The requested kind of I/O operation.
        /// </summary>
        io_operation operation;

        /// <summary>
        /// The actual size of the payload buffer directly following this
        /// structure, in bytes.
        /// </summary>
        std::size_t size;

        /// <summary>
        /// The user data to be passed to any callback.
        /// </summary>
        void *user_data;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        io_context(_In_ const std::size_t size = 0) noexcept;

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
        void invoke_on_received(_In_ dataverse_connection *connection,
            _In_ const std::size_t cnt);

        /// <summary>
        /// Invoke <see cref="on_succeeded::sent" /> if it is set.
        /// </summary>
        void invoke_on_sent(_In_ dataverse_connection *connection);

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
