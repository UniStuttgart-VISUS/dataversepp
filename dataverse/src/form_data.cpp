// <copyright file="form_data.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/form_data.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <system_error>

#include <curl/curl.h>

#include "dataverse/convert.h"

#include "curl_error_category.h"
#include "errors.h"


#if defined(_WIN32)
/*
 * visus::dataverse::form_data::win32_read
 */
std::size_t CALLBACK visus::dataverse::form_data::win32_read(
        _Out_writes_bytes_(cnt *size) char *dst,
        _In_ const size_t size,
        _In_ const size_t cnt,
        _In_opt_ void *handle) {
    auto h = reinterpret_cast<HANDLE>(handle);
    auto c = static_cast<DWORD>(cnt * size);
    DWORD retval = 0;

    if (!::ReadFile(h, dst, c, &retval, nullptr)) {
        return CURL_READFUNC_ABORT;
    } else {
        return retval;
    }
}
#endif /* defined(_WIN32) */


#if defined(_WIN32)
/*
 * visus::dataverse::form_data::win32_seek
 */
int CALLBACK visus::dataverse::form_data::win32_seek(_In_opt_ void *handle,
        _In_ const std::streamoff offset,
        _In_ const int origin) {
    static_assert(SEEK_SET == FILE_BEGIN, "SEEK_SET matches FILE_BEGIN");
    static_assert(SEEK_CUR == FILE_CURRENT, "SEEK_CUR matches FILE_CURRENT");
    static_assert(SEEK_END == SEEK_END, "SEEK_SET matches FILE_END");
    auto h = reinterpret_cast<HANDLE>(handle);

    LARGE_INTEGER o;
    o.QuadPart = offset;

    return ::SetFilePointerEx(h, o, nullptr, origin)
        ? CURL_SEEKFUNC_OK
        : CURL_SEEKFUNC_FAIL;
}
#endif /* defined(_WIN32) */


#if defined(_WIN32)
/*
 * visus::dataverse::form_data::win32_close
 */
void CALLBACK visus::dataverse::form_data::win32_close(_In_ void *handle) {
    ::CloseHandle(reinterpret_cast<HANDLE>(handle));
}
#endif /* defined(_WIN32) */


/*
 * visus::dataverse::form_data::form_data
 */
visus::dataverse::form_data::form_data(void) : _form(nullptr) { }


/*
 * visus::dataverse::form_data::form_data
 */
visus::dataverse::form_data::form_data(_Inout_ form_data&& rhs) noexcept
        : _form(rhs._form) {
    rhs._form = nullptr;
}


/*
 * visus::dataverse::form_data::~form_data
 */
visus::dataverse::form_data::~form_data(void) noexcept {
    if (this->_form != nullptr) {
        ::curl_mime_free(this->_form);
    }
}


/*
 * visus::dataverse::form_data::add_field
 */
visus::dataverse::form_data&
visus::dataverse::form_data::add_field(_In_z_ const wchar_t *name,
        _In_z_ const wchar_t *value) {
    this->check_not_disposed();
    const auto n = to_utf8(name);
    const auto v = to_utf8(value);
    return this->add_field(n.c_str(), v.c_str());
}


/*
 * visus::dataverse::form_data::add_field
 */
visus::dataverse::form_data& visus::dataverse::form_data::add_field(
        _In_ const const_narrow_string& name,
        _In_ const const_narrow_string& value) {
    this->check_not_disposed();
    const auto n = to_utf8(name);
    const auto v = to_utf8(value);
    return this->add_field(n.c_str(), v.c_str());
}


/*
 * visus::dataverse::form_data::add_field
 */
visus::dataverse::form_data& visus::dataverse::form_data::add_field(
        _In_z_ const wchar_t *name,
        _In_reads_bytes_(cnt) _In_ const byte_type *data,
        _In_ const std::size_t cnt) {
    this->check_not_disposed();

    auto field = ::curl_mime_addpart(this->_form);
    if (field == nullptr) {
        throw std::bad_alloc();
    }

    {
        auto n = to_utf8(name);
        auto status = ::curl_mime_name(field, n.c_str());
        if (status != CURLE_OK) {
            throw std::system_error(status, detail::curl_category());
        }
    }

    {
        auto v = reinterpret_cast<const char *>(data);
        auto status = ::curl_mime_data(field, v, cnt);
        if (status != CURLE_OK) {
            throw std::system_error(status, detail::curl_category());
        }
    }

    return *this;
}


/*
 * visus::dataverse::form_data::add_field
 */
visus::dataverse::form_data& visus::dataverse::form_data::add_field(
        _In_ const const_narrow_string& name,
        _In_reads_bytes_(cnt) _In_ const byte_type *data,
        _In_ const std::size_t cnt) {
    // Note: the direct to_utf8 would do the same internally, so there is no
    // performance penalty in being lazy here ...
    auto n = convert<wchar_t>(name);
    return this->add_field(n.c_str(), data, cnt);
}


