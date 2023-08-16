// <copyright file="dataverse_connection.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/dataverse_connection.h"

#include <iterator>

#include "dataverse_connection_impl.h"
#include "errors.h"
#include "wininet_error_category.h"


/*
 * visus::dataverse::dataverse_connection::dataverse_connection
 */
visus::dataverse::dataverse_connection::dataverse_connection(void)
        : _impl(new detail::dataverse_connection_impl()) { }


/*
 * visus::dataverse::dataverse_connection::dataverse_connection
 */
visus::dataverse::dataverse_connection::dataverse_connection(
        _Inout_ dataverse_connection&& rhs) noexcept : _impl(rhs._impl) {
    rhs._impl = nullptr;
}


/*
 * visus::dataverse::dataverse_connection::~dataverse_connection
 */
visus::dataverse::dataverse_connection::~dataverse_connection(void) {
    if (this->_impl != nullptr) {
        this->api_key(nullptr);
    }

    delete this->_impl;
}


/*
 * visus::dataverse::dataverse_connection::api_key
 */
visus::dataverse::dataverse_connection& 
visus::dataverse::dataverse_connection::api_key(
        _In_opt_z_ const char_type *api_key) {
    auto& i = this->check_not_disposed();

    // Make sure that the old API key is erased from memory.
#if defined(_WIN32)
    ::SecureZeroMemory(i.api_key.data(), i.api_key.size());
#else /* defined(_WIN32) */
    ::memset(i.api_key.data(), 0, i.api_key.size());
#endif /* defined(_WIN32) */

    if (api_key != nullptr) {
        const auto len = std::char_traits<char_type>::length(api_key);
        i.api_key.resize(len);
        std::copy(api_key, api_key + len, i.api_key.begin());
    } else {
        this->_impl->api_key.clear();
    }

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::base_path
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::base_path(
        _In_z_ const char_type *base_path) {
    auto& i = this->check_not_disposed();

    if (base_path != nullptr) {
        i.base_path = base_path;
    } else {
        i.base_path.clear();
    }

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::connect
 */
void visus::dataverse::dataverse_connection::connect(
        _In_z_ const char_type *host,
        _In_ const std::uint16_t port,
        _In_ const bool tls) {
    auto& i = this->check_not_disposed();

#if defined(_WIN32)
    if (!i.session) {
        i.session.reset(::InternetOpenW(L"Dataverse++",
            INTERNET_OPEN_TYPE_PRECONFIG,
            nullptr,
            nullptr,
            INTERNET_FLAG_DONT_CACHE));
    }
    if (!i.session) {
        throw std::system_error(::GetLastError(), detail::wininet_category());
    }

    i.connection.reset(::InternetConnectW(i.session.get(),
        host,
        port,
        nullptr,
        nullptr,
        INTERNET_SERVICE_HTTP,
        0,
        reinterpret_cast<DWORD_PTR>(this->_impl)));
    if (!this->_impl->connection) {
        throw std::system_error(::GetLastError(), detail::wininet_category());
    }
#else /* defined(_WIN32) */
#endif /* defined(_WIN32) */

    i.tls = tls;
}


/*
 * visus::dataverse::dataverse_connection::get
 */
visus::dataverse::blob visus::dataverse::dataverse_connection::get(
        _In_z_ const char_type *resource,
        _In_opt_z_ const char_type *version) {
    auto& i = this->check_not_disposed();
    blob retval(1024);

#if defined(_WIN32)
    auto context = reinterpret_cast<DWORD_PTR>(this->_impl);

    auto resource_name = i.base_path;
    if (resource != nullptr) {
        resource_name += resource;
    }

    DWORD flags = 0;
    if (i.tls) {
        flags |= INTERNET_FLAG_SECURE;
    }

    // Post the request.
    ::OutputDebugString((L"GET " + resource_name + L"\r\n").c_str());
    wil::unique_hinternet request(::HttpOpenRequestW(
        i.connection.get(),
        L"GET",
        resource_name.c_str(),
        version,
        nullptr,
        nullptr,
        flags,
        context));
    if (!request) {
        throw std::system_error(::GetLastError(), detail::wininet_category());
    }

    if (!::HttpSendRequestW(request.get(), nullptr, 0, nullptr, context)) {
        throw std::system_error(::GetLastError(), detail::wininet_category());
    }

    // Read the response.
    auto done = false;
    std::size_t offset = 0;

    while (!done) {
        DWORD read = 0;

        if (!::InternetReadFile(request.get(),
                retval.at(offset),
                static_cast<DWORD>(retval.size() - offset),
                &read)) {
            throw std::system_error(::GetLastError(), std::system_category());
        }

        offset += read;
        done = (read <= 0);
        retval.grow(retval.size() + retval.size() / 2);
    }

    retval.truncate(offset);
#endif /* defined(_WIN32) */

    return retval;
}


/*
 * visus::dataverse::dataverse_connection::operator =
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::operator =(
        _Inout_ dataverse_connection&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        delete this->_impl;
        this->_impl = rhs._impl;
        rhs._impl = nullptr;
    }

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::check_not_disposed
 */
visus::dataverse::detail::dataverse_connection_impl&
visus::dataverse::dataverse_connection::check_not_disposed(void) const {
    if (this->_impl == nullptr) {
        throw std::system_error(ERROR_INVALID_STATE, std::system_category());
    } else {
        return *this->_impl;
    }
}
