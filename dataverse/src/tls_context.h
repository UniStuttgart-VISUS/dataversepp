// <copyright file="tls_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <functional>

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

        /// <summary>
        /// The type of a decryption result.
        /// </summary>
        struct decrypted_type {
            /// <summary>
            /// The decrypted data.
            /// </summary>
            std::vector<byte_type> decrypted;

            /// <summary>
            /// The data that could not be decrypted, because the input was too
            /// small.
            /// </summary>
            /// <remarks>
            /// Callers should get more input and prepend this previous data to
            /// the next input of the decrypt method.
            /// </remarks>
            std::vector<byte_type> remainder;

            /// <summary>
            /// If not <c>nullptr</c>, the other side wants to renegotiate this
            /// context.
            /// </summary>
            /// <remarks>
            /// Callers should pass the <see cref="remainder" /> as input to the
            /// handshake when renegotiating.
            /// </remarks>
            tls_context *renegotiate;

            inline decrypted_type(void) : renegotiate(nullptr) { }

            inline decrypted_type(_Inout_ std::vector<byte_type>&& decrypted)
                : decrypted(std::move(decrypted)), renegotiate(nullptr) { }

            inline decrypted_type(_Inout_ std::vector<byte_type>&& decrypted,
                    _Inout_ std::vector<byte_type>&& remainder,
                    _In_opt_ tls_context *renegotiate = nullptr)
                : decrypted(std::move(decrypted)),
                    remainder(std::move(remainder)),
                    renegotiate(renegotiate) { }

            inline bool incomplete(void) const noexcept {
                return (!this->remainder.empty()
                    && (this->renegotiate == nullptr));
            }
        };

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

        tls_context(void);

        /// <summary>
        /// Decrypts the given data received via the TLS connection.
        /// </summary>
        decrypted_type decrypt(_In_reads_bytes_(size) const void *data,
            _In_ const std::size_t size);

        /// <summary>
        /// Encrypt the given data and send them using the connection
        /// provided.
        /// </summary>
        void send(_In_ dataverse_connection *connection,
            _In_reads_bytes_(size) const void *data,
            _In_ const std::size_t size,
            _In_ const decltype(io_context::on_succeded.sent) on_sent,
            _In_ const decltype(io_context::on_failed) on_failed,
            _In_ const decltype(io_context::on_disconnected) on_disconnected,
            _In_opt_ void *context);

        /// <summary>
        /// Send a TLS shudown notification via the given connection.
        /// </summary>
        void shutdown(_In_ dataverse_connection *connection);

    private:

        /// <summary>
        /// Create an empty byte vector.
        /// </summary>
        static inline std::vector<byte_type> make_vector(void) {
            return std::vector<byte_type>();
        }

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
        static std::vector<byte_type> make_vector(_In_ const SecBuffer& buffer,
            _In_ const SecBuffer *extra = nullptr);

#if defined(_WIN32)
        security_context_type _context;
        DWORD _flags;
        credentials_handle_type _handle;
        SecPkgContext_StreamSizes _sizes;
#else /* defined(_WIN32) */
        //SSL_CTX *_context;
#endif /* defined(_WIN32) */

        friend class tls_handshake;

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
