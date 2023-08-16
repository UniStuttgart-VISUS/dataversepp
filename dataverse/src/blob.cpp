// <copyright file="blob.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/blob.h"

#include <cassert>
#include <cstring>
#include <memory>


/*
 * visus::dataverse::blob::blob
 */
visus::dataverse::blob::blob(_In_ const blob& rhs)
        : _data(nullptr), _size(rhs._size) {
    if (rhs._data != nullptr) {
        this->_data = new byte_type[this->_size];
        ::memcpy(this->_data, rhs._data, this->_size);
    }

    assert((this->_data != nullptr) || (this->_size == 0));
}


/*
 * visus::dataverse::blob::blob
 */
visus::dataverse::blob::blob(_Inout_ blob&& rhs) noexcept
        : _data(rhs._data), _size(rhs._size) {
    rhs._data = nullptr;
    rhs._size = 0;
    assert(rhs._data == nullptr);
    assert(rhs._size == 0);
}


/*
 * visus::dataverse::blob::~blob
 */
visus::dataverse::blob::~blob(void) {
    delete[] this->_data;
}


/*
 * visus::dataverse::blob::at
 */
_Ret_maybenull_ void *visus::dataverse::blob::at(
        _In_ const std::size_t offset) noexcept {
    assert((this->_data != nullptr) || (this->_size == 0));
    return (offset < this->_size)
        ? this->_data + offset
        : nullptr;
}


/*
 * visus::dataverse::blob::at
 */
_Ret_maybenull_ const void *visus::dataverse::blob::at(
        _In_ const std::size_t offset) const noexcept {
    assert((this->_data != nullptr) || (this->_size == 0));
    return (offset < this->_size)
        ? this->_data + offset
        : nullptr;
}


/*
 * visus::dataverse::blob::clear
 */
void visus::dataverse::blob::clear(void) {
    delete[] this->_data;
    this->_data = nullptr;
    this->_size = 0;
}


/*
 * visus::dataverse::blob::end
 */
_Ret_maybenull_ visus::dataverse::blob::byte_type *
visus::dataverse::blob::end(void) noexcept {
    return (this->_data != nullptr)
        ? reinterpret_cast<byte_type *>(this->_data) + this->_size
        : nullptr;
}


/*
 * visus::dataverse::blob::end
 */
_Ret_maybenull_ const visus::dataverse::blob::byte_type *
visus::dataverse::blob::end(void) const noexcept {
    return (this->_data != nullptr)
        ? reinterpret_cast<const byte_type *>(this->_data) + this->_size
        : nullptr;
}


/*
 * visus::dataverse::blob::grow
 */
bool visus::dataverse::blob::grow(_In_ const std::size_t size) {
    const auto retval = (size > this->_size);

    if (retval) {
        const auto existing = this->_data;
        this->_data = new byte_type[size];

        if (existing != nullptr) {
            ::memcpy(this->_data, existing, this->_size);
            delete[] existing;
            this->_size = size;
        }
    }

    return retval;
}


/*
 * visus::dataverse::blob::rbegin
 */
_Ret_maybenull_ visus::dataverse::blob::byte_type *
visus::dataverse::blob::rbegin(void) noexcept {
    return ((this->_data != nullptr) && (this->_size > 0))
        ? reinterpret_cast<byte_type *>(this->_data) + this->_size - 1
        : nullptr;
}


/*
 * visus::dataverse::blob::rbegin
 */
_Ret_maybenull_ const visus::dataverse::blob::byte_type *
visus::dataverse::blob::rbegin(void) const noexcept {
    return ((this->_data != nullptr) && (this->_size > 0))
        ? reinterpret_cast<const byte_type *>(this->_data) + this->_size - 1
        : nullptr;
}


/*
 * visus::dataverse::blob::rend
 */
_Ret_maybenull_ visus::dataverse::blob::byte_type *
visus::dataverse::blob::rend(void) noexcept {
    return (this->_data != nullptr)
        ? this->_data - 1
        : nullptr;
}


/*
 * visus::dataverse::blob::rend
 */
_Ret_maybenull_ const visus::dataverse::blob::byte_type *
visus::dataverse::blob::rend(void) const noexcept {
    return (this->_data != nullptr)
        ? this->_data - 1
        : nullptr;
}


/*
 * visus::dataverse::blob::reserve
 */
bool visus::dataverse::blob::reserve(_In_ const std::size_t size) {
    const auto retval = (size > this->_size);

    if (retval) {
        delete[] this->_data;
        this->_size = size;
        this->_data = new byte_type[this->_size];
    }

    return retval;
}


/*
 * visus::dataverse::blob::resize
 */
void visus::dataverse::blob::resize(_In_ const std::size_t size) {
    if (this->_size != size) {
        delete[] this->_data;
        this->_size = size;

        if (this->_size > 0) {
            this->_data = new byte_type[this->_size];
        } else {
            this->_data = nullptr;
        }
    }
}


/*
 * visus::dataverse::blob::truncate
 */
void visus::dataverse::blob::truncate(_In_ const std::size_t size) {
    if (this->_size != size) {
        const auto existing = this->_data;

        if (size > 0) {
            this->_data = new byte_type[size];
        } else {
            this->_data = nullptr;
        }

        if ((this->_data != nullptr) && (existing != nullptr)) {
            ::memcpy(this->_data, existing, (std::min)(this->_size, size));
        }

        delete[] existing;
        this->_size = size;
    }
}


/*
 * visus::dataverse::blob::operator =
 */
visus::dataverse::blob& visus::dataverse::blob::operator =(
        _In_ const blob& rhs) {
    if (this != std::addressof(rhs)) {
        if ((this->_data != nullptr) && (this->_size != rhs._size)) {
            delete[] this->_data;
            this->_data = nullptr;
        }

        this->_size = rhs._size;
        if ((this->_data == nullptr) && (rhs._data != nullptr)) {
            this->_data = new byte_type[this->_size];
        }

        if (this->_data != nullptr) {
            assert(rhs._data != nullptr);
            ::memcpy(this->_data, rhs._data, this->_size);
        }
    }

    assert((this->_data != nullptr) || (rhs._data == nullptr));
    assert((this->_data != nullptr) || (this->_size == 0));
    assert(this->_size == rhs._size);
    return *this;
}


/*
 * visus::dataverse::blob::operator =
 */
visus::dataverse::blob& visus::dataverse::blob::operator =(
        _Inout_ blob&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        this->_data = rhs._data;
        rhs._data = nullptr;
        this->_size = rhs._size;
        rhs._size = 0;
    }

    assert(rhs._data == nullptr);
    assert(rhs._size == 0);
    return *this;
}