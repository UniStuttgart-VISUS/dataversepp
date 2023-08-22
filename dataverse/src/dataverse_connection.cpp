// <copyright file="dataverse_connection.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <nlohmann/json.hpp>
#include "dataverse/dataverse_connection.h"

#include <cassert>
#include <iterator>

#include "dataverse_connection_impl.h"
#include "direct_upload_context.h"
#include "file_properties.h"
#include "io_context.h"


#define _CHECK_ON_RESPONSE if (on_response == nullptr) \
    throw std::invalid_argument("The response handler must be valid.")

#define _CHECK_ON_ERROR if (on_error == nullptr) \
    throw std::invalid_argument("The error handler must be valid.")

#define _DATAVERSE_HASH_ALGORITHMA "MD5"

#define _DATAVERSE_HASH_ALGORITHMW L"MD5"



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
    delete this->_impl;
}


/*
 * visus::dataverse::dataverse_connection::api_key
 */
visus::dataverse::dataverse_connection& 
visus::dataverse::dataverse_connection::api_key(
        _In_opt_z_ const wchar_t *api_key) {
    auto& i = this->check_not_disposed();

    // Make sure that the old API key is erased from memory.
    detail::dataverse_connection_impl::secure_zero(i.api_key);

    if (api_key != nullptr) {
        const auto len = ::wcslen(api_key);
        i.api_key.resize(len + 1);
        to_ascii(i.api_key.data(), i.api_key.size(), api_key, 0);
        i.api_key.back() = static_cast<wchar_t>(0);
    } else {
        this->_impl->api_key.clear();
    }

    return *this;
}



