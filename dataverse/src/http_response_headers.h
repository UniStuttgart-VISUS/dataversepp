// <copyright file="http_request_headers.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "dataverse/types.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// The implementation of the response headers we got from a remote side and
    /// which we parse in-place.
    /// </summary>
    struct http_response_header final {

        typedef std::string ascii_type;

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
