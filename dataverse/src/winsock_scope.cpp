// <copyright file="winsock_scope.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/winsock_scope.h"

#include <system_error>

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#endif /* defined(_WIN32) */


/*
 visus::dataverse::* winsock_scope::winsock_scope
 */
visus::dataverse::winsock_scope::winsock_scope(_In_ const std::uint8_t major,
        _In_ const std::uint8_t minor) {
#if defined(_WIN32)
    WSADATA wsa_data;

    auto status = ::WSAStartup(MAKEWORD(major, minor), &wsa_data);
    if (status != ERROR_SUCCESS) {
        throw std::system_error(status, std::system_category());
    }
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::winsock_scope::~winsock_scope
 */
visus::dataverse::winsock_scope::~winsock_scope(void) noexcept {
#if defined(_WIN32)
    ::WSACleanup();
#endif /* defined(_WIN32) */
}
