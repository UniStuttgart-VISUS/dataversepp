// <copyright file="direct_upload_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <string>
#include <stdexcept>
#include <system_error>

#include <nlohmann/json.hpp>

#include "dataverse/dataverse_connection.h"

#include "io_context.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// The context used for performing a multi-stage direct upload to the
    /// S3 storage backend.
    /// </summary>
    struct direct_upload_context final {

        /// <summary>
        /// The connection to use for follow-up requests.
        /// </summary>
        dataverse_connection *connection;

        /// <summary>
        /// The description posted when registering the file.
        /// </summary>
        nlohmann::json description;

        /// <summary>
        /// The error handler installed by the caller.
        /// </summary>
        dataverse_connection::on_error_type on_error;

        /// <summary>
        /// The final result handler installed by the caller.
        /// </summary>
        dataverse_connection::on_response_type on_response;

        /// <summary>
        /// The registration URL where the metadata need to be posted to.
        /// </summary>
        std::wstring registration_url;

        /// <summary>
        /// The user-specified context pointer to be passed to
        /// <see cref="on_error" /> and <see cref="on_response" />.
        /// </summary>
        void *user_context;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        direct_upload_context(_In_ dataverse_connection *connection,
            _In_ const dataverse_connection::on_response_type on_response,
            _In_ dataverse_connection::on_error_type on_error,
            _In_opt_ void *context);

        /// <summary>
        /// Invokes <see cref="on_error" />.
        /// </summary>
        void invoke_on_error(_In_ const std::system_error& ex);

        /// <summary>
        /// Invokes <see cref="on_error" />.
        /// </summary>
        void invoke_on_error(_In_ const std::exception& ex);

        /// <summary>
        /// Invokes <see cref="on_error" />.
        /// </summary>
        void invoke_on_error(void);

        /// <summary>
        /// Process the response we received on the request for an upload URL by
        /// setting the remaining data in the description and returning the URL
        /// itself.
        /// </summary>
        std::wstring upload_url(_In_ const blob& response);
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
