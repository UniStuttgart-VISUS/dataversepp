// <copyright file="http_response.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/http_response.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <stdexcept>


/*
 * visus::dataverse::http_response::http_response
 */
visus::dataverse::http_response::http_response(
        _In_reads_bytes_opt_(size) const byte_type *data,
        _In_ const std::size_t size) : _data(nullptr), _size(size) {
    if (data != nullptr) {
        this->_data = new byte_type[this->_size];
        std::copy(data, data + size, this->_data);
    }
}


/*
 * visus::dataverse::http_response::http_response
 */
visus::dataverse::http_response::http_response(
        _Inout_ http_response&& rhs) noexcept
    : _data(rhs._data), _headers(std::move(rhs._headers)),
        _size(rhs._size) {
    rhs._data = nullptr;
    rhs._size = 0;
}


/*
 * visus::dataverse::http_response::~http_response
 */
visus::dataverse::http_response::~http_response(void) noexcept {
    delete[] this->_data;
}


/*
 * visus::dataverse::http_response::append
 */
visus::dataverse::http_response& visus::dataverse::http_response::append(
        _In_reads_bytes_(size) const byte_type *data,
        _In_ const std::size_t size) {
    if ((data != nullptr) && (size > 0)) {
        assert((this->_data != nullptr) || (this->_size == 0));
        const auto old = this->_data;

        this->_data = new byte_type[this->_size + size];

        if (old != nullptr) {
            std::copy(old, old + this->_size, this->_data);
        }

        std::copy(data, data + size, this->_data + this->_size);
        this->_size += size;

        delete[] old;
    }

    return *this;
}


/*
 * visus::dataverse::http_response::operator =
 */
visus::dataverse::http_response& visus::dataverse::http_response::operator =(
        _Inout_ http_response&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        delete[] this->_data;
        this->_data = rhs._data;
        rhs._data = nullptr;

        this->_headers = std::move(rhs._headers);

        this->_size = rhs._size;
        rhs._size = 0;
    }

    return *this;
}
