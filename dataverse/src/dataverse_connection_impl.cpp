// <copyright file="dataverse_connection_impl.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse_connection_impl.h"

#if defined(_WIN32)
#include <tchar.h>
#endif /* defined(_WIN32) */

#include <cassert>
#include <stdexcept>
#include <string>

#include "dataverse/convert.h"

#include "curl_error_category.h"
#include "curlm_error_category.h"
#include "invoke_handler.h"
#include "io_context.h"
#include "on_exit.h"
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
        timeout(1000) { }


/*
 * ...::detail::dataverse_connection_impl::~dataverse_connection_impl
 */
visus::dataverse::detail::dataverse_connection_impl::~dataverse_connection_impl(
        void) {
    secure_zero(this->api_key);

    // Ask the worker thread to stop: here, we can only switch from running to
    // stopping. If the thread is not running, we simply do nothing. Otherwise,
    // we try to perform the aforementioned switch, which should only fail if
    // the thread is in a transitional state like starting or stopping. In the
    // former case, we need to retry until the thread has actually started and
    // we can ask it to exit.
    assert(this->worker_state.is_lock_free());
    auto expected = curl_worker_state::running;
    auto stopped = (this->worker_state.load() == curl_worker_state::stopped);
    while (!stopped && !this->worker_state.compare_exchange_strong(expected,
            curl_worker_state::stopping)) {
        // If the transition failed, because the thread was initially in the
        // stopping state and has now transitioned to stopped, we must not try
        // again. The new expected value should never be stopping, because only
        // the destructor can set that, but we still include that here for
        // paranoia in the release build.
        assert(expected != curl_worker_state::stopping);
        stopped == (expected == curl_worker_state::stopped)
            || (expected == curl_worker_state::stopping);
    }

    // Wait for the worker to exit such that we have no dangling pointers.
    if (this->curlm_worker.joinable()) {
        this->curlm_worker.join();
        assert(this->worker_state.load() == curl_worker_state::stopped);
    }
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

    auto expected = curl_worker_state::stopped;
    if (this->worker_state.compare_exchange_strong(expected,
            curl_worker_state::starting)) {
        // If the worker thread was not running and not in a transitional state
        // either, we are the ones who must start it.
        this->curlm_worker = std::thread(&dataverse_connection_impl::run_curlm,
            this);

    } else if (expected == curl_worker_state::stopping) {
        // New work was being queued while the destructor of the connection
        // object was running. This is an error in the application logic.
        throw std::logic_error("New work has been queued to the asynchronous "
            "web API while the connection object is being destructed.");
    }

    // The io_context is now owned by the processing thread.
    request.release();
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::run_curlm
 */
void visus::dataverse::detail::dataverse_connection_impl::run_curlm(void) {
    set_thread_name("Dataverse++ I/O thread");

    // Install an exit handler that informs all other threads when this thread
    // is leaving.
    on_exit([this](void) {
        auto expected = curl_worker_state::stopping;
        if (!this->worker_state.compare_exchange_strong(expected,
                curl_worker_state::stopped)) {
            // It is a catastrophic failure if someone manipulated the state at
            // this point.
            std::abort();
        }
    });

    // Inform all other threads that we are now running.
    {
        auto expected = curl_worker_state::starting;
        if (!this->worker_state.compare_exchange_strong(expected,
            curl_worker_state::running)) {
            // This should basically be unreachable.
            throw std::logic_error("The web API worker thread detected an "
                "illegal state change while starting up. If the worker thread "
                "is about to start, only the thread itself is allowed to "
                "transition from the starting state to the running state. No "
                "other state transitions are allowed at this point.");
        }
    }

    while (this->worker_state.load() == curl_worker_state::running) {
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
                if (!ctx) {
                    // The context is invalid, which should never happen as only
                    // we can add requests to 'curlm' and all code in the
                    // connection class must add the context as private data.
                    // Nevertheless, we do not want the I/O thread to crash in
                    // release builds, so we just ignore it.
                    assert(false);
                    continue;
                }

                assert(ctx != nullptr);
                assert(ctx->on_error != nullptr);
                assert(ctx->on_response != nullptr);
                if (msg->data.result == CURLE_OK) {
                    // Request succeeded, but we need to check the HTTP response
                    // to report API errors.
                    long code = 0;
                    const auto status = ::curl_easy_getinfo(msg->easy_handle,
                        CURLINFO_RESPONSE_CODE, &code);
                    if (status == CURLE_OK) {
                        if (code < 400) {
                            // This was a total success.
                            ctx->on_response(ctx->response, ctx->client_data);
                        } else {
                            // cURL succeeded, but the request failed on a
                            // protocol or application level.
                            try {
                                const auto response = std::string(
                                    ctx->response.as<char>(),
                                    ctx->response.size());
                                const auto api_response = nlohmann::json::parse(
                                    response);
                                const auto msg = api_response["message"]
                                    .get<std::string>();
                                invoke_handler(ctx->on_error,
                                    msg.c_str(),
                                    "API",
                                    dataversepp_code_page,
                                    ctx->client_data);
                            } catch (...) {
                                std::string msg("HTTP ");
                                msg += std::to_string(code);
                                invoke_handler(ctx->on_error,
                                    msg.c_str(),
                                    "HTTP",
                                    dataversepp_code_page,
                                    ctx->client_data);
                            }
                        }
                    } else {
                        // Failed to retrieve the HTTP code, which should not
                        // happen for the requests from our connection objects,
                        // but we still report that to the user.
                        std::system_error e(status, curl_category());
                        invoke_handler(ctx->on_error, e, ctx->client_data);
                    }

                } else {
                    // Request failed.
                    std::system_error e(msg->data.result, curl_category());
                    invoke_handler(ctx->on_error, e, ctx->client_data);
                } /* if (msg->data.result == CURLE_OK) */

                // Recycle the context including the cURL handle and input data.
                io_context::recycle(std::move(ctx));
            } /* if (msg->msg == CURLMSG_DONE) */
        } /*  while ((msg = ::curl_multi_info_read(... */
    } /* while (this->curlm_running.load()) */

    assert(this->worker_state.load() == curl_worker_state::stopping);
}
