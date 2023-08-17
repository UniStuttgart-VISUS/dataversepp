// <copyright file="curl_error_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "curl_error_category.h"


/*
 * visus::dataverse::detail::curl_error_category::default_error_condition
 */
std::error_condition
visus::dataverse::detail::curl_error_category::default_error_condition(
        _In_ int error) const noexcept {
    const auto code = static_cast<CURLcode>(error);
    return std::error_condition(code, curl_category());
}


/*
 * visus::dataverse::detail::curl_error_category::message
 */
std::string visus::dataverse::detail::curl_error_category::message(
        _In_ int error) const {
    const auto code = static_cast<CURLcode>(error);
    return curl_easy_strerror(code);
}


/*
 * visus::dataverse::detail::curl_category
 */
_Ret_valid_ const std::error_category&
visus::dataverse::detail::curl_category(void) noexcept {
    static const curl_error_category retval;
    return retval;
}
