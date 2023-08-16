// <copyright file="http_response.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "dataverse/http_headers.h"


namespace visus {
namespace dataverse {

    /// <summary>
    /// Represents an HTTP response received from the server.
    /// </summary>
    class DATAVERSE_API http_response final {

    public:

        /// <summary>
        /// The type to represent a byte.
        /// </summary>
        typedef http_headers::byte_type byte_type;

        explicit http_response(
            _In_reads_bytes_opt_(size) const byte_type *data = nullptr,
            _In_ const std::size_t size = 0);

        http_response(_Inout_ http_response&& rhs) noexcept;

        ~http_response(void) noexcept;

        http_response& append(_In_reads_bytes_(size) const byte_type *data,
            _In_ const std::size_t size);

        http_response& operator =(_Inout_ http_response&& rhs) noexcept;

    private:

        byte_type *_data;
        http_headers _headers;
        std::size_t _size;

    };

} /* namespace dataverse */
} /* namespace visus */
