// <copyright file="http_request.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <vector>

#include "dataverse/convert.h"
#include "dataverse/http_headers.h"
#include "dataverse/types.h"



namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// Representation of an HTTP request.
    /// </summary>
    class http_request final {

    public:

        typedef std::basic_string<http_headers::ascii_type> ascii_type;
        typedef http_headers::byte_type byte_type;
        typedef std::basic_string<char_type> string_type;


        /// <summary>
        /// Writes the request to the specified buffer.
        /// </summary>
        /// <param name="dst">The pointer to the begin oft he range that can be
        /// written.</param>
        /// <param name="end">A pointer past the last element that can be
        /// written.</param>
        /// <param name="request">The request to be written.</param>
        /// <returns>A pointer past the last header in <paramref name="dst" />
        /// or <c>nullptr</c> if not all headers could be written.</returns>
        static _Ret_maybenull_ byte_type *write(
            _Out_writes_bytes_(end - dst) byte_type *dst,
            _In_ const byte_type *end,
            _In_ const http_request& request);

        /// <summary>
        /// Convert the request into a byte array.
        /// </summary>
        /// <returns></returns>
        std::vector<byte_type> as_octets(void) const;

        /// <summary>
        /// Gets the request body.
        /// </summary>
        /// <returns></returns>
        inline const std::vector<byte_type>& body(void) const noexcept {
            return this->_body;
        }

        /// <summary>
        /// Sets the request body.
        /// </summary>
        /// <param name="body"></param>
        /// <returns></returns>
        inline http_request& body(_In_ const std::vector<byte_type>& body) {
            this->_body = body;
            return *this;
        }

        /// <summary>
        /// Sets the request body.
        /// </summary>
        /// <param name="body"></param>
        /// <returns></returns>
        inline http_request& body(
                _Inout_ std::vector<byte_type>&& body) noexcept {
            this->_body = std::move(body);
            return *this;
        }

        /// <summary>
        /// Gets the HTTP headers of the request.
        /// </summary>
        /// <returns>The header collection of the request.</returns>
        inline const http_headers& headers(void) const noexcept {
            return this->_headers;
        }

        /// <summary>
        /// Gets the HTTP headers of the request.
        /// </summary>
        /// <returns>The header collection of the request.</returns>
        inline http_headers& headers(void) noexcept {
            return this->_headers;
        }

        /// <summary>
        /// Sets the HTTP headers.
        /// </summary>
        /// <param name="headers"></param>
        /// <returns></returns>
        inline http_request& headers(_Inout_ http_headers&& headers) noexcept {
            this->_headers = std::move(headers);
            return *this;
        }

        /// <summary>
        /// Gets the request method.
        /// </summary>
        /// <returns></returns>
        inline const ascii_type& method(void) const noexcept {
            return this->_method;
        }

        /// <summary>
        /// Sets the request method.
        /// </summary>
        /// <param name="method"></param>
        /// <returns></returns>
        inline http_request& method(_In_ const string_type& method) {
            this->_method = to_ascii(method);
            return *this;
        }

        /// <summary>
        /// Sets the request method.
        /// </summary>
        /// <param name="method"></param>
        /// <returns></returns>
        inline http_request& method(_Inout_ ascii_type&& method) noexcept {
            this->_method = std::move(method);
            return *this;
        }

        /// <summary>
        /// Gets the requested path.
        /// </summary>
        /// <returns></returns>
        inline const ascii_type& path(void) const noexcept {
            return this->_path;
        }

        /// <summary>
        /// Sets the requested path.
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        inline http_request& path(_In_ const string_type& path) {
            this->_path = to_ascii(path);
            return *this;
        }

        /// <summary>
        /// Sets the requested path.
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        inline http_request& path(_Inout_ ascii_type&& path) noexcept {
            this->_path = std::move(path);
            return *this;
        }


        /// <summary>
        /// Gets the HTTP version string of the request, if any.
        /// </summary>
        /// <returns></returns>
        inline const ascii_type& protocol(void) const noexcept {
            return this->_protocol;
        }

        /// <summary>
        /// Sets the HTTP version.
        /// </summary>
        /// <param name="protocol"></param>
        /// <returns></returns>
        inline http_request& version(_In_ ascii_type& protocol) {
            this->_protocol = protocol;
            return *this;
        }

        /// <summary>
        /// Sets the HTTP version.
        /// </summary>
        /// <param name="version"></param>
        /// <returns></returns>
        inline http_request& version(_Inout_ ascii_type&& protocol) noexcept {
            this->_protocol = std::move(protocol);
            return *this;
        }

        /// <summary>
        /// Answer the overall size of the request in bytes.
        /// </summary>
        /// <returns></returns>
        std::size_t size(void) const noexcept;

    private:

        static const ascii_type space;

        std::vector<byte_type> _body;
        http_headers _headers;
        ascii_type _method;
        ascii_type _path;
        ascii_type _protocol;
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
