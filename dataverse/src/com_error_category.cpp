// <copyright file="com_error_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "com_error_category.h"

#if defined(_WIN32)
#include <comdef.h>

#include "dataverse/convert.h"


/*
 * visus::dataverse::detail::com_error_category::default_error_condition
 */
std::error_condition
visus::dataverse::detail::com_error_category::default_error_condition(
        _In_ int error) const noexcept {
    auto code = HRESULT_CODE(error);

    if (code || (error == ERROR_SUCCESS)) {
        // 'hr' is a default Windows code mapped to HRESULT.
        return std::system_category().default_error_condition(error);
    } else {
        // This is an actual COM error.
        return std::error_condition(error, com_category());
    }
}


/*
 * visus::dataverse::detail::com_error_category::message
 */
std::string visus::dataverse::detail::com_error_category::message(
        _In_ int error) const {
    _com_error e(error);

#if (defined(UNICODE) || defined(_UNICODE))
    return convert<char>(reinterpret_cast<const char16_t *>(e.ErrorMessage()),
        0, CP_OEMCP);
#else /* (defined(UNICODE) || defined(_UNICODE)) */
    return e.ErrorMessage();
#endif /* (defined(UNICODE) || defined(_UNICODE)) */
}


/*
 * visus::dataverse::detail::com_category
 */
_Ret_valid_ const std::error_category&
visus::dataverse::detail::com_category(void) noexcept {
    static const com_error_category retval;
    return retval;
}
#endif /* _WIN32 */