/*
 * visus::dataverse::dataverse_connection::api_key
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::api_key(
        _In_ const const_narrow_string& api_key) {
    auto& i = this->check_not_disposed();

    // Make sure that the old API key is erased from memory.
    detail::dataverse_connection_impl::secure_zero(i.api_key);

    if (api_key != nullptr) {
        const auto len = ::strlen(api_key);
        i.api_key.resize(len + 1);
        to_ascii(i.api_key.data(), i.api_key.size(), api_key, 0,
            api_key.code_page());
        i.api_key.back() = static_cast<wchar_t>(0);
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
        _In_z_ const wchar_t *base_path) {
    auto& i = this->check_not_disposed();

    if (base_path != nullptr) {
        i.base_path = to_ascii(base_path);
    } else {
        i.base_path.clear();
    }

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::base_path
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::base_path(
        _In_ const const_narrow_string& base_path) {
    auto& i = this->check_not_disposed();

    if (base_path != nullptr) {
        i.base_path = to_ascii(base_path);
    } else {
        i.base_path.clear();
    }

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::direct_upload
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::direct_upload(
        _In_z_ const wchar_t *persistent_id,
        _In_z_ const wchar_t *path,
        _In_z_ const wchar_t *description,
        _In_z_ const wchar_t *directory,
        const wchar_t **categories,
        _In_ const std::size_t cnt_cats,
        _In_ const bool restricted,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    using detail::direct_upload_context;
    _CHECK_ON_RESPONSE;
    _CHECK_ON_ERROR;

    // This is the generic error handler that forwards errors directly to
    // the user-provided callback.
    const auto on_error_forwarder = [](const int c, const char *m,
            const char *a, const narrow_string::code_page_type p, void *u) {
        auto ctx = static_cast<direct_upload_context *>(u);
        assert(ctx != nullptr);
        assert(ctx->on_error != nullptr);
        ctx->on_error(c, m, a, p, ctx->user_context);
        delete ctx;
    };

    // This is performed once the request for the one-time upload URL succeeded.
    const auto on_upload_url = [](const blob &r, void *u) {
        auto ctx = static_cast<direct_upload_context *>(u);
        try {
            const auto url = ctx->upload_url(r);
            auto c = detail::io_context::create();
            c->client_data = u;
            //c->prepare_request(data, cnt, data_deleter);

            //::curl_easy_setopt(i.curl.get(), CURLOPT_URL, url.c_str());
            //::curl_easy_setopt(i.curl.get(), CURLOPT_UPLOAD, 1L);
            //::curl_easy_setopt(i.curl.get(), CURLOPT_PUT, 1L);
            ////::curl_easy_setopt(i.curl.get(), CURLOPT_INFILESIZE_LARGE, cnt);
            //::curl_easy_setopt(i.curl.get(), CURLOPT_WRITEDATA, c.get());
            //::curl_easy_setopt(i.curl.get(), CURLOPT_READDATA, c.get());
            //::curl_easy_setopt(i.curl.get(), CURLOPT_READFUNCTION,
            //    &detail::io_context::read_request);

            //// Set the headers.
            //if (content_type != nullptr) {
            //    auto h = "Content-Type: " + to_ascii(content_type);
            //    headers.reset(::curl_slist_append(headers.release(), h.c_str()));
            //}
            //::curl_easy_setopt(i.curl.get(), CURLOPT_HTTPHEADER, headers.get());

            //auto status = ::curl_easy_perform(i.curl.get());
            //if (status != CURLE_OK) {
            //    throw std::system_error(status, detail::curl_category());
            //}

            //on_response(c->response, c->client_data);
            //detail::io_context::recycle(std::move(c));


            //curl -X PUT -H "x-amz-tagging: dv-state=temp" --upload-file <path-to-file> "<url>"
        } catch (std::system_error ex) {
            ctx->invoke_on_error(ex);
            delete ctx;

        } catch (std::exception& ex) {
            ctx->invoke_on_error(ex);
            delete ctx;

        } catch (...) {
            ctx->invoke_on_error();
            delete ctx;
        }
    };

    // Allocate the upload context which helps us tracking the progress through
    // the individual stages of the process. From here on, the context must be
    // successfully passed to the API or freed in case of any error, wherefore
    // the following code must be enclosed in try/catch.
    auto ctx = new direct_upload_context(this, on_response, on_error, context);

    try {
        // Prepare as much of the description as we can right now.
        ctx->description = nlohmann::json::object({
            { "description", to_utf8(description) },
            { "directoryLabel", to_utf8(directory) },
            { "restrict", restricted },
            { "categories", nlohmann::json::array() },
        });

        if (categories != nullptr) {
            auto &cats = ctx->description["categories"];
            for (std::size_t i = 0; i < cnt_cats; ++i) {
                cats.push_back(to_utf8(categories[i]));
            }
        }

        // Add metadata about the file itself (size, name, etc.).
        ctx->description.update(detail::get_file_properties(path));

        // The URL we will later use to register the metadata.
        ctx->registration_url = std::wstring(L"/datasets/:persistentId/add?"
            L"?persistentId=") + persistent_id;

        // Begin the chain of operations by retrieving the upload URL.
        const auto url = std::wstring(L"/datasets/:persistentId/uploadsid/"
            L"?persistentId=") + persistent_id;
        return this->get(url.c_str(), on_upload_url, on_error_forwarder, ctx);
    } catch (...) {
        delete ctx;
        throw;
    }
}


/*
 * visus::dataverse::dataverse_connection::get
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::get(
        _In_opt_z_ const wchar_t *resource,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    _CHECK_ON_RESPONSE;
    _CHECK_ON_ERROR;
    auto& i = this->check_not_disposed();

    // Prepare the request.
    auto ctx = detail::io_context::create(i.make_url(resource),
        on_response, on_error, context);
    assert(ctx->curl != nullptr);
    assert(ctx->client_data == context);

    // Set the authentication header.
    i.add_auth_header(ctx);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::get
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::get(
        _In_ const const_narrow_string& resource,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    // Note: The ASCII conversion would go via wchar_t anyway, so this is
    // basically for free.
    if (resource == nullptr) {
        auto r = static_cast<wchar_t *>(nullptr);
        return this->get(r, on_response, on_error, context);
    } else {
        auto r = convert<wchar_t>(resource);
        return this->get(r.c_str(), on_response, on_error, context);
    }
}


/*
 * visus::dataverse::dataverse_connection::make_form
 */
visus::dataverse::form_data visus::dataverse::dataverse_connection::make_form(
        void) const {
    return form_data(detail::dataverse_connection_impl::make_curl().release());
}


