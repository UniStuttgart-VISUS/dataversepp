// <copyright file="file_properties.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <nlohmann/json.hpp>

#include "dataverse/narrow_string.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// Gets the file properties required for a direct upload for the file at
    /// the specified location.
    /// </summary>
    /// <param name="path">The path to the file.</param>
    /// <returns>A JSON object holding the relevant information to be merged
    /// into the description of a direct upload.</returns>
    /// <exception cref="std::system_error">If any of the required properties
    /// could not be determined.</exception>
    nlohmann::json get_file_properties(_In_ const wchar_t *path);

    /// <summary>
    /// Gets the file properties required for a direct upload for the file at
    /// the specified location.
    /// </summary>
    /// <param name="path">The path to the file.</param>
    /// <returns>A JSON object holding the relevant information to be merged
    /// into the description of a direct upload.</returns>
    /// <exception cref="std::system_error">If any of the required properties
    /// could not be determined.</exception>
    nlohmann::json get_file_properties(_In_ const const_narrow_string& path);

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
