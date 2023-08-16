// <copyright file="http_request.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "http_request.h"

#include <stdexcept>


/*
 * visus::dataverse::detail::http_request::write
 */
_Ret_maybenull_ visus::dataverse::detail::http_request::byte_type *
visus::dataverse::detail::http_request::write(
        _Out_writes_bytes_(end - dst) byte_type *dst,
        _In_ const byte_type *end,
        _In_ const http_request& request) {
    if ((dst == nullptr) || (end <= dst)) {
        return nullptr;
    }

    auto retval = dst;

    retval = http_headers::write(retval, end, (request._method + space));
    retval = http_headers::write(retval, end, (request._path + space));
    retval = http_headers::write( retval, end, (request._protocol
        + http_headers::line_break));
    retval = http_headers::write(retval, end, request._headers);

    if (!request._body.empty()) {
        if (retval + request._body.size() > end) {
            return nullptr;
        }

        retval = std::copy(request._body.begin(), request._body.end(), retval);
    }

    retval = http_headers::write(retval, end, http_headers::line_break);

    return retval;
}


/*
 * visus::dataverse::detail::http_request::as_octets
 */
std::vector<visus::dataverse::detail::http_request::byte_type>
visus::dataverse::detail::http_request::as_octets(void) const {
    std::vector<byte_type> retval(this->size());

    if (write(retval.data(), retval.data() + retval.size(), *this) == nullptr) {
        throw std::logic_error("This should never happen unless the HTTP "
            "request is manipulated concurrently, which is not supported.");
    }

    return retval;
}


/*
 * visus::dataverse::detail::http_request::size
 */
std::size_t visus::dataverse::detail::http_request::size(void) const noexcept {
    std::size_t retval = 0;

    // METHOD path HTTP/1.1\r\n
    retval += this->_method.size() + space.size();
    retval += this->_path.size() + space.size();
    retval += this->_protocol.size() + ::strlen(http_headers::line_break);

    // Headers
    retval += this->_headers.size();

    // Body
    if (!this->_body.empty()) {
        retval += this->_body.size();
        retval += ::strlen(http_headers::line_break);
    }

    // Empty line at the end.
    retval += ::strlen(http_headers::line_break);

    return retval;
}


/*
 * visus::dataverse::detail::http_request::space
 */
const visus::dataverse::detail::http_request::ascii_type
visus::dataverse::detail::http_request::space(" ");
