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
        typedef less_ci<ascii_type> compare_type;
        typedef std::basic_string<char_type> string_type;
        typedef std::multimap<ascii_type, ascii_type, compare_type> value_type;

        /// <summary>
        /// The line breaks used in HTTP.
        /// </summary>
        static const ascii_type line_break;

        /// <summary>
        /// Adds the given header to the collection.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <exception cref="system_error">If the header name or value contains
        /// any non-ASCII characters.</exception>
        void add(_In_ const string_type& name, _In_ const string_type& value);

        /// <summary>
        /// Clears all headers.
        /// </summary>
        void clear(void);

        /// <summary>
        /// Removes all headers with the specified name.
        /// </summary>
        /// <param name="name"></param>
        /// <exception cref="system_error">If the name contains any non-ASCII
        /// characters.</exception>
        void remove(_In_ const string_type& name);

        /// <summary>
        /// Answer the number of bytes used to format all headers.
        /// </summary>
        /// <returns></returns>
        std::size_t size(void) const noexcept;

        /// <summary>
        /// Writes the headers to the specified buffer.
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

        static const ascii_type delimiter;

        value_type _values;
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
