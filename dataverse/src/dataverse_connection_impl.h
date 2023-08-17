// <copyright file="dataverse_connection_impl.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include <curl/curl.h>

#include "dataverse/types.h"

#include "curl_error_category.h"
#include "curlm_error_category.h"
#include "errors.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// The head of an in-flight context of an asynchronous I/O operation.
    /// </summary>
    struct dataverse_connection_impl final {

        typedef std::unique_ptr<CURL, decltype(&::curl_easy_cleanup)> curl_type;
        typedef std::unique_ptr<CURLM, decltype(&::curl_multi_cleanup)> curlm_type;
        typedef std::unique_ptr<curl_mime, decltype(&::curl_mime_free)> mime_type;
        typedef std::unique_ptr<curl_slist, decltype(&::curl_slist_free_all)>
            string_list_type;

        std::vector<char> api_key;
        std::string base_path;
        curl_type curl;

        /// <summary>
        /// Overwrites <paramref name="vector" /> with zeros.
        /// </summary>
        template<class TElement, class TAlloc>
        static inline void secure_zero(
                _Inout_ std::vector<TElement, TAlloc>& vector) {
#if defined(_WIN32)
            ::SecureZeroMemory(vector.data(), vector.size() * sizeof(TElement));
#else /* defined(_WIN32) */
            ::memset(vector.data(), vector.size() * sizeof(TElement));
#endif /* defined(_WIN32) */
        }

        /// <summary>
        /// Overwrites all list members with zeros.
        /// </summary>
        static void secure_zero(_Inout_ string_list_type& list);

        dataverse_connection_impl(void);

        string_list_type add_auth_header(_In_opt_ string_list_type&& headers
            = string_list_type(nullptr, &::curl_slist_free_all)) const;

        inline ~dataverse_connection_impl(void) {
            secure_zero(this->api_key);
        }

        inline curlm_type make_curlm(void) const {
            return curlm_type(::curl_multi_init(), &::curl_multi_cleanup);
        }

        inline mime_type make_mime(void) const {
            auto curl = this->curl.get();
            return mime_type(::curl_mime_init(curl), &::curl_mime_free);
        }

        std::string make_url(_In_opt_z_ const char_type *resource) const;
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
