// <copyright file="http_headers.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/http_headers.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>

#include "dataverse/convert.h"

#include "http_request_headers.h"
#include "http_response_headers.h"



/*
 * visus::dataverse::http_headers::write
 */
_Ret_maybenull_ visus::dataverse::http_headers::byte_type *
visus::dataverse::http_headers::write(
        _Out_writes_opt_(end - dst) byte_type *dst,
        _In_ const byte_type *end,
        _In_z_ const ascii_type *str) {
    if (dst == nullptr) {
        return nullptr;
    }

    if (str == nullptr) {
        return dst;
    }

    const auto len = ::strlen(str);

    if (dst + len > end) {
        return nullptr;
    }

    return std::copy(str, str + len, dst);
}


/*
 * visus::dataverse::http_headers::write
 */
_Ret_maybenull_ visus::dataverse::http_headers::byte_type *
visus::dataverse::http_headers::write(
        _Out_writes_opt_(end - dst) byte_type *dst,
        _In_ const byte_type *end,
        _In_ const http_headers& headers) {
    if ((dst == nullptr) || (end <= dst)) {
        return nullptr;
    }

    const auto len_delimiter = ::strlen(delimiter);
    const auto len_line_break = ::strlen(line_break);
    auto retval = dst;

    if (headers._request != nullptr) {
        for (auto &v : headers._request->values) {
            const auto required = v.first.size()
                + len_delimiter
                + v.second.size()
                + len_line_break;

            if (retval + required >= end) {
                return nullptr;
            }

            retval = write(retval, end, v.first.c_str());
            retval = write(retval, end, delimiter);
            retval = write(retval, end, v.second.c_str());
            retval = write(retval, end, line_break);
        }

    } else {
        throw "TODO";
    }

    return retval;
}


/*
 * visus::dataverse::http_headers::http_headers
 */
visus::dataverse::http_headers::http_headers(void)
    : _request(new detail::http_request_headers()),
    _response(nullptr) { }


/*
 * visus::dataverse::http_headers::http_headers
 */
visus::dataverse::http_headers::http_headers(
        _Inout_ http_headers&& rhs) noexcept
        : _request(rhs._request), _response(rhs._response) {
    rhs._request = nullptr;
    rhs._response = nullptr;
}


/*
 * visus::dataverse::http_headers::~http_headers
 */
visus::dataverse::http_headers::~http_headers(void) noexcept {
    delete this->_request;
    delete this->_response;
}


/*
 * visus::dataverse::http_headers::delimiter
 */
constexpr const visus::dataverse::http_headers::ascii_type *
visus::dataverse::http_headers::delimiter;


/*
 * visus::dataverse::http_headers::line_break
 */
constexpr const visus::dataverse::http_headers::ascii_type *
visus::dataverse::http_headers::line_break;


/*
 * visus::dataverse::http_headers::add
 */
visus::dataverse::http_headers&
visus::dataverse::http_headers::add(_In_z_ const char_type *name,
        _In_z_ const char_type *value) {
    if (this->_request != nullptr) {
        this->_request->values.insert(std::make_pair(
            to_ascii(name), to_ascii(value)));
    } else {
        std::logic_error("A response header cannot be modified.");
    }

    return *this;
}


/*
 * visus::dataverse::http_headers::clear
 */
visus::dataverse::http_headers&
visus::dataverse::http_headers::clear(void) {
    if (this->_request != nullptr) {
        this->_request->values.clear();
    } else {
        std::logic_error("A response header cannot be modified.");
    }

    return *this;
}


/*
 * visus::dataverse::http_headers::remove
 */
visus::dataverse::http_headers&
visus::dataverse::http_headers::remove(_In_z_ const char_type *name) {
    if (this->_request != nullptr) {
        this->_request->values.erase(to_ascii(name));
    } else {
        std::logic_error("A response header cannot be modified.");
    }

    return *this;
}


/*
 * visus::dataverse::http_headers::size
 */
std::size_t visus::dataverse::http_headers::size(void) const noexcept {
    std::size_t retval = 0;

    if (this->_request != nullptr) {
        for (auto &v : this->_request->values) {
            retval += v.first.size();
            retval += ::strlen(delimiter);
            retval += v.second.size();
        }

        retval += this->_request->values.size() * ::strlen(line_break);

    } else {
        throw "TODO";
    }

    return retval;
}


/*
 * visus::dataverse::http_headers::operator =
 */
visus::dataverse::http_headers& visus::dataverse::http_headers::operator =(
        _Inout_ http_headers&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        delete this->_request;
        this->_request = rhs._request;
        rhs._request = nullptr;

        delete this->_response;
        this->_response = rhs._response;
        rhs._response = nullptr;
    }

    return *this;
}
