// <copyright file="direct_upload_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "direct_upload_context.h"

#if !defined(_WIN32)
#include <fcntl.h>
#endif /* !defined(_WIN32) */


/*
 * visus::dataverse::detail::direct_upload_context::forward_error
 */
void visus::dataverse::detail::direct_upload_context::forward_error(
        _In_ const int error_code,
        _In_z_ const char *message,
        _In_z_ const char *category,
        _In_ const narrow_string::code_page_type code_page,
        _In_opt_ void *context) {
    auto that = static_cast<direct_upload_context *>(context);
    assert(that != nullptr);
    assert(that->on_error != nullptr);
    that->on_error(error_code, message, category, code_page,
        that->user_context);
    delete that;
}


/*
 * visus::dataverse::detail::direct_upload_context::direct_upload_context
 */
visus::dataverse::detail::direct_upload_context::direct_upload_context(
        _In_ dataverse_connection_impl *connection,
        _In_ const dataverse_connection::on_response_type on_response,
        _In_ dataverse_connection::on_error_type on_error,
        _In_opt_ void *context)
    : connection(connection), on_error(on_error), on_response(on_response),
        user_context(context) {
#if !defined(_WIN32)
    this->_file = -1;
#endif /* !defined(_WIN32) */
}


/*
 * visus::dataverse::detail::direct_upload_context::~direct_upload_context
 */
visus::dataverse::detail::direct_upload_context::~direct_upload_context(
        void) {
#if !defined(_WIN32)
    if (this->_file != -1) {
        ::close(this->_file);
    }
#endif /* !defined(_WIN32) */
}


/*
 * visus::dataverse::detail::direct_upload_context::upload_url
 */
std::string visus::dataverse::detail::direct_upload_context::upload_url(
        _In_ const blob& response) {
    const std::string utf8(response.as<char>(), response.size());
    const auto json = nlohmann::json::parse(utf8);

    if (json["status"] != "OK") {
        // We received an API error.
        const auto msg = json["message"].get<std::string>();
        throw std::runtime_error(msg.c_str());
    }

    // Remmeber the storage identifier for later.
    this->description["storageIdentifier"] = json["data"]["storageIdentifier"];

    return json["data"]["url"].get<std::string>();
}
