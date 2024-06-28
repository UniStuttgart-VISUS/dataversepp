// <copyright file="icu_error_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "icu_error_category.h"

#include <string>

#if !defined(_WIN32)
#include <unicode/errorcode.h>
#include <unicode/umsg.h>


/*
 * visus::dataverse::detail::icu_error_category::default_error_condition
 */
std::error_condition
visus::dataverse::detail::icu_error_category::default_error_condition(
        _In_ int error) const noexcept {
    const auto code = static_cast<UErrorCode>(error);
    return std::error_condition(code, icu_category());
}


/*
 * visus::dataverse::detail::icu_error_category::message
 */
std::string visus::dataverse::detail::icu_error_category::message(
        _In_ int error) const {
    const auto code = static_cast<UErrorCode>(error);
    return std::to_string(error);
//    const auto error_name = ::u_errorName(code);
//
//    if (error_name == nullptr) {
//        return std::to_string(error);
//    }
//    
////    (UChar *)ures_getStringByKey(gBundle, errname, &msgLen, &subErr);
}


/*
 * visus::dataverse::detail::icu_category
 */
_Ret_valid_ const std::error_category&
visus::dataverse::detail::icu_category(void) noexcept {
    static const icu_error_category retval;
    return retval;
}

#endif /* !defined(_WIN32) */
