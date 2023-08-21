// <copyright file="direct_upload_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "direct_upload_context.h"


/*
 * visus::dataverse::detail::direct_upload_context::direct_upload_context
 */
visus::dataverse::detail::direct_upload_context::direct_upload_context(
        _In_ dataverse_connection *connection,
        _In_ const dataverse_connection::on_response_type on_response,
        _In_ dataverse_connection::on_error_type on_error,
        _In_opt_ void *context)
    : connection(connection), on_error(on_error), on_response(on_response),
        user_context(context) { }


/*
 * visus::dataverse::detail::direct_upload_context::invoke_on_error
 */
void visus::dataverse::detail::direct_upload_context::invoke_on_error(
        _In_ const std::system_error& ex) {
    if (this->on_error != nullptr) {
        this->on_error(ex.code().value(),
            ex.what(),
            ex.code().category().name(),
#if defined(_WIN32)
            CP_OEMCP,
#else /* defined(_WIN32) */
            nullptr,
#endif /* defined(_WIN32) */
            this->user_context);
    }
}


/*
 * visus::dataverse::detail::direct_upload_context::invoke_on_error
 */
void visus::dataverse::detail::direct_upload_context::invoke_on_error(
        _In_ const std::exception& ex) {
    if (this->on_error != nullptr) {
        this->on_error(0,
            ex.what(),
            "Generic STL Exception",
#if defined(_WIN32)
            CP_OEMCP,
#else /* defined(_WIN32) */
            nullptr,
#endif /* defined(_WIN32) */
            this->user_context);
    }
}


/*
 * visus::dataverse::detail::direct_upload_context::invoke_on_error
 */
void visus::dataverse::detail::direct_upload_context::invoke_on_error(void) {
    if (this->on_error != nullptr) {
        this->on_error(0,
            "An unexpected error was encountered while processing the request.",
            "Unexpected Exception",
#if defined(_WIN32)
            CP_OEMCP,
#else /* defined(_WIN32) */
            nullptr,
#endif /* defined(_WIN32) */
            this->user_context);
    }
}


/*
 * visus::dataverse::detail::direct_upload_context::upload_url
 */
std::wstring visus::dataverse::detail::direct_upload_context::upload_url(
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

    return from_utf8<wchar_t>(json["data"]["url"].get<std::string>());
}
