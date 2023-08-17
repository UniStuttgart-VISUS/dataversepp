// <copyright file="dataverse_connection.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/dataverse_connection.h"

#include <iterator>

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
 * visus::dataverse::dataverse_connection::connect
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::connect(
        _In_z_ const char_type *host,
        _In_ const std::uint16_t port,
        _In_ const bool tls) {
    auto& i = this->check_not_disposed();
    return *this;
}


/*
 * visus::dataverse::dataverse_connection::get
 */
void visus::dataverse::dataverse_connection::get(
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
    ::curl_easy_setopt(i.curl.get(), CURLOPT_USERAGENT, "Dataverse++");

    c->headers = i.make_headers();
    ::curl_easy_setopt(i.curl.get(), CURLOPT_HEADER, c->headers.get());

    auto status = ::curl_easy_perform(i.curl.get());
    detail::dataverse_connection_impl::secure_zero(c->headers);
    if (status != CURLE_OK) {
        throw std::system_error(status, detail::curl_category());
    }

    on_response(c->response, c->client_data);
    detail::io_context::recycle(std::move(c));
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
