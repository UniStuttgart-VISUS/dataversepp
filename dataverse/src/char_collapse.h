// <copyright file="char_collapse.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <cinttypes>


namespace visus {
namespace dataverse {
namespace detail {

    template<class TChar> struct char_collapse final { };

#if (defined(__cplusplus) && (__cplusplus >= 202000))
    template<> struct char_collapse<char8_t> final {
        typedef char type;
    };
#endif /* (defined(__cplusplus) && (__cplusplus >= 202000)) */

#if defined(_WIN32)
    template<> struct char_collapse<char16_t> final {
        typedef wchar_t type;
    };
#endif /* defined(_WIN32) */

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
