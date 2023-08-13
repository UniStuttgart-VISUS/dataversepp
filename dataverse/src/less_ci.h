// <copyright file="less_ci.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cstring>
#include <string>


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// A case-insensitive less operator.
    /// </summary>
    /// <typeparam name="TType">The type the operator is for.</typeparam>
    template<class TType>
    struct less_ci { };


    /// <summary>
    /// Specialisation for <c>char</c>.
    /// </summary>
    /// <typeparam name="TTraits"></typeparam>
    /// <typeparam name="TAlloc"></typeparam>
    template<class TTraits, class TAlloc>
    struct less_ci<std::basic_string<char, TTraits, TAlloc>> {
        typedef std::basic_string<char, TTraits, TAlloc> value_type;

        inline constexpr bool operator ()(const value_type& lhs,
                const value_type& rhs) const {
            return (::stricmp(lhs.c_str(), rhs.c_str()) < 0);
        }
    };


    /// <summary>
    /// Specialisation for <c>wchar_t</c>.
    /// </summary>
    /// <typeparam name="TTraits"></typeparam>
    /// <typeparam name="TAlloc"></typeparam>
    template<class TTraits, class TAlloc>
    struct less_ci<std::basic_string<wchar_t, TTraits, TAlloc>> {
        typedef std::basic_string<wchar_t, TTraits, TAlloc> value_type;

        inline constexpr bool operator ()(const value_type& lhs,
                const value_type& rhs) const {
            return (::wcsicmp(lhs.c_str(), rhs.c_str()) < 0);
        }
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
