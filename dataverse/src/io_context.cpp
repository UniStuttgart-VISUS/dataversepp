// <copyright file="io_context.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "io_context.h"

#include <cstring>


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
visus::dataverse::detail::io_context::io_context(void) noexcept
        : connection(nullptr), on_failed(nullptr),
        operation(io_operation::unknown), size(0), user_data(nullptr) {
    this->buffer.buf = nullptr;
    this->buffer.len = 0;
    this->on_succeded.received = nullptr;
    std::memset(&this->overlapped, 0, sizeof(this->overlapped));
}


/*
 * visus::dataverse::detail::io_context::invoke_on_failed
 */
void visus::dataverse::detail::io_context::invoke_on_failed(
        _In_ const system_error_code error) {
    if (this->on_failed != nullptr) {
        this->on_failed(this->connection, error, this->user_data);
    }
}


/*
 * visus::dataverse::detail::io_context::invoke_on_received
 */
void visus::dataverse::detail::io_context::invoke_on_received(
        _In_ const std::size_t cnt) {
    if (this->on_succeded.received) {
        this->on_succeded.received(this->connection, this->payload(), cnt,
            this->user_data);
    }
}


/*
 * visus::dataverse::detail::io_context::invoke_on_sent
 */
void visus::dataverse::detail::io_context::invoke_on_sent(void) {
    if (this->on_succeded.sent) {
        this->on_succeded.sent(this->connection, this->user_data);
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
