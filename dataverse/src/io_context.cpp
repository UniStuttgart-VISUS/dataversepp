// <copyright file="io_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "io_context.h"

#include "dataverse/convert.h"


/*
 * visus::dataverse::detail::io_context::create
 */
std::unique_ptr<visus::dataverse::detail::io_context>
visus::dataverse::detail::io_context::create(void) {
    std::lock_guard<decltype(lock)> l(lock);
    if (cache.empty()) {
        return std::unique_ptr<io_context>(new io_context());
    } else {
        auto retval = std::move(cache.back());
        retval->response.clear();
        cache.pop_back();
        return retval;
    }
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
    const auto offset = that->request_size- that->request_remaining;
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
    curl(nullptr, &::curl_multi_cleanup),
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
