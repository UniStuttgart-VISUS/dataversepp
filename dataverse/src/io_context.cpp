// <copyright file="io_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "io_context.h"

#include <cassert>

#include "dataverse/convert.h"


/*
 * visus::dataverse::detail::io_context::create
 */
std::unique_ptr<visus::dataverse::detail::io_context>
visus::dataverse::detail::io_context::create(_In_ CURL *curl) {
    std::lock_guard<decltype(lock)> l(lock);
    std::unique_ptr<visus::dataverse::detail::io_context> retval;

    if (cache.empty()) {
        retval.reset(new io_context());
    } else {
        retval = std::move(cache.back());
        retval->headers.reset();
        retval->on_error = nullptr;
        retval->on_response = nullptr;
        retval->response.clear();
        cache.pop_back();
    }

    if (curl != nullptr) {
        // If requested, use the given cURL handle instead of our own.
        retval->curl.reset(curl);
    }
    assert(retval->curl);

    // Set the output callback and the context for it.
    retval->option(CURLOPT_WRITEFUNCTION, detail::io_context::write_response);
    retval->option(CURLOPT_WRITEDATA, retval.get());

    // Set the private pointer such that we can extract the context from
    // the CURLM processing thread.
    retval->option(CURLOPT_PRIVATE, retval.get());

    return retval;
}


/*
 * visus::dataverse::detail::io_context::create
 */
std::unique_ptr<visus::dataverse::detail::io_context>
visus::dataverse::detail::io_context::create(_In_ CURL *curl,
            _In_z_ const std::string& url,
            _In_ dataverse_connection::on_response_type on_response,
            _In_ dataverse_connection::on_error_type on_error,
            _In_opt_ void *client_data) {
    auto retval = create(curl);
    assert(retval != nullptr);
    retval->client_data = client_data;
#if defined(DATAVERSE_WITH_JSON)
    retval->on_api_response = nullptr;
#endif /* defined(DATAVERSE_WITH_JSON) */
    retval->on_error = on_error;
    retval->on_response = on_response;
    retval->option(CURLOPT_URL, url.c_str());
    return retval;
}


/*
 * visus::dataverse::detail::io_context::create
 */
std::unique_ptr<visus::dataverse::detail::io_context>
visus::dataverse::detail::io_context::create(_In_z_ const std::string& url,
        _In_ dataverse_connection::on_response_type on_response,
        _In_ dataverse_connection::on_error_type on_error,
        _In_opt_ void *client_data) {
    return create(nullptr, url, on_response, on_error, client_data);
}


/*
 * visus::dataverse::detail::io_context::get
 */
std::unique_ptr<visus::dataverse::detail::io_context>
visus::dataverse::detail::io_context::get(_In_ CURL *curl) {
    std::unique_ptr<io_context> retval;

    if (curl != nullptr) {
        io_context *tmp;
        dataverse_connection_impl::check_code(::curl_easy_getinfo(
            curl,
            CURLINFO_PRIVATE,
            &tmp));
        retval.reset(tmp);
    }

    return retval;
}


/*
 * visus::dataverse::detail::io_context::read_request
 */
std::size_t CALLBACK visus::dataverse::detail::io_context::read_request(
        _Out_writes_bytes_(cnt * size) char *dst,
        _In_ const size_t size,
        _In_ const size_t cnt,
        _In_opt_ void *context) {
    auto that = static_cast<detail::io_context *>(context);
    const auto requested = size * cnt;
    const auto offset = that->request_size - that->request_remaining;
    const auto retval = (std::min)(requested, that->request_remaining);

    if (retval > 0) {
        ::memcpy(dst, that->request + offset, retval);
        that->request_remaining -= retval;
    } else {
        that->delete_request();
    }

    return retval;
}


/*
 * visus::dataverse::detail::io_context::recycle
 */
void visus::dataverse::detail::io_context::recycle(
        _Inout_ std::unique_ptr<io_context>&& context) {
    if (context != nullptr) {
        context->delete_request();
        context->curl = dataverse_connection_impl::make_curl();
        std::lock_guard<decltype(lock)> l(lock);
        cache.push_back(std::move(context));
    }
}


/*
 * visus::dataverse::detail::io_context::write_response
 */
std::size_t CALLBACK visus::dataverse::detail::io_context::write_response(
        _In_reads_bytes_(cnt *element_size) const void *data,
        _In_ const std::size_t size,
        _In_ const std::size_t cnt,
        _In_ void *context) {
    auto that = static_cast<io_context *>(context);
    const auto offset = that->response.size();
    const auto retval = cnt * size;

    that->response.truncate(retval + offset);
    ::memcpy(that->response.at(offset), data, retval);

    return retval;
}


/*
 * visus::dataverse::detail::io_context::io_context
 */
visus::dataverse::detail::io_context::io_context(void)
    : client_data(nullptr),
        curl(std::move(dataverse_connection_impl::make_curl())),
        headers(nullptr, &::curl_slist_free_all),
        on_error(nullptr),
        on_response(nullptr),
        request(nullptr),
        request_deleter(nullptr),
        request_remaining(0),
        request_size(0) { }


/*
 * visus::dataverse::detail::io_context::~io_context
 */
visus::dataverse::detail::io_context::~io_context(void) {
    this->delete_request();
}


/*
 * visus::dataverse::detail::io_context::add_header
 */
void visus::dataverse::detail::io_context::add_header(
        _In_opt_z_ const char *header) {
    if (header != nullptr) {
        auto old_headers = this->headers.release();
        this->headers.reset(::curl_slist_append(old_headers, header));

        if (!this->headers) {
            this->headers.reset(old_headers);
            throw std::bad_alloc();
        }
    }
}


/*
 * visus::dataverse::detail::io_context::apply_headers
 */
void visus::dataverse::detail::io_context::apply_headers(void) {
    if (this->curl) {
        this->option(CURLOPT_HTTPHEADER, this->headers.get());
    }
}


/*
 * visus::dataverse::detail::dataverse_connection_impl::add_content_type
 */
void visus::dataverse::detail::io_context::content_type(
        _In_ const wchar_t *content_type) {
    if (content_type != nullptr) {
        auto h = "Content-Type: " + to_ascii(content_type);
        this->add_header(h.c_str());
    }
}


/*
 * visus::dataverse::detail::io_context::delete_request
 */
void visus::dataverse::detail::io_context::delete_request(void) {
    if ((this->request_deleter != nullptr) && (this->request != nullptr)) {
        this->request_deleter(this->request);
    }

    this->request = nullptr;
    this->request_deleter = nullptr;
}


/*
 * visus::dataverse::detail::io_context::prepare_request
 */
void visus::dataverse::detail::io_context::prepare_request(
        _In_reads_bytes_(cnt) const byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ const dataverse_connection::data_deleter_type deleter) {
    this->delete_request();
    this->request = data;
    this->request_size = cnt;
    this->request_remaining = cnt;

    this->option(CURLOPT_READFUNCTION, &detail::io_context::read_request);
    this->option(CURLOPT_READDATA, this);
    this->option(CURLOPT_INFILESIZE_LARGE, cnt);

}


/*
 * visus::dataverse::detail::io_context::cache
 */
std::vector<std::unique_ptr<visus::dataverse::detail::io_context>>
visus::dataverse::detail::io_context::cache;


/*
 * visus::dataverse::detail::io_context::lock
 */
std::mutex visus::dataverse::detail::io_context::lock;
