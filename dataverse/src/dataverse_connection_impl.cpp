// <copyright file="dataverse_connection_impl.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse_connection_impl.h"

#include <stdexcept>

#include "dataverse/convert.h"

#include "io_context.h"


/*
 * visus::dataverse::detail::dataverse_connection_impl::secure_zero
 */
void visus::dataverse::detail::dataverse_connection_impl::secure_zero(
        _Inout_ string_list_type& list) {
    auto cur = list.get();

    while (cur != nullptr) {
        if (cur->data != nullptr) {
#if defined(_WIN32)
            ::SecureZeroMemory(cur->data, ::strlen(cur->data));
#else /* defined(_WIN32) */
            ::memset(cur->data, ::strlen(cur->data));
#endif /* defined(_WIN32) */
        }

        cur = cur->next;
    }
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::dataverse_connection_impl
 */
visus::dataverse::detail::dataverse_connection_impl::dataverse_connection_impl(
        void)
        : curl(::curl_easy_init(), &::curl_easy_cleanup) {
    {
        auto status = ::curl_easy_setopt(this->curl.get(),
            CURLOPT_WRITEFUNCTION,
            detail::io_context::write_response);
        if (status != CURLE_OK) {
            throw std::system_error(status, curl_category());
        }
    }

    {
        auto status = ::curl_easy_setopt(this->curl.get(), CURLOPT_USERAGENT,
            "Dataverse++");
        if (status != CURLE_OK) {
            throw std::system_error(status, curl_category());
        }
    }
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::add_auth_header
 */
visus::dataverse::detail::dataverse_connection_impl::string_list_type
visus::dataverse::detail::dataverse_connection_impl::add_auth_header(
        _In_opt_ string_list_type&& headers) const {
    if (!this->api_key.empty()) {
        static constexpr const char name[] = "X-Dataverse-key: ";
        std::vector<char> header(name, name + sizeof(name) - 1);
        header.insert(header.end(), this->api_key.begin(), this->api_key.end());

        auto old_headers = headers.release();
        headers.reset(::curl_slist_append(old_headers, header.data()));
        secure_zero(header);

        if (!headers) {
            ::curl_slist_free_all(old_headers);
            throw std::bad_alloc();
        }
    }

    return std::move(headers);
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::make_curlm
 */
visus::dataverse::detail::dataverse_connection_impl::curlm_type
visus::dataverse::detail::dataverse_connection_impl::make_curlm(void) const {
    auto retval = curlm_type(::curl_multi_init(), &::curl_multi_cleanup);
    if (!retval) {
        throw std::bad_alloc();
    }
    return retval;
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::make_mime
 */
visus::dataverse::detail::dataverse_connection_impl::mime_type
visus::dataverse::detail::dataverse_connection_impl::make_mime(void) const {
    auto curl = this->curl.get();
    auto retval = mime_type(::curl_mime_init(curl), &::curl_mime_free);
    if (!retval) {
        throw std::bad_alloc();
    }
    return retval;
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::make_url
 */
std::string visus::dataverse::detail::dataverse_connection_impl::make_url(
        _In_opt_z_ const wchar_t *resource) const {
    return (resource != nullptr)
        ? this->base_path + to_ascii(resource)
        : this->base_path;
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::make_url
 */
std::string visus::dataverse::detail::dataverse_connection_impl::make_url(
        _In_ const const_narrow_string& resource) const {
    return (resource != nullptr)
        ? this->base_path + to_ascii(resource)
        : this->base_path;
}
