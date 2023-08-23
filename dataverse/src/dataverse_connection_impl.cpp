// <copyright file="dataverse_connection_impl.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse_connection_impl.h"

#include <cassert>
#include <stdexcept>

#include "dataverse/convert.h"

#include "io_context.h"
#include "curl_error_category.h"
#include "curlm_error_category.h"
#include "thread_name.h"


/*
 * visus::dataverse::detail::dataverse_connection_impl::make_curl
 */
visus::dataverse::detail::dataverse_connection_impl::curl_type
visus::dataverse::detail::dataverse_connection_impl::make_curl(void) {
    curl_type retval(::curl_easy_init(), &::curl_easy_cleanup);
    if (!retval) {
        throw std::bad_alloc();
    }

    {
        auto status = ::curl_easy_setopt(retval.get(),
            CURLOPT_USERAGENT,
            "Dataverse++");
        if (status != CURLE_OK) {
            throw std::system_error(status, curl_category());
        }
    }

    return retval;
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::make_mime
 */
visus::dataverse::detail::dataverse_connection_impl::mime_type
visus::dataverse::detail::dataverse_connection_impl::make_mime(
        _In_ const curl_type& curl) {
    mime_type retval(::curl_mime_init(curl.get()), &::curl_mime_free);
    if (!retval) {
        throw std::bad_alloc();
    }
    return retval;
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::check_code
 */
void visus::dataverse::detail::dataverse_connection_impl::check_code(
        _In_ const CURLcode code) {
    if (code != CURLE_OK) {
        throw std::system_error(code, curl_category());
    }
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::check_code
 */
void visus::dataverse::detail::dataverse_connection_impl::check_code(
        _In_ const CURLMcode code) {
    if (code != CURLM_OK) {
        throw std::system_error(code, curlm_category());
    }
}


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
    : curlm(::curl_multi_init(), &::curl_multi_cleanup),
        curlm_event(create_event()),
        timeout(1000) { }


/*
 * ...::detail::dataverse_connection_impl::~dataverse_connection_impl
 */
visus::dataverse::detail::dataverse_connection_impl::~dataverse_connection_impl(
        void) {
    secure_zero(this->api_key);

    this->curlm_running = false;        // Disable the worker
    set_event(this->curlm_event);       // Make sure the worker reads the state.

    // Wait for the worker.
    if (this->curlm_worker.joinable()) {
        this->curlm_worker.join();
    }

    destroy_event(this->curlm_event);
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::add_auth_header
 */
void visus::dataverse::detail::dataverse_connection_impl::add_auth_header(
        _In_ std::unique_ptr<io_context>& ctx) const {
    if (ctx && !this->api_key.empty()) {
        static constexpr const char name[] = "X-Dataverse-key: ";
        std::vector<char> header(name, name + sizeof(name) - 1);
        header.insert(header.end(), this->api_key.begin(), this->api_key.end());

        auto old_headers = ctx->headers.release();
        ctx->headers.reset(::curl_slist_append(old_headers, header.data()));
        secure_zero(header);

        if (!ctx->headers) {
            ctx->headers.reset(old_headers);
            throw std::bad_alloc();
        }
    }
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


/*
 * visus::dataverse::detail::dataverse_connection_impl::process
 */
void visus::dataverse::detail::dataverse_connection_impl::process(
        _Inout_ std::unique_ptr<io_context>&& request) {
    assert(request != nullptr);
    check_code(::curl_multi_add_handle(this->curlm.get(), request->curl.get()));

    auto expected = false;
    if (this->curlm_running.compare_exchange_strong(expected, true)) {
        this->curlm_worker = std::thread(&dataverse_connection_impl::run_curlm,
            this);
    }

    // The io_context is now owned by the processing thread.
    request.release();

    // Make sure that the thread is awake.
    set_event(this->curlm_event);
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::run_curlm
 */
void visus::dataverse::detail::dataverse_connection_impl::run_curlm(void) {
    set_thread_name("Dataverse++ I/O thread");

    while (this->curlm_running.load()) {
        CURLMsg *msg = nullptr;
        int remaining = 0;

        // Have cURL do its stuff as long as there is work left.
        do {
            auto status = ::curl_multi_perform(this->curlm.get(), &remaining);
            if (remaining != 0) {
                // If there is work left, wait for it to become ready.
                status = ::curl_multi_poll(this->curlm.get(), nullptr, 0,
                    this->timeout, nullptr);
            }

            if (status != CURLM_OK) {
                // If the poll failed, there is nothing left to do. Cf. 
                // https://curl.se/libcurl/c/multi-app.html
                remaining = 0;
            }
        } while (remaining != 0);

        // Check how the transfers went.
        while ((msg = ::curl_multi_info_read(this->curlm.get(), &remaining))
                != nullptr) {
            if (msg->msg == CURLMSG_DONE) {
                auto ctx = io_context::get(msg->easy_handle);

                if (ctx) {
                    if (msg->data.result == CURLE_OK) {
                        ctx->on_response(ctx->response, ctx->client_data);
                    } else {
                        std::system_error e(msg->data.result, curl_category());
                        ctx->on_error(msg->data.result, e.what(), e.what(),
#if defined(_WIN32)
                            CP_ACP,
#else /* defined(_WIN32) */
                            nullptr,
#endif /* defined(_WIN32) */
                            ctx->client_data);
                    }
                }

                // Recycle the context including the cURL handle and input data.
                io_context::recycle(std::move(ctx));
            } /* if (msg->msg == CURLMSG_DONE) */
        }

        // Wait for new I/O being queued or the thread is asked to exit.
        //wait_event(this->curlm_event, 1000);
    } /* while (this->curlm_running.load()) */
}
