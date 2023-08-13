// <copyright file="resolve.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "resolve.h"

#include <cstring>
#include <string>

#if defined(_WIN32)
#include <WS2tcpip.h>

#include <wil/resource.h>
#else /* defined(_WIN32) */
#include <sys/socket.h>
#endif /* defined(_WIN32) */

#include "errors.h"


/*
 * visus::dataverse::detail::resolve
 */
std::vector<sockaddr_storage> visus::dataverse::detail::resolve(
        _In_z_ const char_type *host,
        _In_z_ const char_type *service,
        _In_ const int address_family) {
    if (host == nullptr) {
        throw std::invalid_argument("The host to resolve to must be valid.");
    }
    if (service == nullptr) {
        throw std::invalid_argument("The service to resolve must be valid");
    }

    std::vector<sockaddr_storage> retval;

#if defined(_WIN32)
    wil::unique_any<ADDRINFOEXW *, decltype(&::FreeAddrInfoExW),
        ::FreeAddrInfoExW> addresses;
    wil::unique_any<ADDRINFOW *, decltype(&::FreeAddrInfoW),
        ::FreeAddrInfoW> fallback_addresses;

    // Resolve the end point.
    {
        ADDRINFOEXW hints;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = address_family;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        auto status = ::GetAddrInfoExW(
            reinterpret_cast<const wchar_t *>(host),
            reinterpret_cast<const wchar_t *>(service),
            NS_DNS,
            nullptr,
            &hints,
            addresses.addressof(),
            nullptr, nullptr, nullptr, nullptr);
        if (status == WSATRY_AGAIN) {
            // GetAddrInfoEx fails when impersonated for unknown reasons, but
            // according to https://github.com/dotnet/runtime/pull/45816,
            // GetAddrInfo works, so we try this when we encounter this issue.
            // The reason for not going for GetAddrInfo in the first place is
            // that this function cannot resolve numeric IP addresses, whereas
            // GetAddrInfoEx can. This has the nasty effect that in case of
            // running from the command line and impersonating a domain admin,
            // the service can only use host names, not addresses.
            ADDRINFOW hints;
            ZeroMemory(&hints, sizeof(hints));
            hints.ai_family = address_family;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            status = ::GetAddrInfoW(
                reinterpret_cast<const wchar_t *>(host),
                reinterpret_cast<const wchar_t *>(service),
                &hints,
                fallback_addresses.addressof());
        }

        if (status != ERROR_SUCCESS) {
            throw std::system_error(status, std::system_category());
        }
    }

    for (auto a = addresses.get(); a != nullptr; a = a->ai_next) {
        retval.emplace_back();
        ::memcpy(&retval.back(), a->ai_addr, a->ai_addrlen);
    }

    // Append the fallback stuff at the end, as we typically try connecting from
    // the start of the array.
    for (auto a = fallback_addresses.get(); a != nullptr; a = a->ai_next) {
        retval.emplace_back();
        ::memcpy(&retval.back(), a->ai_addr, a->ai_addrlen);
    }

#else  /* defined(_WIN32) */
    throw "TODO: Implement address resolution on Linux.";
    //    int address_family;
    //::getsockopt(this->_handle, SOL_SOCKET, SO_DOMAIN, &address_family, sizeof(address_family));
#endif /* defined(_WIN32) */

    return retval;
}


/*
 * visus::dataverse::detail::resolve
 */
std::vector<sockaddr_storage> visus::dataverse::detail::resolve(
        _In_z_ const char_type *host,
        _In_ const std::uint16_t port,
        _In_ const int address_family) {
#if defined(_WIN32)
    auto service0 = std::to_wstring(port);
    auto service = reinterpret_cast<const char_type *>(service0.c_str());
#else /* defined(_WIN32) */
    auto service0 = std::to_string(port);
    auto service = reinterpret_cast<const char_type *>(service0.c_str());
#endif /* defined(_WIN32) */
    return resolve(host, service, address_family);
}
