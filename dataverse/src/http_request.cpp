// <copyright file="http_request.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "http_request.h"


/*
 * visus::dataverse::detail::http_request::delimiter
 */
const visus::dataverse::detail::http_request::ascii_type
visus::dataverse::detail::http_request::delimiter(" ");


/*
 * visus::dataverse::detail::http_request::protocol
 */
const visus::dataverse::detail::http_request::ascii_type
visus::dataverse::detail::http_request::protocol("HTTP/1.1");


/*
 * visus::dataverse::detail::http_request::size
 */
std::size_t visus::dataverse::detail::http_request::size(void) const noexcept {
    std::size_t retval = 0;

    // METHOD path HTTP/1.1\r\n
    retval += this->_method.size() + delimiter.size();
    retval += this->_path.size() + delimiter.size();
    retval += protocol.size() + http_headers::line_break.size();

    // Headers
    retval += this->_headers.size();

    // Body
    if (!this->_body.empty()) {
        retval += this->_body.size();
        retval += http_headers::line_break.size();
    }

    return retval;
}


/*
 * visus::dataverse::detail::http_request::write
 */
_Ret_maybenull_ char *visus::dataverse::detail::http_request::write(
        _Out_writes_bytes_(cnt) char *dst, _In_ const char *end) const {
    if ((dst == nullptr) || (end <= dst)) {
        return nullptr;
    }

    auto retval = dst;

    if ((retval = write(retval, end, this->_method + delimiter)) == nullptr) {
        return nullptr;
    }

    if ((retval = write(retval, end, this->_path + delimiter)) == nullptr) {
        return nullptr;
    }

    if ((retval = write(retval, end, protocol + http_headers::line_break))
            == nullptr) {
        return nullptr;
    }

    if ((retval = this->_headers.write(retval, end)) == nullptr) {
        return nullptr;
    }

    if (!this->_body.empty()) {

    }

    return retval;
}


/*
 * visus::dataverse::detail::http_request::write
 */
_Ret_maybenull_ char *visus::dataverse::detail::http_request::write(
        _Out_writes_bytes_(cnt) char *dst,
        _In_ const char *end,
        _In_ const ascii_type& str) {
    if (dst == nullptr) {
        return nullptr;
    }

    if (dst + str.size() >= end) {
        return nullptr;
    }

    return ::strncat(dst, str.c_str(), str.size());
}
