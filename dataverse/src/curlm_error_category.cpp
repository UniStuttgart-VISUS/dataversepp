// <copyright file="curlm_error_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "curlm_error_category.h"


/*
 * visus::dataverse::detail::curlm_error_category::default_error_condition
 */
std::error_condition
visus::dataverse::detail::curlm_error_category::default_error_condition(
        _In_ int error) const noexcept {
    const auto code = static_cast<CURLMcode>(error);
    return std::error_condition(code, curlm_category());
}


/*
 * visus::dataverse::detail::curlm_error_category::message
 */
std::string visus::dataverse::detail::curlm_error_category::message(
        _In_ int error) const {
    const auto code = static_cast<CURLMcode>(error);
    return curl_multi_strerror(code);
}


/*
 * visus::dataverse::detail::curlm_category
 */
_Ret_valid_ const std::error_category&
visus::dataverse::detail::curlm_category(void) noexcept {
    static const curlm_error_category retval;
    return retval;
}
