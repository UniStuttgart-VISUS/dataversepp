// <copyright file="event_handlers.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <stdexcept>
#include <system_error>

#include "dataverse/dataverse_connection.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// Invokes the given error handler with the given exception.
    /// </summary>
    /// <param name="on_error"></param>
    /// <param name="ex"></param>
    /// <param name="client_data"></param>
    void invoke_handler(
        _In_opt_ const dataverse_connection::on_error_type on_error,
        _In_ const std::system_error& ex,
        _In_opt_ void *client_data);

    /// <summary>
    /// Invokes the given error handler with the given exception.
    /// </summary>
    /// <param name="on_error"></param>
    /// <param name="ex"></param>
    /// <param name="client_data"></param>
    void invoke_handler(
        _In_opt_ const dataverse_connection::on_error_type on_error,
        _In_ const std::exception& ex,
        _In_opt_ void *client_data);

    /// <summary>
    /// Invokes the given error handler if it is valid.
    /// </summary>
    /// <param name="on_error"></param>
    /// <param name="message"></param>
    /// <param name="category"></param>
    /// <param name="code_page"></param>
    /// <param name="client_data"></param>
    void invoke_handler(
        _In_opt_ const dataverse_connection::on_error_type on_error,
        _In_z_ const char *message,
        _In_z_ const char *category,
        _In_ narrow_string::code_page_type code_page,
        _In_opt_ void *client_data);

    /// <summary>
    /// Invokes the given error handler specifying that an unexpected error
    /// occurred.
    /// </summary>
    /// <param name="on_error"></param>
    /// <param name="client_data"></param>
    void invoke_handler(
        _In_opt_ const dataverse_connection::on_error_type on_error,
        _In_opt_ void *client_data);

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
