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

#include "dataverse_connection_impl.h"
#include "invoke_handler.h"
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
        /// Forwards the error to <see cref="on_error" /> and deletes the
        /// <paramref name="context" />.
        /// </summary>
        static void forward_error(_In_ const int error_code,
            _In_z_ const char *message,
            _In_z_ const char *category,
            _In_ const narrow_string::code_page_type code_page,
            _In_opt_ void *context);

        /// <summary>
        /// The connection to use for follow-up requests.
        /// </summary>
        dataverse_connection_impl *connection;

        /// <summary>
        /// The description posted when registering the file.
        /// </summary>
        nlohmann::json description;

        /// <summary>
        /// The handle of the file to be uploaded.
        /// </summary>
        /// <remarks>
        /// We need to preserve this here for direct uploads, because we do not
        /// have access to the <see cref="io_context" /> in this case.
        /// </remarks>
        io_context::file_type file;

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
        std::string registration_url;

        /// <summary>
        /// The user-specified context pointer to be passed to
        /// <see cref="on_error" /> and <see cref="on_response" />.
        /// </summary>
        void *user_context;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        direct_upload_context(_In_ dataverse_connection_impl *connection,
            _In_ const dataverse_connection::on_response_type on_response,
            _In_ dataverse_connection::on_error_type on_error,
            _In_opt_ void *context);

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~direct_upload_context(void);

        /// <summary>
        /// Runs <paramref name="function" /> in a try/catch and self-deletes
        /// in case of an error after invoking the error handler.
        /// </summary>
        template<class TFunction>
        inline void handle_errors(TFunction function) {
            try {
                function();
            } catch (std::system_error ex) {
                invoke_handler(this->on_error, ex, this->user_context);
                delete this;
            } catch (std::exception &ex) {
                invoke_handler(this->on_error, ex, this->user_context);
                delete this;
            } catch (...) {
                invoke_handler(this->on_error, this->user_context);
                delete this;
            }
        }

        /// <summary>
        /// Process the response we received on the request for an upload URL by
        /// setting the remaining data in the description and returning the URL
        /// itself.
        /// </summary>
        std::string upload_url(_In_ const blob& response);
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
