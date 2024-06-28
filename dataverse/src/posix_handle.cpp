// <copyright file="posix_handle.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "posix_handle.h"

#include <memory>

#include <fcntl.h>

#if defined(_WIN32)
#include <io.h>
#else /* defined(_WIN32) */
#include <unistd.h>
#endif /* defined(_WIN32) */


/*
 * visus::dataverse::detail::posix_handle::posix_handle
 */
visus::dataverse::detail::posix_handle::posix_handle(
        _Inout_ posix_handle&& rhs) noexcept : _handle(rhs._handle) {
    rhs._handle = -1;
}


/*
 * visus::dataverse::detail::posix_handle::~posix_handle
 */
visus::dataverse::detail::posix_handle::~posix_handle(void) noexcept {
    *this = -1;
}


/*
 * visus::dataverse::detail::posix_handle::operator =
 */
visus::dataverse::detail::posix_handle&
visus::dataverse::detail::posix_handle::operator =(
        _Inout_ posix_handle&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        *this = rhs._handle;
        rhs._handle = -1;
    }

    return *this;
}


/*
 * visus::dataverse::detail::posix_handle::operator =
 */
visus::dataverse::detail::posix_handle&
visus::dataverse::detail::posix_handle::operator =(
        _In_ const int handle) noexcept {
    if ((this->_handle != -1) && (this->_handle != handle)) {
#if defined(_WIN32)
        ::_close(this->_handle);
#else /* defined(_WIN32) */
        ::close(this->_handle);
#endif /* defined(_WIN32) */
    }

    this->_handle = handle;
    return *this;
}
