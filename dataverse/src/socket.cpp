// <copyright file="socket.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/socket.h"

#include <memory>
#include <system_error>

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#endif /* defined(_WIN32) */

#if !defined(_WIN32)
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif /* !defined(_WIN32) */

#if defined(_WIN32)
#define THROW_SOCKET_EXCEPTION throw std::system_error(::WSAGetLastError(), \
    std::system_category())
#else /* defined(_WIN32) */
#define THROW_SOCKET_EXCEPTION throw std::system_error(errno, \
    std::system_category())
#endif /* defined(_WIN32) */


/*
 * visus::dataverse::detail::socket::socket
 */
visus::dataverse::detail::socket::socket(void) noexcept
    : _handle(INVALID_SOCKET) { }


/*
 * visus::dataverse::detail::socket::socket
 */
visus::dataverse::detail::socket::socket(_In_ const int address_family)
        : _handle(INVALID_SOCKET) {
#if defined(_WIN32)
    this->_handle = ::WSASocketW(address_family, SOCK_STREAM, IPPROTO_TCP,
        nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT | WSA_FLAG_OVERLAPPED);
    if (this->_handle == INVALID_SOCKET) {
        THROW_SOCKET_EXCEPTION;
    }

#else /* defined(_WIN32) */
    this->_handle = ::socket(address_family, SOCK_STREAM, IPPROTO_TCP);
    if (this->_handle == INVALID_SOCKET) {
        THROW_SOCKET_EXCEPTION;
    }
#endif /* defined(_WIN32) */

    // Enable REUSEADDR socket object to circumvent cooldown after restart.
    {
        unsigned int value = 1;
        if (::setsockopt(this->_handle,
                SOL_SOCKET,
                SO_REUSEADDR,
                reinterpret_cast<char *>(&value),
                sizeof(value))
                == SOCKET_ERROR) {
            THROW_SOCKET_EXCEPTION;
        }
    }
}


/*
 * visus::dataverse::detail::socket::socket
 */
visus::dataverse::detail::socket::socket(_Inout_ socket&& rhs) noexcept
        : _handle(rhs._handle) {
    rhs._handle = INVALID_SOCKET;
}


/*
 * visus::dataverse::detail::socket::~socket
 */
visus::dataverse::detail::socket::~socket(void) noexcept {
    this->close(std::nothrow);
}


/*
 * visus::dataverse::detail::socket::connect
 */
void visus::dataverse::detail::socket::connect(
        _In_reads_bytes_(length) sockaddr *address,
        _In_ const std::size_t length) {
#if defined(_WIN32)
    if (::WSAConnect(this->_handle,
            address,
            static_cast<int>(length),
            nullptr,
            nullptr,
            nullptr,
            nullptr)
            == SOCKET_ERROR) {
        THROW_SOCKET_EXCEPTION;
    }
#else /* defined(_WIN32) */
    if (::connect(this->_handle,
            address,
            static_cast<int>(length)
            == -1) {
        THROW_SOCKET_EXCEPTION;
    }
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::detail::socket::close
 */
visus::dataverse::system_error_code visus::dataverse::detail::socket::close(
        _In_ std::nothrow_t) noexcept {
    if (*this) {
#if defined(_WIN32)
        if (::closesocket(this->_handle) == SOCKET_ERROR) {
            return ::WSAGetLastError();
        }
#else /* defined(_WIN32) */
        if (::close(this->_handle) == SOCKET_ERROR) {
            return errno;
        }
#endif /* defined(_WIN32) */

        this->_handle = INVALID_SOCKET;
    }

    return ERROR_SUCCESS;
}


/*
 * visus::dataverse::detail::socket::close
 */
void visus::dataverse::detail::socket::close(void) {
    auto error = this->close(std::nothrow);
    if (error != ERROR_SUCCESS) {
        throw std::system_error(error, std::system_category());
    }
}


/*
 * visus::dataverse::detail::socket::operator =
 */
visus::dataverse::detail::socket& visus::dataverse::detail::socket::operator =(
        _Inout_ socket&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        this->close(std::nothrow);
        this->_handle = rhs._handle;
        rhs._handle = INVALID_SOCKET;
    }

    return *this;
}


/*
 * visus::dataverse::detail::socket::operator bool
 */
visus::dataverse::detail::socket::operator bool(void) const noexcept {
    return (this->_handle != INVALID_SOCKET);
}

