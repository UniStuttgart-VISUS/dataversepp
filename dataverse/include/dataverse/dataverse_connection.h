// <copyright file="dataverse_connection_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#endif /* defined(_WIN32) */

#include "dataverse/blob.h"
#include "dataverse/types.h"


namespace visus {
namespace dataverse {

    /* Forward declarations. */
    namespace detail { class dataverse_connection_impl; }

    /// <summary>
    /// Represents a TCP/IP connection to the DataVerse API, which is basically a
    /// RAII wrapper around a TCP/IP socket.
    /// </summary>
    /// <remarks>
    /// <para>This API is only exposed for testing purposes.</para>
    /// </remarks>
    class DATAVERSE_API dataverse_connection {

    public:

        /// <summary>
        /// The type used to represent a single byte.
        /// </summary>
        typedef blob::byte_type byte_type;

        /// <summary>
        /// The callback to be invoked for a raw response.
        /// </summary>
        typedef _In_ void (*on_response_type)(_In_ const blob &,
            _In_opt_ void *);

        /// <summary>
        /// The callback to be invoked for an error.
        /// </summary>
        typedef _In_ void (*on_error_type)(_In_ const int,
            _In_z_ const char_type *,
            _In_z_ const char_type *,
            _In_opt_ void *);

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        dataverse_connection(void);

        /// <summary>
        /// Initialise from move.
        /// </summary>
        /// <param name="rhs">The object to be moved.</param>
        dataverse_connection(_Inout_ dataverse_connection&& rhs) noexcept;

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~dataverse_connection(void);

        /// <summary>
        /// Sets a new API key to authenticate with DataVerse.
        /// </summary>
        /// <param name="api_key">The API key to use. It is safe to pass
        /// <c>nullptr</c>, in which case the object tries to make
        /// unauthenticated requests.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to store the
        /// data could not be alloctated.</exception>
        dataverse_connection& api_key(_In_opt_z_ const char_type *api_key);

        /// <summary>
        /// Sets the base path to the API end point such that you do not have to
        /// specify the common part with every request.
        /// </summary>
        /// <param name="base_path">The base path to prepend to every request.
        /// It is safe to pass <c>nullptr</c>.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to store the
        /// data could not be alloctated.</exception>
        dataverse_connection& base_path(_In_opt_z_ const char_type *base_path);

        dataverse_connection& connect(_In_z_ const char_type *host,
            _In_ const std::uint16_t port,
            _In_ const bool tls = true);

        void get(_In_opt_z_ const char_type *resource,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        dataverse_connection& operator =(
            _Inout_ dataverse_connection&& rhs) noexcept;

    private:

        detail::dataverse_connection_impl& check_not_disposed(void) const;

        detail::dataverse_connection_impl *_impl;

    };

} /* namespace dataverse */
} /* namespace visus */
