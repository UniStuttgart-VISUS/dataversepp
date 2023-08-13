// <copyright file="openssl_error_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "openssl_error_category.h"

#include <array>


/*
 * visus::dataverse::detail::openssl_error_category::default_error_condition
 */
std::error_condition
visus::dataverse::detail::openssl_error_category::default_error_condition(
        _In_ int error) const noexcept {
    return std::error_condition(error, openssl_category());
}


/*
 * visus::dataverse::detail::openssl_error_category::message
 */
std::string visus::dataverse::detail::openssl_error_category::message(
        _In_ int error) const {
    std::array<char, 256> msg;
    throw "TODO";
    //ERR_error_string_n(error, msg.data(), msg.size());
    return msg.data();
}


/*
 * visus::dataverse::detail::openssl_category
 */
_Ret_valid_ const std::error_category&
visus::dataverse::detail::openssl_category(void) noexcept {
    static const openssl_error_category retval;
    return retval;
}
