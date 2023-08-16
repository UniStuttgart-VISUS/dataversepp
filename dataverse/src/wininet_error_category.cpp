// <copyright file="wininet_error_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "wininet_error_category.h"

#if defined(_WIN32)
#include <Windows.h>
#include <wininet.h>

#include <wil/resource.h>


/*
 * visus::dataverse::detail::wininet_error_category::default_error_condition
 */
std::error_condition
visus::dataverse::detail::wininet_error_category::default_error_condition(
        _In_ int error) const noexcept {
    return std::error_condition(error, wininet_category());
}


/*
 * visus::dataverse::detail::wininet_error_category::message
 */
std::string visus::dataverse::detail::wininet_error_category::message(
        _In_ int error) const {
    char *msg = nullptr;

    const auto length = ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
        GetModuleHandle(L"wininet.dll"),
        error,
        0,
        reinterpret_cast<char *>(&msg),
        0,
        nullptr);

    if (length > 0) {
        wil::scope_exit([&msg](void) { ::LocalFree(msg); });
        return std::string(msg, msg + length);
    } else {
        return std::to_string(error);
    }
}


/*
 * visus::dataverse::detail::wininet_category
 */
_Ret_valid_ const std::error_category&
visus::dataverse::detail::wininet_category(void) noexcept {
    static const wininet_error_category retval;
    return retval;
}
#endif /* _WIN32 */