/*
 * visus::dataverse::dataverse_connection::post
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::post(
        _In_opt_z_ const wchar_t *resource,
        _Inout_ form_data&& form,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    _CHECK_ON_RESPONSE;
    _CHECK_ON_ERROR;

    if (!form) {
        throw std::invalid_argument("The form must be valid.");
    }

    auto& i = this->check_not_disposed();

    // Prepare the request.
    auto ctx = detail::io_context::create(form._curl, i.make_url(resource),
        on_response, on_error, context);
    ctx->form = std::move(form);
    assert(!form);

    // Context has taken ownership of CURL object, so erase it from form.
    form._curl = nullptr;

    detail::dataverse_connection_impl::check_code(::curl_easy_setopt(
        ctx->curl.get(), CURLOPT_MIMEPOST, ctx->form._form));

    // Set the authentication header.
    i.add_auth_header(ctx);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::post
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::post(
        _In_ const const_narrow_string& resource,
        _Inout_ form_data&& form,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    // Note: The ASCII conversion would go via wchar_t anyway, so this is
    // basically for free.
    if (resource == nullptr) {
        auto r = static_cast<wchar_t *>(nullptr);
        return this->post(r, std::move(form), on_response, on_error, context);
    } else {
        auto r = convert<wchar_t>(resource);
        return this->post(r.c_str(), std::move(form), on_response, on_error,
            context);
    }
}


/*
 * visus::dataverse::dataverse_connection::post
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::post(
        _In_opt_z_ const wchar_t *resource,
        _In_reads_bytes_(cnt) const byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ const data_deleter_type data_deleter,
        _In_opt_z_ const wchar_t *content_type,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    _CHECK_ON_RESPONSE;
    _CHECK_ON_ERROR;

    if (data == nullptr) {
        throw std::invalid_argument("The data to be uploaded must be valid.");
    }

    auto& i = this->check_not_disposed();

    // Prepare the request.
    auto ctx = detail::io_context::create(i.make_url(resource),
        on_response, on_error, context);
    assert(ctx->client_data == context);

    detail::dataverse_connection_impl::check_code(::curl_easy_setopt(
        ctx->curl.get(), CURLOPT_UPLOAD, 1L));
    detail::dataverse_connection_impl::check_code(::curl_easy_setopt(
        ctx->curl.get(), CURLOPT_POST, 1L));
    detail::dataverse_connection_impl::check_code(::curl_easy_setopt(
        ctx->curl.get(), CURLOPT_INFILESIZE_LARGE, cnt));

    // Move the data to the context.
    ctx->prepare_request(data, cnt, data_deleter);

    // Set the HTTP headers.
    i.add_auth_header(ctx);
    ctx->content_type(content_type);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::post
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::post(
        _In_ const const_narrow_string& resource,
        _In_reads_bytes_(cnt) const byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ const data_deleter_type data_deleter,
        _In_ const const_narrow_string& content_type,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto c = (content_type != nullptr)
        ? convert<wchar_t>(content_type)
        : std::wstring();
    const auto r = (resource != nullptr)
        ? convert<wchar_t>(resource)
        : std::wstring();

    return this->post((resource != nullptr) ? r.c_str() : nullptr,
        data, cnt, data_deleter,
        (content_type != nullptr) ? c.c_str() : nullptr,
        on_response,
        on_error,
        context);
}


/*
 * visus::dataverse::dataverse_connection::put
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::put(
        _In_opt_z_ const wchar_t *resource,
        _In_reads_bytes_(cnt) const byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ const data_deleter_type data_deleter,
        _In_opt_z_ const wchar_t *content_type,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    _CHECK_ON_RESPONSE;
    _CHECK_ON_ERROR;

    if (data == nullptr) {
        throw std::invalid_argument("The data to be uploaded must be valid.");
    }

    auto& i = this->check_not_disposed();

    // Prepare the request.
    auto ctx = detail::io_context::create(i.make_url(resource),
        on_response, on_error, context);
    assert(ctx->client_data == context);

    detail::dataverse_connection_impl::check_code(::curl_easy_setopt(
        ctx->curl.get(), CURLOPT_UPLOAD, 1L));
    detail::dataverse_connection_impl::check_code(::curl_easy_setopt(
        ctx->curl.get(), CURLOPT_PUT, 1L));
    detail::dataverse_connection_impl::check_code(::curl_easy_setopt(
        ctx->curl.get(), CURLOPT_INFILESIZE_LARGE, cnt));

    // Move the data to the context.
    ctx->prepare_request(data, cnt, data_deleter);

    // Set the HTTP headers.
    i.add_auth_header(ctx);
    ctx->content_type(content_type);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));

    return *this;
}


/*
 * visus::dataverse::dataverse_connection::put
 */
visus::dataverse::dataverse_connection &
visus::dataverse::dataverse_connection::put(
        _In_ const const_narrow_string& resource,
        _In_reads_bytes_(cnt) const byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ const data_deleter_type data_deleter,
        _In_ const const_narrow_string& content_type,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto c = (content_type != nullptr)
        ? convert<wchar_t>(content_type)
        : std::wstring();
    const auto r = (resource != nullptr)
        ? convert<wchar_t>(resource)
        : std::wstring();

    return this->post((resource != nullptr) ? r.c_str() : nullptr,
        data, cnt, data_deleter,
        (content_type != nullptr) ? c.c_str() : nullptr,
        on_response,
        on_error,
        context);
}


/*
 * visus::dataverse::dataverse_connection::upload
 */
visus::dataverse::dataverse_connection &
visus::dataverse::dataverse_connection::upload(
        _In_z_ const wchar_t *persistent_id,
        _In_z_ const wchar_t *path,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::wstring(L"/datasets/:persistentId/add?"
        L"persistentId=") + persistent_id;
    return this->post(url.c_str(),
        std::move(this->make_form().add_file(L"file", path)),
        on_response,
        on_error,
        context);
}


