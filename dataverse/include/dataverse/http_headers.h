// <copyright file="http_headers.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "dataverse/api.h"
#include "dataverse/types.h"

#include <string>


namespace visus {
namespace dataverse {

    /* Forward declarations. */
    namespace detail { struct http_request_headers; }
    namespace detail { struct http_response_headers; }


    /// <summary>
    /// Container for the HTTP headers, which must be 7-bit ASCII.
    /// </summary>
    class DATAVERSE_API http_headers final {

    public:

        /// <summary>
        /// The type of an ASCII character.
        /// </summary>
        typedef char ascii_type;

        /// <summary>
        /// The type to represent a single byte.
        /// </summary>
        typedef std::uint8_t byte_type;

        /// <summary>
        /// Appends, if there is sufficient space, <paramref name="dst" /> to
        /// <paramref name="str" /> and returns the pointer past the appended
        /// text in <paramref name="dst" />.
        /// </summary>
        /// <param name="dst"></param>
        /// <param name="end"></param>
        /// <param name="str"></param>
        /// <returns></returns>
        _Ret_maybenull_ static byte_type *write(
            _Out_writes_opt_(end - dst) byte_type *dst,
            _In_ const byte_type *end,
            _In_z_ const ascii_type *str);

        /// <summary>
        /// Appends, if there is sufficient space, <paramref name="dst" /> to
        /// <paramref name="str" /> and returns the pointer past the appended
        /// text in <paramref name="dst" />.
        /// </summary>
        /// <param name="dst"></param>
        /// <param name="end"></param>
        /// <param name="str"></param>
        /// <returns></returns>
        inline _Ret_maybenull_ static byte_type *write(
                _Out_writes_opt_(end - dst) byte_type *dst,
                _In_ const byte_type *end,
                _In_z_ const std::basic_string<ascii_type>& str) {
            return write(dst, end, str.c_str());
        }

        /// <summary>
        /// Appends, if there is sufficient space, <paramref name="headers" />
        /// to <paramref name="str" /> and returns the pointer past the appended
        /// text in <paramref name="dst" />.
        /// </summary>
        /// <param name="dst"></param>
        /// <param name="end"></param>
        /// <param name="headers"></param>
        /// <returns></returns>
        _Ret_maybenull_ static byte_type *write(
            _Out_writes_opt_(end - dst) byte_type *dst,
            _In_ const byte_type *end,
            _In_ const http_headers& headers);

        /// <summary>
        /// The delimiter between the name of a header and its value.
        /// </summary>
        static constexpr const ascii_type *delimiter = ":";

        /// <summary>
        /// The line breaks used in HTTP.
        /// </summary>
        static constexpr const ascii_type *line_break = "\r\n";

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        http_headers(void);

        /// <summary>
        /// Initialise from move.
        /// </summary>
        /// <param name="rhs">The object to be moved.</param>
        http_headers(_Inout_ http_headers&& rhs) noexcept;

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~http_headers(void) noexcept;

        /// <summary>
        /// Adds the given header to the collection.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="system_error">If the header name or value contains
        /// any non-ASCII characters.</exception>
        http_headers& add(_In_z_ const char_type *name,
            _In_z_ const char_type *value);

        /// <summary>
        /// Clears all headers.
        /// </summary>
        /// <returns><c>*this</c>.</returns>
        http_headers& clear(void);

        /// <summary>
        /// Removes all headers with the specified name.
        /// </summary>
        /// <param name="name"></param>
        /// <exception cref="system_error">If the name contains any non-ASCII
        /// characters.</exception>
        /// <returns><c>*this</c>.</returns>
        http_headers& remove(_In_z_ const char_type *name);

        /// <summary>
        /// Answer the number of bytes used to format all headers.
        /// </summary>
        /// <returns></returns>
        std::size_t size(void) const noexcept;

        /// <summary>
        /// Move assignment.
        /// </summary>
        /// <param name="rhs">The right-hand side operand.</param>
        /// <returns><c>*this</c>.</returns>
        http_headers& operator =(_Inout_ http_headers&& rhs) noexcept;

    private:

        detail::http_request_headers *_request;
        detail::http_response_headers *_response;
    };

} /* namespace dataverse */
} /* namespace visus */
