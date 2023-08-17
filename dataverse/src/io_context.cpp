// <copyright file="io_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "io_context.h"

#include "dataverse/convert.h"


#if 0
/*
 * visus::dataverse::detail::io_context::io_context
 */
visus::dataverse::detail::io_context::io_context(
        _In_ const std::size_t size) noexcept
    : client_data(nullptr), on_failed(nullptr),
        operation(io_operation::unknown), size(size) {
#if defined(_WIN32)
    this->buffer.buf = reinterpret_cast<CHAR *>(this->payload());
    this->buffer.len = static_cast<ULONG>(this->size);
#endif /* defined(_WIN32) */
    std::memset(&this->overlapped, 0, sizeof(this->overlapped));
}


/*
 * visus::dataverse::detail::io_context::~io_context
 */
visus::dataverse::detail::io_context::~io_context(void) noexcept {
    // We need to make sure here that we delete any possible heap allocation for
    // std::functions.
    this->operation_unknown();
}


/*
 * visus::dataverse::detail::io_context::invoke_on_failed
 */
void visus::dataverse::detail::io_context::invoke_on_failed(
        _In_ dataverse_connection *connection,
        _In_ const std::system_error& ex) {
    if (this->on_failed != nullptr) {
        this->on_failed(connection, ex, this);
    }
}


/*
 * visus::dataverse::detail::io_context::invoke_on_received
 */
std::size_t visus::dataverse::detail::io_context::invoke_on_received(
        _In_ dataverse_connection *connection,
        _In_ const std::size_t cnt) {
    if ((this->operation == io_operation::receive) && this->on_received) {
        return this->on_received(connection, cnt, this);
    } else {
        return 0;
    }
}


/*
 * visus::dataverse::detail::io_context::invoke_on_sent
 */
void visus::dataverse::detail::io_context::invoke_on_sent(
        _In_ dataverse_connection *connection) {
    if ((this->operation == io_operation::send) && this->on_sent) {
        this->on_sent(connection, this);
    }
}


/*
 * visus::dataverse::detail::io_context::operation_receive
 */
void visus::dataverse::detail::io_context::operation_receive(
        _In_ const decltype(io_context::on_received)& on_received) {
    this->operation_unknown();
    new (&this->on_received) decltype(io_context::on_received)(on_received);
    this->operation = io_operation::receive;
}


/*
 * visus::dataverse::detail::io_context::operation_send
 */
void visus::dataverse::detail::io_context::operation_send(
        _In_ const decltype(io_context::on_sent)& on_sent) {
    this->operation_unknown();
    new (&this->on_sent) decltype(io_context::on_sent)(on_sent);
    this->operation = io_operation::send;
}


/*
 * visus::dataverse::detail::io_context::operation_unknown
 */
void visus::dataverse::detail::io_context::operation_unknown(void) noexcept {
#define _DESTRUCT_CB(cb) this->cb.~decltype(this->cb)()

    switch (this->operation) {
        case io_operation::receive:
            _DESTRUCT_CB(on_received);
            break;

        case io_operation::send:
            _DESTRUCT_CB(on_sent);
            break;

        case io_operation::unknown:
            // Nothing to do.
            break;
    }

    this->operation = io_operation::unknown;
#undef _DESTRUCT_CB
}
#endif


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
        cache.pop_back();
        return retval;
    }
}


/*
 * visus::dataverse::detail::io_context::recycle
 */
void visus::dataverse::detail::io_context::recycle(
        _Inout_ std::unique_ptr<io_context>&& context) {
    if (context != nullptr) {
        std::lock_guard<decltype(lock)> l(lock);
        cache.push_back(std::move(context));
    }
}


/*
 * visus::dataverse::detail::io_context::write_response
 */
std::size_t CALLBACK visus::dataverse::detail::io_context::write_response(
        _In_reads_bytes_(cnt *element_size) const void *data,
        _In_ const std::size_t cnt,
        _In_ const std::size_t size,
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
    headers(nullptr, &::curl_slist_free_all),
    form(nullptr, &curl_mime_free) { }


/*
 * visus::dataverse::detail::io_context::cache
 */
std::vector<std::unique_ptr<visus::dataverse::detail::io_context>>
visus::dataverse::detail::io_context::cache;


/*
 * visus::dataverse::detail::io_context::lock
 */
std::mutex visus::dataverse::detail::io_context::lock;