/*
 * visus::dataverse::dataverse_connection::upload
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::upload(
        _In_ const const_narrow_string& persistent_id,
        _In_ const const_narrow_string& path,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::string("/datasets/:persistentId/add?"
        "persistentId=") + to_ascii(persistent_id);
    return this->post(const_narrow_string(url.c_str(), CP_ACP),
#if defined(_WIN32)
        std::move(this->make_form().add_file(narrow_string("file", CP_OEMCP),
            path)),
#else /* defined(_WIN32) */
        std::move(this->make_form().add_file(narrow_string("file", nullptr),
            path)),
#endif /* defined(_WIN32) */
        on_response,
        on_error,
        context);
}


/*
 * visus::dataverse::dataverse_connection::upload
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::upload(
        _In_z_ const wchar_t *persistent_id,
        _In_z_ const wchar_t *path,
        _In_z_ const wchar_t *description,
        _In_z_ const wchar_t *directory,
        const wchar_t **categories,
        _In_ const std::size_t cnt_cats,
        _In_ const bool restricted,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    // Note: We know that we do not use anything but 7-bit ASCII in the labels
    // here, so we do not need to add a UTF-8 conversion for the names.
    const auto url = std::wstring(L"/datasets/:persistentId/add?"
        L"persistentId=") + persistent_id;

    auto json = nlohmann::json::object({
        { "description", to_utf8(description) },
        { "directoryLabel", to_utf8(directory) },
        { "restrict", restricted },
        { "categories", nlohmann::json::array() }
    });

    if (categories != nullptr) {
        auto& cats = json["categories"];
        for (std::size_t i = 0; i < cnt_cats; ++i) {
            cats.push_back(to_utf8(categories[i]));
        }
    }

    const auto dump = json.dump();
    const auto desc = reinterpret_cast<const std::uint8_t *>(dump.data());
    const auto size = dump.size();

    return this->post(url.c_str(),
        std::move(this->make_form()
            .add_file(L"file", path)
            .add_field(L"jsonData", desc, size)),
        on_response,
        on_error,
        context);
}


/*
 * visus::dataverse::dataverse_connection::upload
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::upload(
        _In_ const const_narrow_string& persistent_id,
        _In_ const const_narrow_string& path,
        _In_ const const_narrow_string& description,
        _In_ const const_narrow_string& directory,
        _In_reads_opt_(cnt_cats) const const_narrow_string *categories,
        _In_ const std::size_t cnt_cats,
        _In_ const bool restricted,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    auto i = convert<wchar_t>(persistent_id);
    auto p = convert<wchar_t>(path);
    auto d = convert<wchar_t>(description);
    auto f = convert<wchar_t>(directory);

    std::vector<std::wstring> cats;
    if (categories != nullptr) {
        cats.reserve(cnt_cats);
        std::transform(categories,
            categories + cnt_cats,
            std::back_inserter(cats),
            [](const const_narrow_string& s) { return convert<wchar_t>(s); });
    }

    std::vector<const wchar_t *> cat_ptrs;
    cat_ptrs.reserve(cats.size());
    std::transform(cats.begin(),
        cats.end(),
        std::back_inserter(cat_ptrs),
        [](const std::wstring& s ) { return s.c_str(); });

    return this->upload(i.c_str(),
        p.c_str(),
        d.c_str(),
        f.c_str(),
        cat_ptrs.data(),
        cat_ptrs.size(),
        restricted,
        on_response,
        on_error,
        context);
}


#if defined(DATAVERSE_WITH_JSON)
/*
 * visus::dataverse::dataverse_connection::upload
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::upload(
        _In_ const const_narrow_string& persistent_id,
        _In_ const const_narrow_string& path,
        _In_ const nlohmann::json& description,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::wstring(L"/datasets/:persistentId/add?"
        L"persistentId=") + convert<wchar_t>(persistent_id);
    const auto dump = description.dump();
    const auto d = reinterpret_cast<const std::uint8_t *>(dump.data());
    const auto s = dump.size();
    const auto p = convert<wchar_t>(path);
    return this->post(url.c_str(),
        std::move(this->make_form()
            .add_file(L"file", p.c_str())
            .add_field(L"jsonData", d, s)),
        on_response,
        on_error,
        context);
}
#endif /* defined(DATAVERSE_WITH_JSON) */


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

#undef _CHECK_ON_RESPONSE
#undef _CHECK_ON_ERROR
#undef _DATAVERSE_HASH_ALGORITHMA
#undef _DATAVERSE_HASH_ALGORITHMW
