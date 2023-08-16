// <copyright file="tls_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <functional>
#include <vector>

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#include <schannel.h>
#define SECURITY_WIN32
#include <sspi.h>

#include <wil/resource.h>
#else /* defined(_WIN32) */
#endif /* defined(_WIN32) */

#include "dataverse/convert.h"
#include "dataverse/dataverse_connection.h"
#include "dataverse/types.h"

#include "io_context.h"


namespace visus {
namespace dataverse {
namespace detail {


    /// <summary>
    /// A RAII container for a TLS connection.
    /// </summary>
    class tls_context final {

    public:

        /// <summary>
        /// The type to represent a single byte.
        /// </summary>
        typedef io_context::byte_type byte_type;

#if defined(_WIN32)
        /// <summary>
        /// The type to store an SChannel credentials handle.
        /// </summary>
        typedef wil::unique_struct<CredHandle,
            decltype(&::FreeCredentialsHandle),
            ::FreeCredentialsHandle> credentials_handle_type;

        /// <summary>
        /// The type to store an SChannel security context.
        /// </summary>
        typedef wil::unique_struct<CtxtHandle,
            decltype(&::DeleteSecurityContext),
            ::DeleteSecurityContext> security_context_type;
#else /* defined(_WIN32) */
        //SSL_CTX *_context;
#endif /* defined(_WIN32) */

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        tls_context(void);

        /// <summary>
        /// Answer the maximum message size in bytes for the session.
        /// </summary>
        /// <returns></returns>
        std::size_t message_size(void) const;

        /// <summary>
        /// Receives and decrypts data via the connection provided.
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
            _In_opt_ void *client_data);

        /// <summary>
        /// Encrypt the given data and send them using the connection
        /// provided.
        /// </summary>
        void send(_In_ dataverse_connection *connection,
            _In_reads_bytes_(size) const void *data,
            _In_ const std::size_t size,
            _In_ const decltype(io_context::on_sent) on_sent,
            _In_ const decltype(io_context::on_failed) on_failed,
            _In_opt_ void *client_data);

        /// <summary>
        /// Send a TLS shudown notification via the given connection.
        /// </summary>
        void shutdown(_In_ dataverse_connection *connection);

    private:

        /// <summary>
        /// Convert a raw pointer to a byte vector.
        /// </summary>
        static inline std::vector<byte_type> make_vector(
                _In_reads_bytes_(size) const void *data,
                _In_ const std::size_t size) {
            auto d = static_cast<const byte_type *>(data);
            return std::vector<byte_type>(d, d + size);
        }

        /// <summary>
        /// Converts the given SSPI buffer into a byte vector.
        /// </summary>
        static std::vector<byte_type> make_vector(_In_ const SecBuffer& buffer);

        /// <summary>
        /// Decrypts the given message, possibly prepending the stuff in
        /// <see cref="_pending_decrypt" />.
        /// </summary>
        std::unique_ptr<io_context> decrypt(
            _In_reads_bytes_(size) const void *data,
            _In_ const std::size_t size);

#if defined(_WIN32)
        security_context_type _context;
        DWORD _flags;
        credentials_handle_type _handle;
        SecPkgContext_StreamSizes _sizes;
#else /* defined(_WIN32) */
        //SSL_CTX *_context;
#endif /* defined(_WIN32) */

        std::vector<byte_type> _pending_decrypt;

        friend class tls_handshake;

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
