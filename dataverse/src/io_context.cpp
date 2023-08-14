// <copyright file="io_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "io_context.h"

#include "dataverse/convert.h"



/*
 * visus::dataverse::detail::io_context::operator new
 */
_Ret_ void *visus::dataverse::detail::io_context::operator new(
        _In_ const std::size_t header_size,
        _In_ const std::size_t body_size) {
    return ::operator new(header_size + body_size);
}


/*
 * visus::dataverse::detail::io_context::operator delete
 */
void visus::dataverse::detail::io_context::operator delete(_In_opt_ void *ptr) {
    ::operator delete(ptr);
}


/*
 * visus::dataverse::detail::io_context::io_context
 */
visus::dataverse::detail::io_context::io_context(
        _In_ const std::size_t size) noexcept
    : on_failed(nullptr), operation(io_operation::unknown), size(size),
        user_data(nullptr) {
#if defined(_WIN32)
    this->buffer.buf = reinterpret_cast<CHAR *>(this->payload());
    this->buffer.len = static_cast<ULONG>(this->size);
#endif /* defined(_WIN32) */
    this->on_succeded.received = nullptr;
    std::memset(&this->overlapped, 0, sizeof(this->overlapped));
}


/*
 * visus::dataverse::detail::io_context::invoke_on_disconnected
 */
void visus::dataverse::detail::io_context::invoke_on_disconnected(
        _In_ dataverse_connection *connection) {
    if (this->on_disconnected != nullptr) {
        this->on_disconnected(connection, this->user_data);
    }
}


/*
 * visus::dataverse::detail::io_context::invoke_on_failed
 */
void visus::dataverse::detail::io_context::invoke_on_failed(
        _In_ dataverse_connection *connection,
        _In_ const std::system_error& ex) {
    if (this->on_failed != nullptr) {
        this->on_failed(connection, ex, this->user_data);
    }
}


/*
 * visus::dataverse::detail::io_context::invoke_on_received
 */
void visus::dataverse::detail::io_context::invoke_on_received(
        _In_ dataverse_connection *connection,
        _In_ const std::size_t cnt) {
    if (this->on_succeded.received) {
        this->on_succeded.received(connection, this->payload(), cnt,
            this->user_data);
    }
}


/*
 * visus::dataverse::detail::io_context::invoke_on_sent
 */
void visus::dataverse::detail::io_context::invoke_on_sent(
        _In_ dataverse_connection *connection) {
    if (this->on_succeded.sent) {
        this->on_succeded.sent(connection, this->user_data);
    }
}


/*
 * visus::dataverse::detail::io_context::payload
 */
std::uint8_t *visus::dataverse::detail::io_context::payload(void) noexcept {
    return (this->size > 0)
        ? reinterpret_cast<std::uint8_t *>(this + 1)
        : nullptr;
}


/*
 * visus::dataverse::detail::io_context::payload
 */
const std::uint8_t *visus::dataverse::detail::io_context::payload(
        void) const noexcept {
    return (this->size > 0)
        ? reinterpret_cast<const std::uint8_t *>(this + 1)
        : nullptr;
}
