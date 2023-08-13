// <copyright file="http_request.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <vector>

#include "dataverse/convert.h"
#include "dataverse/types.h"

#include "http_headers.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// Representation of an HTTP request.
    /// </summary>
    class http_request final {

    public:

        typedef http_headers::ascii_type ascii_type;
        typedef std::uint8_t byte_type;
        typedef http_headers::string_type string_type;

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
        inline http_request& headers(_In_ const http_headers& headers) {
            this->_headers = headers;
            return *this;
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
        inline http_request&path(_In_ const string_type& path) {
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
        /// Answer the overall size of the request in bytes.
        /// </summary>
        /// <returns></returns>
        std::size_t size(void) const noexcept;

        /// <summary>
        /// Writes the request to the specified buffer.
        /// </summary>
        /// <param name="dst">The pointer to the begin oft he range that can be
        /// written.</param>
        /// <param name="end">A pointer past the last element that can be
        /// written.</param>
        /// <returns>A pointer past the last header in <paramref name="dst" />
        /// or <c>nullptr</c> if not all headers could be written.</returns>
        _Ret_maybenull_ char *write(_Out_writes_bytes_(cnt) char *dst,
            _In_ const char *end) const;

    private:

        _Ret_maybenull_ static char *write(_Out_writes_bytes_(cnt) char *dst,
            _In_ const char *end, _In_ const ascii_type& str);

        static const ascii_type delimiter;
        static const ascii_type protocol;

        std::vector<byte_type> _body;
        http_headers _headers;
        ascii_type _method;
        ascii_type _path;
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
