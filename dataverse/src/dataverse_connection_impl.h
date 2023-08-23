// <copyright file="dataverse_connection_impl.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <atomic>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#include <curl/curl.h>

#include "dataverse/convert.h"
#include "dataverse/event.h"

#include "curl_error_category.h"
#include "curlm_error_category.h"
#include "errors.h"


namespace visus {
namespace dataverse {
namespace detail {

    /* Forward declarations. */
    struct io_context;


    /// <summary>
    /// The head of an in-flight context of an asynchronous I/O operation.
    /// </summary>
    struct dataverse_connection_impl final {

        typedef std::unique_ptr<CURL, decltype(&::curl_easy_cleanup)> curl_type;
        typedef std::unique_ptr<CURLM, decltype(&::curl_multi_cleanup)> curlm_type;
        typedef std::unique_ptr<curl_mime, decltype(&::curl_mime_free)> mime_type;
        typedef std::unique_ptr<curl_slist, decltype(&::curl_slist_free_all)>
            string_list_type;

        /// <summary>
        /// Creates a new cURL easy handle.
        /// </summary>
        static curl_type make_curl(void);

        /// <summary>
        /// Creates a new MIME handle.
        /// </summary>
        static mime_type make_mime(_In_ const curl_type& curl);

        /// <summary>
        /// Checks the code and throws a exception if it does not indicate
        ///  success.
        /// </summary>
        static void check_code(_In_ const CURLcode code);

        /// <summary>
        /// Checks the code and throws a exception if it does not indicate
        ///  success.
        /// </summary>
        static void check_code(_In_ const CURLMcode code);

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

        std::vector<char> api_key;
        std::string base_path;
        curlm_type curlm;
        event_type curlm_event;
        std::atomic<bool> curlm_running;
        std::thread curlm_worker;
        int timeout;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        dataverse_connection_impl(void);

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~dataverse_connection_impl(void);

        /// <summary>
        /// Add the Dataverse authentication header to the given request.
        /// </summary>
        void add_auth_header(_In_ std::unique_ptr<io_context>& ctx) const;

        /// <summary>
        /// Makes an ASCII URL string from the given input.
        /// </summary>
        std::string make_url(_In_opt_z_ const wchar_t *resource) const;

        /// <summary>
        /// Makes an ASCII URL string from the given input.
        /// </summary>
        inline std::string make_url(_In_ const std::wstring& resource) const {
            return this->make_url(resource.c_str());
        }

        /// <summary>
        /// Makes an ASCII URL string from the given input.
        /// </summary>
        std::string make_url(_In_ const const_narrow_string& resource) const;

        /// <summary>
        /// Process the given I/O using curlm.
        /// </summary>
        void process(_Inout_ std::unique_ptr<io_context>&& request);

        /// <summary>
        /// The entry point of the curlm thread.
        /// </summary>
        void run_curlm(void);
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