/*
 * visus::dataverse::form_data::add_file
 */
visus::dataverse::form_data& visus::dataverse::form_data::add_file(
        _In_z_ const wchar_t *name, _In_z_ const wchar_t *path) {
    this->check_not_disposed();

    auto field = ::curl_mime_addpart(this->_form);
    if (field == nullptr) {
        throw std::bad_alloc();
    }

    {
        auto n = to_utf8(name);
        auto status = ::curl_mime_name(field, n.c_str());
        if (status != CURLE_OK) {
            throw std::system_error(status, detail::curl_category());
        }
    }

    {
        auto v = to_utf8(path);
        auto status = ::curl_mime_filedata(field, v.c_str());
        if (status != CURLE_OK) {
            throw std::system_error(status, detail::curl_category());
        }
    }

    return *this;
}


/*
 * visus::dataverse::form_data::add_file
 */
visus::dataverse::form_data& visus::dataverse::form_data::add_file(
        _In_ const const_narrow_string& name,
        _In_ const const_narrow_string& path) {
    // Note: the direct to_utf8 would do the same internally, so there is no
    // performance penalty in being lazy here ...
    auto n = convert<wchar_t>(name);
    auto v = convert<wchar_t>(path);
    return this->add_file(n.c_str(), v.c_str());
}



/*
 * visus::dataverse::form_data::add_file
 */
visus::dataverse::form_data& visus::dataverse::form_data::add_file(
        _In_z_ const wchar_t *name,
        _In_ const std::size_t size,
        _In_ on_read_type on_read,
        _In_opt_ on_seek_type on_seek,
        _In_opt_ on_close_type on_close,
        _In_opt_ void *context) {
    this->check_not_disposed();

    auto field = ::curl_mime_addpart(this->_form);
    if (field == nullptr) {
        throw std::bad_alloc();
    }

    {
        auto n = to_utf8(name);
        auto status = ::curl_mime_name(field, n.c_str());
        if (status != CURLE_OK) {
            throw std::system_error(status, detail::curl_category());
        }
    }

    {
        auto status = ::curl_mime_data_cb(field, size, on_read, on_seek,
            on_close, context);
        if (status != CURLE_OK) {
            throw std::system_error(status, detail::curl_category());
        }
    }

    return *this;
}


/*
 * visus::dataverse::form_data::add_file
 */
visus::dataverse::form_data& visus::dataverse::form_data::add_file(
        _In_ const const_narrow_string& name,
        _In_ const std::size_t size,
        _In_ on_read_type on_read,
        _In_opt_ on_seek_type on_seek,
        _In_opt_ on_close_type on_close,
        _In_opt_ void *context) {
    // Note: the direct to_utf8 would do the same internally, so there is no
    // performance penalty in being lazy here ...
    auto n = convert<wchar_t>(name);
    return this->add_file(n.c_str(), size, on_read, on_seek, on_close, context);
}


/*
 * visus::dataverse::form_data::operator =
 */
visus::dataverse::form_data&
visus::dataverse::form_data::operator =(_Inout_ form_data&& rhs) noexcept {
    if (this != std::addressof(rhs)) {
        if (this->_form != nullptr) {
            ::curl_mime_free(this->_form);
        }

        this->_form = rhs._form;
        rhs._form = nullptr;
    }

    return *this;
}


/*
 * visus::dataverse::form_data::form_data
 */
visus::dataverse::form_data::form_data(_In_ void *curl) : _form(nullptr) {
    if (curl == nullptr) {
        throw std::invalid_argument("A valid CURL handle must be provided.");
    }

    this->_form = curl_mime_init(curl);
}



/*
 * visus::dataverse::form_data::add_field
 */
visus::dataverse::form_data& visus::dataverse::form_data::add_field(
        _In_z_ const char *name, _In_z_ const char *value) {
    auto field = ::curl_mime_addpart(this->_form);
    if (field == nullptr) {
        throw std::bad_alloc();
    }

    {
        auto status = ::curl_mime_name(field, name);
        if (status != CURLE_OK) {
            throw std::system_error(status, detail::curl_category());
        }
    }

    {
        auto status = ::curl_mime_data(field, value, CURL_ZERO_TERMINATED);
        if (status != CURLE_OK) {
            throw std::system_error(status, detail::curl_category());
        }
    }

    return *this;
}


/*
 * visus::dataverse::form_data::check_not_disposed
 */
void visus::dataverse::form_data::check_not_disposed(void) {
    if (this->_form == nullptr) {
        throw std::system_error(ERROR_INVALID_STATE, std::system_category());
    }
}

