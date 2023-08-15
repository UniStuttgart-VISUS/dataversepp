// <copyright file="http_headers.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <map>
#include <string>

#include "dataverse/api.h"
#include "dataverse/types.h"

#include "less_ci.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// Container for the HTTP headers, which must be 7-bit ASCII.
    /// </summary>
    class http_headers final {

    public:

        typedef std::string ascii_type;
        typedef std::uint8_t byte_type;
        typedef less_ci<ascii_type> compare_type;
        typedef std::basic_string<char_type> string_type;
        typedef std::multimap<ascii_type, ascii_type, compare_type> value_type;

        /// <summary>
        /// Appends, if there is sufficient space, <paramref name="dst" /> to
        /// <paramref name="str" /> and returns the pointer past the appended
        /// text in <paramref name="dst" />.
        /// </summary>
        _Ret_maybenull_ static byte_type *write(
            _Out_writes_opt_(end - dst) byte_type *dst,
            _In_ const byte_type *end,
            _In_ const ascii_type& str);

        /// <summary>
        /// Appends, if there is sufficient space, <paramref name="headers" />
        /// to <paramref name="str" /> and returns the pointer past the appended
        /// text in <paramref name="dst" />.
        /// </summary>
        _Ret_maybenull_ static byte_type *write(
            _Out_writes_opt_(end - dst) byte_type *dst,
            _In_ const byte_type *end,
            _In_ const http_headers& headers);

        /// <summary>
        /// The line breaks used in HTTP.
        /// </summary>
        static const ascii_type line_break;

        /// <summary>
        /// Adds the given header to the collection.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="system_error">If the header name or value contains
        /// any non-ASCII characters.</exception>
        http_headers& add(_In_ const string_type& name,
            _In_ const string_type& value);

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
        http_headers& remove(_In_ const string_type& name);

        /// <summary>
        /// Answer the number of bytes used to format all headers.
        /// </summary>
        /// <returns></returns>
        std::size_t size(void) const noexcept;

    private:

        static const ascii_type delimiter;

        value_type _values;
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
