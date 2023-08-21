// <copyright file="ntstatus_error_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "ntstatus_error_category.h"

#if defined(_WIN32)
#include "dataverse/convert.h"


/*
 * visus::dataverse::detail::ntstatus_error_category::default_error_condition
 */
std::error_condition
visus::dataverse::detail::ntstatus_error_category::default_error_condition(
        _In_ int error) const noexcept {
    OVERLAPPED overlapped;
    DWORD transferred;

    ::ZeroMemory(&overlapped, sizeof(overlapped));
    overlapped.Internal = error;

    ::GetOverlappedResult(NULL, &overlapped, &transferred, FALSE);
    return std::system_category().default_error_condition(::GetLastError());
}


/*
 * visus::dataverse::detail::ntstatus_error_category::message
 */
std::string visus::dataverse::detail::ntstatus_error_category::message(
        _In_ int error) const {
    OVERLAPPED overlapped;
    DWORD transferred;

    ::ZeroMemory(&overlapped, sizeof(overlapped));
    overlapped.Internal = error;

    ::GetOverlappedResult(NULL, &overlapped, &transferred, FALSE);
    return std::system_category().message(::GetLastError());
}


/*
 * visus::dataverse::detail::ntstatus_category
 */
_Ret_valid_ const std::error_category&
visus::dataverse::detail::ntstatus_category(void) noexcept {
    static const ntstatus_error_category retval;
    return retval;
}
#endif /* _WIN32 */
