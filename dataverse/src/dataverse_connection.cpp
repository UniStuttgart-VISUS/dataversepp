// <copyright file="dataverse_connection.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/dataverse_connection.h"

#include <iterator>

#include <nlohmann/json.hpp>

#include "dataverse/convert.h"

#include "dataverse_connection_impl.h"
#include "io_context.h"


/*
 * visus::dataverse::dataverse_connection::dataverse_connection
 */
visus::dataverse::dataverse_connection::dataverse_connection(void)
        : _impl(new detail::dataverse_connection_impl()) { }


/*
 * visus::dataverse::dataverse_connection::dataverse_connection
 */
visus::dataverse::dataverse_connection::dataverse_connection(
        _Inout_ dataverse_connection&& rhs) noexcept : _impl(rhs._impl) {
    rhs._impl = nullptr;
}


/*
 * visus::dataverse::dataverse_connection::~dataverse_connection
 */
visus::dataverse::dataverse_connection::~dataverse_connection(void) {
    delete this->_impl;
}


/*
 * visus::dataverse::dataverse_connection::api_key
 */
visus::dataverse::dataverse_connection& 
visus::dataverse::dataverse_connection::api_key(
        _In_opt_z_ const char_type *api_key) {
    auto& i = this->check_not_disposed();

    // Make sure that the old API key is erased from memory.
    detail::dataverse_connection_impl::secure_zero(i.api_key);

    if (api_key != nullptr) {
        const auto len = std::char_traits<char_type>::length(api_key);
        i.api_key.resize(len + 1);
        to_ascii(i.api_key.data(), i.api_key.size(), api_key, 0);
        i.api_key.back() = static_cast<char_type>(0);
    } else {
        this->_impl->api_key.clear();
    }

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::base_path
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::base_path(
        _In_z_ const char_type *base_path) {
    auto& i = this->check_not_disposed();

    if (base_path != nullptr) {
        i.base_path = to_ascii(base_path);
    } else {
        i.base_path.clear();
    }

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::get
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::get(
        _In_opt_z_ const char_type *resource,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    if (on_response == nullptr) {
        throw std::invalid_argument("The response handler must be valid.");
    }
    if (on_error == nullptr) {
        throw std::invalid_argument("The error handler must be valid.");
    }
    
    auto& i = this->check_not_disposed();

    auto c = detail::io_context::create();
    c->client_data = context;

    std::string url = i.make_url(resource);
    ::curl_easy_setopt(i.curl.get(), CURLOPT_URL, url.c_str());
    ::curl_easy_setopt(i.curl.get(), CURLOPT_WRITEDATA, c.get());

    auto headers = i.add_auth_header();
    ::curl_easy_setopt(i.curl.get(), CURLOPT_HTTPHEADER, headers.get());

    auto status = ::curl_easy_perform(i.curl.get());
    if (status != CURLE_OK) {
        throw std::system_error(status, detail::curl_category());
    }

    on_response(c->response, c->client_data);
    detail::io_context::recycle(std::move(c));

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::make_form
 */
visus::dataverse::form_data visus::dataverse::dataverse_connection::make_form(
        void) const {
    return form_data(this->check_not_disposed().curl.get());
}


/*
 * visus::dataverse::dataverse_connection::post
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::post(
        _In_opt_z_ const char_type *resource,
        _In_ const form_data& form,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    if (!form) {
        throw std::invalid_argument("The form must be valid.");
    }
    if (on_response == nullptr) {
        throw std::invalid_argument("The response handler must be valid.");
    }
    if (on_error == nullptr) {
        throw std::invalid_argument("The error handler must be valid.");
    }

    auto& i = this->check_not_disposed();

    auto c = detail::io_context::create();
    c->client_data = context;

    std::string url = i.make_url(resource);
    ::curl_easy_setopt(i.curl.get(), CURLOPT_URL, url.c_str());
    ::curl_easy_setopt(i.curl.get(), CURLOPT_WRITEDATA, c.get());
    ::curl_easy_setopt(i.curl.get(), CURLOPT_MIMEPOST, form._form);

    auto headers = i.add_auth_header();
    ::curl_easy_setopt(i.curl.get(), CURLOPT_HTTPHEADER, headers.get());

    auto status = ::curl_easy_perform(i.curl.get());
    if (status != CURLE_OK) {
        throw std::system_error(status, detail::curl_category());
    }

    on_response(c->response, c->client_data);
    detail::io_context::recycle(std::move(c));

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::upload
 */
visus::dataverse::dataverse_connection &
visus::dataverse::dataverse_connection::upload(
        _In_z_ const char_type *persistent_id,
        _In_z_ const char_type *path,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::basic_string<char_type>(DVSL("/datasets/")
        DVSL(":persistentId/add?persistentId=")) + persistent_id;
    return this->post(url.c_str(),
        this->make_form().add_file(DVSL("file"), path),
        on_response,
        on_error,
        context);
}


/*
 * visus::dataverse::dataverse_connection::upload
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::upload(
        _In_z_ const char_type *persistent_id,
        _In_z_ const char_type *path,
        _In_z_ const char_type *description,
        _In_z_ const char_type *directory,
        const char_type **categories,
        _In_ const std::size_t cnt_categories,
        _In_ const bool restricted,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::basic_string<char_type>(DVSL("/datasets/")
        DVSL(":persistentId/add?persistentId=")) + persistent_id;

    // Note: It is important to pass all JSON input through to_utf8, because in
    // contrast to the beliefs of MegaMol authors, char * is not necessarily
    // UTF-8, but might use a different code page, specifically when using
    // hard-coded string literals. This makes the whole thing explode if anyone
    // passes anything but 7-bit ASCII as input to the method. The literals are
    // OK for the labels, because we know(TM) that this is 7-bit ASCII.
    auto json = nlohmann::json::object({
        { "description", to_utf8(description) },
        { "directoryLabel", to_utf8(directory) },
        { "restrict", restricted },
        { "categories", nlohmann::json::array() }
    });

    if (categories != nullptr) {
        auto& cats = json["categories"];
        for (std::size_t i = 0; i < cnt_categories; ++i) {
            cats.push_back(to_utf8(categories[i]));
        }
    }

    const auto dump = json.dump();
    const auto desc = reinterpret_cast<const std::uint8_t *>(dump.data());
    const auto size = dump.size();

    return this->post(url.c_str(),
        this->make_form()
            .add_file(DVSL("file"), path)
            .add_field(DVSL("jsonData"), desc, size),
        on_response,
        on_error,
        context);
}


/*
 * visus::dataverse::dataverse_connection::operator =
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::operator =(
        _Inout_ dataverse_connection&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        delete this->_impl;
        this->_impl = rhs._impl;
        rhs._impl = nullptr;
    }

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::check_not_disposed
 */
visus::dataverse::detail::dataverse_connection_impl&
visus::dataverse::dataverse_connection::check_not_disposed(void) const {
    if (this->_impl == nullptr) {
        throw std::system_error(ERROR_INVALID_STATE, std::system_category());
    } else {
        return *this->_impl;
    }
}
