// <copyright file="http_headers.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "http_headers.h"

#include "dataverse/convert.h"


/*
 * visus::dataverse::detail::http_headers::line_break
 */
const visus::dataverse::detail::http_headers::ascii_type
visus::dataverse::detail::http_headers::line_break("\r\n");


/*
 * visus::dataverse::detail::http_headers::add
 */
void visus::dataverse::detail::http_headers::add(_In_ const string_type& name,
        _In_ const string_type& value) {
    this->_values.insert(std::make_pair(to_ascii(name), to_ascii(value)));
}


/*
 * visus::dataverse::detail::http_headers::clear
 */
void visus::dataverse::detail::http_headers::clear(void) {
    this->_values.clear();
}


/*
 * visus::dataverse::detail::http_headers::remove
 */
void visus::dataverse::detail::http_headers::remove(
        _In_ const string_type& name) {
    this->_values.erase(to_ascii(name));
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
 * visus::dataverse::detail::http_headers::write
 */
_Ret_maybenull_ char *visus::dataverse::detail::http_headers::write(
        _Out_writes_bytes_(cnt) char *dst, _In_ const char *end) const {
    if ((dst == nullptr) || (end <= dst)) {
        return nullptr;
    }

    auto retval = dst;

    for (auto& v : this->_values) {
        const auto required = v.first.size() + delimiter.size()
            + v.second.size() + line_break.size();
        if (retval + required >= end) {
            return nullptr;
        }

        retval = ::strncat(retval, v.first.c_str(), v.first.size());
        retval = ::strncat(retval, delimiter.c_str(), delimiter.size());
        retval = ::strncat(retval, v.second.c_str(), v.second.size());
        retval = ::strncat(retval, line_break.c_str(), line_break.size());
    }

    return retval;
}


/*
 * visus::dataverse::detail::http_headers::delimiter
 */
const visus::dataverse::detail::http_headers::ascii_type
visus::dataverse::detail::http_headers::delimiter(": ");
