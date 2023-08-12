// <copyright file="socket.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <new>

#include "dataverse/api.h"
#include "dataverse/types.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// Represents a RAII wrapper around a TCP/IP socket.
    /// </summary>
    /// <remarks>
    /// <para>This API is only exposed for testing purposes and should not be
    /// directly used by clients.</para>
    /// </remarks>
    class DATAVERSE_API socket final {

    public:

        /// <summary>
        /// Initialises a new (invalid) instance.
        /// </summary>
        socket(void) noexcept;

        /// <summary>
        /// Initialises a new TCP/IP instance using the specified address
        /// family.
        /// </summary>
        /// <param name="address_family"></param>
        /// <exception cref="std::system_error"></exception>
        explicit socket(_In_ const int address_family);

        /// <summary>
        /// Initialise from move.
        /// </summary>
        /// <param name="rhs"></param>
        /// <returns></returns>
        socket(_Inout_ socket&& rhs) noexcept;

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~socket(void) noexcept;

        /// <summary>
        /// Connects the socket to the specified remote address.
        /// </summary>
        /// <param name="address"></param>
        /// <param name="length"></param>
        void connect(_In_reads_bytes_(length) const sockaddr *address,
            _In_ const std::size_t length);

        /// <summary>
        /// Closes the socket if it is open.
        /// </summary>
        /// <returns></returns>
        system_error_code close(_In_ std::nothrow_t) noexcept;

        /// <summary>
        /// Closes the socket if it is open.
        /// </summary>
        /// <exception cref="std::system_error"></exception>
        void close(void);

        /// <summary>
        /// Move assignment
        /// </summary>
        /// <param name="rhs"></param>
        /// <returns><c>*this</c>.</returns>
        socket& operator =(_Inout_ socket&& rhs) noexcept;

        /// <summary>
        /// Answer whether the socket handle is valid.
        /// </summary>
        /// <returns></returns>
        operator bool(void) const noexcept;

#if defined(_WIN32)
        /// <summary>
        /// Expose the native socket handle.
        /// </summary>
        /// <returns></returns>
        inline operator SOCKET(void) noexcept {
            return this->_handle;
        }
#else /* defined(_WIN32) */
        inline operator int(void) noexcept {
            return this->_handle;
        }
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        /// <summary>
        /// Reinterprets the socket object as a <c>HANDLE</c>.
        /// </summary>
        /// <returns></returns>
        inline operator HANDLE(void) noexcept {
            return reinterpret_cast<HANDLE>(this->_handle);
        }
#endif /* defined(_WIN32) */

    private:

#if defined(_WIN32)
        SOCKET _handle;
#else /* defined(_WIN32) */
        int _handle;
#endif /* defined(_WIN32) */
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
