// <copyright file="http_headers.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "http_headers.h"

#include <cassert>

#include "dataverse/convert.h"


/*
 * visus::dataverse::detail::http_headers::line_break
 */
const visus::dataverse::detail::http_headers::ascii_type
visus::dataverse::detail::http_headers::line_break("\r\n");


/*
 * visus::dataverse::detail::http_headers::write
 */
_Ret_maybenull_ visus::dataverse::detail::http_headers::byte_type *
visus::dataverse::detail::http_headers::write(
        _Out_writes_opt_(end - dst) byte_type *dst,
        _In_ const byte_type *end,
        _In_ const ascii_type& str) {
    if (dst == nullptr) {
        return nullptr;
    }

    if (dst + str.size() > end) {
        return nullptr;
    }

    return std::copy(str.begin(), str.end(), dst);
}


/*
 * visus::dataverse::detail::http_headers::write
 */
_Ret_maybenull_ visus::dataverse::detail::http_headers::byte_type *
visus::dataverse::detail::http_headers::write(
        _Out_writes_opt_(end - dst) byte_type *dst,
        _In_ const byte_type *end,
        _In_ const http_headers& headers) {
    if ((dst == nullptr) || (end <= dst)) {
        return nullptr;
    }

    auto retval = dst;

    for (auto &v : headers._values) {
        const auto required = v.first.size()
            + delimiter.size()
            + v.second.size()
            + line_break.size();
        if (retval + required >= end) {
            return nullptr;
        }

        retval = write(retval, end, v.first);
        retval = write(retval, end, delimiter);
        retval = write(retval, end, v.second);
        retval = write(retval, end, line_break);
    }

    return retval;
}


/*
 * visus::dataverse::detail::http_headers::add
 */
visus::dataverse::detail::http_headers&
visus::dataverse::detail::http_headers::add(_In_ const string_type& name,
        _In_ const string_type& value) {
    this->_values.insert(std::make_pair(to_ascii(name), to_ascii(value)));
    return *this;
}


/*
 * visus::dataverse::detail::http_headers::clear
 */
visus::dataverse::detail::http_headers&
visus::dataverse::detail::http_headers::clear(void) {
    this->_values.clear();
    return *this;
}


/*
 * visus::dataverse::detail::http_headers::remove
 */
visus::dataverse::detail::http_headers&
visus::dataverse::detail::http_headers::remove(_In_ const string_type& name) {
    this->_values.erase(to_ascii(name));
    return *this;
}


/*
 * visus::dataverse::detail::http_headers::size
 */
std::size_t visus::dataverse::detail::http_headers::size(void) const noexcept {
    std::size_t retval = 0;

    for (auto& v : this->_values) {
        retval += v.first.size();
        retval += delimiter.size();
        retval += v.second.size();
    }

    retval += this->_values.size() * line_break.size();

    return retval;
}


/*
 * visus::dataverse::detail::http_headers::delimiter
 */
const visus::dataverse::detail::http_headers::ascii_type
visus::dataverse::detail::http_headers::delimiter(": ");
