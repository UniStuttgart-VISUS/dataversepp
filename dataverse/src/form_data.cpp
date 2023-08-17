// <copyright file="form_data.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/form_data.h"

#include <memory>
#include <stdexcept>
#include <system_error>

#include <curl/curl.h>

#include "dataverse/convert.h"

#include "curl_error_category.h"
#include "errors.h"


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
visus::dataverse::form_data::add_field(_In_z_ const char_type *name,
        _In_z_ const char_type *value) {
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
        auto v = to_utf8(value);
        auto status = ::curl_mime_data(field, v.c_str(), CURL_ZERO_TERMINATED);
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
        _In_z_ const char_type *name,
        _In_reads_bytes_(cnt) _In_ const std::uint8_t *data,
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
 * visus::dataverse::form_data::add_file
 */
visus::dataverse::form_data& visus::dataverse::form_data::add_file(
        _In_z_ const char_type *name, _In_z_ const char_type *path) {
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
 * visus::dataverse::form_data::check_not_disposed
 */
void visus::dataverse::form_data::check_not_disposed(void) {
    if (this->_form == nullptr) {
        throw std::system_error(ERROR_INVALID_STATE, std::system_category());
    }
}
