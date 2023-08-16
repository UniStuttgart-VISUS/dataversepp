// <copyright file="http_request_headers.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "dataverse/types.h"

#include <map>
#include <string>

#include "less_ci.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// The implementation of modifyable HTTP request headers.
    /// </summary>
    struct http_request_headers final {

        typedef std::string ascii_type;
        typedef less_ci<ascii_type> compare_type;

        std::multimap<ascii_type, ascii_type, compare_type> values;
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
