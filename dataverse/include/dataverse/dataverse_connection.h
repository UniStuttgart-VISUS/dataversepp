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

        typedef blob::byte_type byte_type;

        dataverse_connection(void);

        dataverse_connection(_Inout_ dataverse_connection&& rhs) noexcept;

        ~dataverse_connection(void);

        dataverse_connection& api_key(_In_opt_z_ const char_type *api_key);

        dataverse_connection& base_path(_In_opt_z_ const char_type *base_path);

        void connect(_In_z_ const char_type *host,
            _In_ const std::uint16_t port,
            _In_ const bool tls = true);


        blob get(_In_z_ const char_type *resource,
            _In_opt_z_ const char_type *version = DVSL("1.0"));

        dataverse_connection& operator =(
            _Inout_ dataverse_connection&& rhs) noexcept;

    private:

        detail::dataverse_connection_impl& check_not_disposed(void) const;

        detail::dataverse_connection_impl *_impl;

    };

} /* namespace dataverse */
} /* namespace visus */
