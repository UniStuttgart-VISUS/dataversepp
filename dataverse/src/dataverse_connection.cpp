// <copyright file="dataverse_connection.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <nlohmann/json.hpp>
#include "dataverse/dataverse_connection.h"

#include <algorithm>
#include <cassert>
#include <iterator>

#if defined(_WIN32)
#include <Windows.h>
#else /* defined(_WIN32) */
#include <fcntl.h>
#endif /* defined(_WIN32) */

#include "dataverse_connection_impl.h"
#include "direct_upload_context.h"
#include "file_properties.h"
#include "io_context.h"


#define _CHECK_API_ON_RESPONSE if (on_api_response == nullptr) \
    throw std::invalid_argument("The response handler must be valid.")

#define _CHECK_ON_RESPONSE if (on_response == nullptr) \
    throw std::invalid_argument("The response handler must be valid.")

#define _CHECK_ON_ERROR if (on_error == nullptr) \
    throw std::invalid_argument("The error handler must be valid.")

#define _DATAVERSE_HASH_ALGORITHMA "MD5"

#define _DATAVERSE_HASH_ALGORITHMW L"MD5"


/*
 * visus::dataverse::dataverse_connection::draught_version
 */
const wchar_t *const visus::dataverse::dataverse_connection::draught_version
    = L":draft";


/*
 * visus::dataverse::dataverse_connection::latest_published_version
 */
const wchar_t *const
visus::dataverse::dataverse_connection::latest_published_version
    = L":latest-published";


/*
 * visus::dataverse::dataverse_connection::latest_version
 */
const wchar_t *const visus::dataverse::dataverse_connection::latest_version
    = L":latest";


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
visus::dataverse::const_narrow_string
visus::dataverse::dataverse_connection::base_path(void) const {
    return make_narrow_string(this->check_not_disposed().base_path,
#if defined(_WIN32)
        CP_ACP
#else /* defined(_WIN32) */
        "ASCII"
#endif /* defined(_WIN32)*/
        );
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
 * visus::dataverse::dataverse_connection::data_set
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::data_set(
        _In_z_ const wchar_t *persistent_id,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::wstring(L"/datasets/:persistentId/?"
        L"persistentId=") + persistent_id;
    this->get(url.c_str(), on_response, nullptr, on_error, context);
    return *this;
}


/*
 * visus::dataverse::dataverse_connection::data_set
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::data_set(
        _In_ const const_narrow_string& persistent_id,
        _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::wstring(L"/datasets/:persistentId/?"
        L"persistentId=") + convert<wchar_t>(persistent_id);
    this->get(url.c_str(), on_response, nullptr, on_error, context);
    return *this;
}


/*
 * visus::dataverse::dataverse_connection::download
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::download(_In_ const std::uint64_t id,
        _In_z_ const wchar_t *format,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::wstring(L"/access/datafile/") + std::to_wstring(id)
        + std::wstring(L"?format=") + std::wstring(format);
    this->get(url.c_str(), on_response, nullptr, on_error, context);
    return *this;
}


/*
 * visus::dataverse::dataverse_connection::download
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::download(_In_ const std::uint64_t id,
        _In_ const const_narrow_string& format,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::wstring(L"/access/datafile/") + std::to_wstring(id)
        + std::wstring(L"?format=") + convert<wchar_t>(format);
    this->get(url.c_str(), on_response, nullptr, on_error, context);
    return *this;
}


/*
 * visus::dataverse::dataverse_connection::files
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::files(_In_z_ const std::uint64_t id,
        _In_z_ const wchar_t *version,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::wstring(L"/datasets/") + std::to_wstring(id)
        + std::wstring(L"/versions/") + version
        + std::wstring(L"/files");
    this->get(url.c_str(), on_response, nullptr, on_error, context);
    return *this;
}


/*
 * visus::dataverse::dataverse_connection::download
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::download(
        _In_z_ const wchar_t *persistent_id,
        _In_z_ const wchar_t *format,
        _In_z_ const wchar_t *version,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::wstring(L"/access/datafile/:persistentId"
        L"?persistentId=") + std::wstring(persistent_id)
        + std::wstring(L"&version=") + std::wstring(version)
        + std::wstring(L"&format=") + std::wstring(format);
    this->get(url.c_str(), on_response, nullptr, on_error, context);
    return *this;
}


/*
 * visus::dataverse::dataverse_connection::download
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::download(
        _In_ const const_narrow_string& persistent_id,
        _In_ const const_narrow_string& format,
        _In_ const const_narrow_string& version,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::wstring(L"/access/datafile/:persistentId"
        L"?persistentId=") + convert<wchar_t>(persistent_id)
        + std::wstring(L"&version=") + convert<wchar_t>(version)
        + std::wstring(L"&format=") + convert<wchar_t>(format);
    this->get(url.c_str(), on_response, nullptr, on_error, context);
    return *this;
}


/*
 * visus::dataverse::dataverse_connection::files
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::files(_In_ const std::uint64_t id,
        _In_ const const_narrow_string& version,
        _In_ const on_response_type on_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto url = std::wstring(L"/datasets/") + std::to_wstring(id)
        + std::wstring(L"/versions/") + convert<wchar_t>(version)
        + std::wstring(L"/files");
    this->get(url.c_str(), on_response, nullptr, on_error, context);
    return *this;
}


/*
 * visus::dataverse::dataverse_connection::io_timeout
 */
visus::dataverse::dataverse_connection&
visus::dataverse::dataverse_connection::io_timeout(_In_ const int millis) {
    this->check_not_disposed().timeout = (std::max)(0, millis);
    return *this;
}


/*
 * visus::dataverse::dataverse_connection::io_timeout
 */
int visus::dataverse::dataverse_connection::io_timeout(void) const {
    return this->check_not_disposed().timeout;
}


/*
 * visus::dataverse::dataverse_connection::make_form
 */
visus::dataverse::form_data visus::dataverse::dataverse_connection::make_form(
        void) const {
    return form_data(detail::dataverse_connection_impl::make_curl().release());
}


/*
 * visus::dataverse::dataverse_connection::upload
 */
visus::dataverse::dataverse_connection&
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
 * visus::dataverse::dataverse_connection::get_api_response_client_data
 */
void *visus::dataverse::dataverse_connection::get_api_response_client_data(
        _In_ void *client_data) {
    auto context = static_cast<detail::io_context *>(client_data);
    return context->api_data;
}


/*
 * visus::dataverse::dataverse_connection::get_on_api_response
 */
void *visus::dataverse::dataverse_connection::get_on_api_response(
        _In_ void *client_data) {
    auto context = static_cast<detail::io_context *>(client_data);
    return context->on_api_response;
}


/*
 * visus::dataverse::dataverse_connection::get_on_error
 */
visus::dataverse::dataverse_connection::on_error_type
visus::dataverse::dataverse_connection::get_on_error(_In_ void *client_data) {
    auto context = static_cast<detail::io_context *>(client_data);
    return context->on_error;
}


/*
 * visus::dataverse::dataverse_connection::report_api_error
 */
void visus::dataverse::dataverse_connection::report_api_error(
        _In_ void *client_data, _In_z_ const char *error) {
    auto context = static_cast<detail::io_context *>(client_data);
    detail::invoke_handler(context->on_error, error, "API", utf8_code_page,
        context->client_data);
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


/*
 * visus::dataverse::dataverse_connection::delete_resource
 */
void visus::dataverse::dataverse_connection::delete_resource(
        _In_opt_z_ const wchar_t *resource,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    _CHECK_ON_RESPONSE;
    _CHECK_ON_ERROR;

    auto &i = this->check_not_disposed();

    // Prepare the request.
    auto ctx = detail::io_context::create(i.make_url(resource), on_response,
        on_error, context);
    ctx->option(CURLOPT_CUSTOMREQUEST, "DELETE");
    ctx->configure_on_api_response(const_cast<void *>(on_api_response));

    // Set the authentication header.
    i.add_auth_header(ctx);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));
}


/*
 * visus::dataverse::dataverse_connection::delete_resource
 */
void visus::dataverse::dataverse_connection::delete_resource(
        _In_ const const_narrow_string &resource,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    // Note: The ASCII conversion would go via wchar_t anyway, so this is
    // basically for free.
    if (resource == nullptr) {
        auto r = static_cast<wchar_t *>(nullptr);
        return this->delete_resource(r, on_response, on_api_response,
            on_error, context);
    } else {
        auto r = convert<wchar_t>(resource);
        return this->delete_resource(r.c_str(), on_response,
            on_api_response, on_error, context);
    }
}


/*
 * visus::dataverse::dataverse_connection::direct_upload
 */
void visus::dataverse::dataverse_connection::direct_upload(
        _In_z_ const wchar_t *persistent_id,
        _In_z_ const wchar_t *path,
        _In_opt_z_ const wchar_t *mime_type,
        _In_z_ const wchar_t *description,
        _In_z_ const wchar_t *directory,
        _In_reads_z_(cnt_cats) const wchar_t **categories,
        _In_ const std::size_t cnt_cats,
        _In_ const bool restricted,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    using detail::direct_upload_context;
    _CHECK_ON_RESPONSE;
    _CHECK_ON_ERROR;

    // TODO: on_api_response must be handled.

    // This is performed once the request for the one-time upload URL succeeded.
    const auto on_upload_url = [](const blob& r, void *u) {
        auto ctx = static_cast<direct_upload_context *>(u);
        ctx->handle_errors([ctx, &r](void) {
            // Retrieve the upload URL from the response and store the storage
            // identifier from the response in the context.
            const auto url = ctx->upload_url(r);
            const auto size = ctx->description["fileSize"].get<std::uint64_t>();

            // Create an I/O context for posting the continuation.
            auto c = detail::io_context::create(url,
                    [](const blob& r, void *u) {
                auto ctx = static_cast<direct_upload_context *>(u);
                ctx->handle_errors([ctx](void) {
                    const auto desc = ctx->description.dump();

                    // Create the final request, which uses the user-facing
                    // callbacks directly.
                    auto c = detail::io_context::create(ctx->registration_url,
                        ctx->on_response, ctx->on_error, ctx->user_context);

                    // Add the previously compiled JSON data to the request.
                    c->form = form_data(c->curl.get());
                    c->form._curl = nullptr;    // Owned by context!
                    c->form.add_field("jsonData", desc.c_str());
                    c->option(CURLOPT_MIMEPOST, c->form._form);

                    // This time, we need the Dataverse authentication.
                    ctx->connection->add_auth_header(c);
                    c->apply_headers();

                    // Post the final request.
                    ctx->connection->process(std::move(c));

                    // We do not need the context anymore. Note that
                    // 'handle_errors' will delete it in case of an exception
                    // in the code above, so we do not have to care about this.
                    delete ctx;
                });
            }, direct_upload_context::forward_error, ctx);

            // This one is special, because it is an S3 request and not a
            // Dataverse API call.
            c->option(CURLOPT_UPLOAD, 1L);
            c->option(CURLOPT_PUT, 1L);
            c->option(CURLOPT_INFILESIZE_LARGE, size);
#if defined(_WIN32)
            c->option(CURLOPT_READFUNCTION, form_data::win32_read);
#else /* defined(_WIN32) */
            c->option(CURLOPT_READFUNCTION, form_data::posix_read);
#endif /* defined(_WIN32) */
            c->option(CURLOPT_READDATA, ctx->file.get());
#if defined(_WIN32)
            c->option(CURLOPT_SEEKFUNCTION, form_data::win32_seek);
#else /* defined(_WIN32) */
            c->option(CURLOPT_SEEKFUNCTION, form_data::posix_seek);
#endif /* defined(_WIN32) */
            c->option(CURLOPT_SEEKDATA, ctx->file.get());

            c->add_header("x-amz-tagging: dv-state=temp");
            c->apply_headers();

            ctx->connection->process(std::move(c));
        });
    };

    // Allocate the upload context which helps us tracking the progress through
    // the individual stages of the process. From here on, the context must be
    // successfully passed to the API or freed in case of any error, wherefore
    // the following code must be enclosed in try/catch.
    auto ctx = new direct_upload_context(this->_impl, on_response, on_error,
        context);

    try {
        // This is the variant where we read ourselves, so we need to open the
        // file and remember the handle in the context.
        ctx->file = detail::io_context::open_file(path);

        // Prepare as much of the description as we can right now.
        ctx->description = nlohmann::json::object({
            { "description", to_utf8(description) },
            { "directoryLabel", to_utf8(directory) },
            { "restrict", restricted },
            { "categories", nlohmann::json::array() },
        });

        if (categories != nullptr) {
            auto& cats = ctx->description["categories"];
            for (std::size_t i = 0; i < cnt_cats; ++i) {
                cats.push_back(to_utf8(categories[i]));
            }
        }

        // Note a MIME type is required for direct uploads, so we use the most
        // generic one (bytes) if no valid input is given.
        if (mime_type != nullptr) {
            ctx->description["mimeType"] = to_utf8(mime_type);
        } else {
            ctx->description["mimeType"] = to_utf8(L"application/octet-stream");
        }

        // Add metadata about the file itself (size, name, etc.).
        ctx->description.update(detail::get_file_properties(path));

        // The URL we will later use to register the metadata.
        ctx->registration_url = this->_impl->make_url(std::wstring(
            L"/datasets/:persistentId/add?persistentId=")
            + persistent_id);

        // Begin the chain of operations by retrieving the upload URL.
        const auto url = std::wstring(L"/datasets/:persistentId/uploadsid/"
            L"?persistentId=") + persistent_id;
        this->get(url.c_str(), on_upload_url,
            direct_upload_context::forward_error, ctx);
    } catch (...) {
        delete ctx;
        throw;
    }
}


/*
 * visus::dataverse::dataverse_connection::direct_upload
 */
void visus::dataverse::dataverse_connection::direct_upload(
        _In_ const const_narrow_string& persistent_id,
        _In_ const const_narrow_string& path,
        _In_ const const_narrow_string& mime_type,
        _In_ const const_narrow_string& description,
        _In_ const const_narrow_string& directory,
        _In_reads_opt_(cnt_cats) const const_narrow_string *categories,
        _In_ const std::size_t cnt_cats,
        _In_ const bool restricted,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    auto i = convert<wchar_t>(persistent_id);
    auto p = convert<wchar_t>(path);
    auto m = convert<wchar_t>(mime_type);
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

    this->direct_upload(i.c_str(),
        p.c_str(),
        m.c_str(),
        d.c_str(),
        f.c_str(),
        cat_ptrs.data(),
        cat_ptrs.size(),
        restricted,
        on_response,
        on_api_response,
        on_error,
        context);
}


/*
 * visus::dataverse::dataverse_connection::get
 */
void visus::dataverse::dataverse_connection::get(
        _In_opt_z_ const wchar_t *resource,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
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
    ctx->configure_on_api_response(const_cast<void *>(on_api_response));

    // Set the authentication header.
    i.add_auth_header(ctx);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));
}


/*
 * visus::dataverse::dataverse_connection::get
 */
void visus::dataverse::dataverse_connection::get(
        _In_ const const_narrow_string& resource,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    // Note: The ASCII conversion would go via wchar_t anyway, so this is
    // basically for free.
    if (resource == nullptr) {
        auto r = static_cast<wchar_t *>(nullptr);
        return this->get(r, on_response, on_api_response, on_error, context);
    } else {
        auto r = convert<wchar_t>(resource);
        return this->get(r.c_str(), on_response, on_api_response, on_error,
            context);
    }
}


/*
 * visus::dataverse::dataverse_connection::post
 */
void visus::dataverse::dataverse_connection::post(
        _In_opt_z_ const wchar_t *resource,
        _In_z_ const wchar_t *path,
        _In_opt_z_ const wchar_t *content_type,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    _CHECK_ON_RESPONSE;
    _CHECK_ON_ERROR;

    if (path == nullptr) {
        throw std::invalid_argument("The file to be uploaded must be valid.");
    }

    auto& i = this->check_not_disposed();

    // Prepare the request.
    auto ctx = detail::io_context::create(i.make_url(resource),
        on_response, on_error, context);
    assert(ctx->client_data == context);
    ctx->configure_on_api_response(const_cast<void *>(on_api_response));

    ctx->option(CURLOPT_UPLOAD, 1L);
    ctx->option(CURLOPT_POST, 1L);
    ctx->prepare_request(path);

    // Set the HTTP headers.
    i.add_auth_header(ctx);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));
}


/*
 * visus::dataverse::dataverse_connection::post
 */
void visus::dataverse::dataverse_connection::post(
        _In_ const const_narrow_string& resource,
        _In_ const const_narrow_string& path,
        _In_ const const_narrow_string& content_type,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto r = (resource != nullptr)
        ? convert<wchar_t>(resource)
        : std::wstring();
    const auto p = (path != nullptr)
        ? convert<wchar_t>(path)
        : std::wstring();
    const auto c = (content_type != nullptr)
        ? convert<wchar_t>(content_type)
        : std::wstring();

    this->post((resource != nullptr) ? r.c_str() : nullptr,
        (path != nullptr) ? p.c_str() : nullptr,
        (content_type != nullptr) ? c.c_str() : nullptr,
        on_response,
        on_api_response,
        on_error,
        context);
}


/*
 * visus::dataverse::dataverse_connection::post
 */
void visus::dataverse::dataverse_connection::post(
        _In_opt_z_ const wchar_t *resource,
        _Inout_ form_data&& form,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
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
    ctx->configure_on_api_response(const_cast<void *>(on_api_response));

    // Context has taken ownership of CURL object, so erase it from form.
    form._curl = nullptr;

    ctx->option(CURLOPT_MIMEPOST, ctx->form._form);

    // Set the authentication header.
    i.add_auth_header(ctx);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));
}


/*
 * visus::dataverse::dataverse_connection::post
 */
void visus::dataverse::dataverse_connection::post(
        _In_ const const_narrow_string &resource,
        _Inout_ form_data &&form,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    _CHECK_ON_RESPONSE;
    _CHECK_ON_ERROR;

    // Note: The ASCII conversion would go via wchar_t anyway, so this is
    // basically for free.
    if (resource == nullptr) {
        auto r = static_cast<wchar_t *>(nullptr);
        return this->post(r, std::move(form), on_response, on_api_response,
            on_error, context);
    } else {
        auto r = convert<wchar_t>(resource);
        return this->post(r.c_str(), std::move(form), on_response,
            on_api_response, on_error, context);
    }
}


/*
 * visus::dataverse::dataverse_connection::post
 */
void visus::dataverse::dataverse_connection::post(
        _In_opt_z_ const wchar_t *resource,
        _In_reads_bytes_(cnt) const byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ const data_deleter_type data_deleter,
        _In_opt_z_ const wchar_t *content_type,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
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
    ctx->configure_on_api_response(const_cast<void *>(on_api_response));

    ctx->option(CURLOPT_UPLOAD, 1L);
    ctx->option(CURLOPT_POST, 1L);
    ctx->option(CURLOPT_INFILESIZE_LARGE, cnt);

    // Move the data to the context.
    ctx->prepare_request(data, cnt, data_deleter);

    // Set the HTTP headers.
    i.add_auth_header(ctx);
    ctx->content_type(content_type);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));
}


/*
 * visus::dataverse::dataverse_connection::post
 */
void visus::dataverse::dataverse_connection::post(
        _In_ const const_narrow_string& resource,
        _In_reads_bytes_(cnt) const byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ const data_deleter_type data_deleter,
        _In_ const const_narrow_string& content_type,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto c = (content_type != nullptr)
        ? convert<wchar_t>(content_type)
        : std::wstring();
    const auto r = (resource != nullptr)
        ? convert<wchar_t>(resource)
        : std::wstring();

    this->post((resource != nullptr) ? r.c_str() : nullptr,
        data, cnt, data_deleter,
        (content_type != nullptr) ? c.c_str() : nullptr,
        on_response,
        on_api_response,
        on_error,
        context);
}


/*
 * visus::dataverse::dataverse_connection::put
 */
void visus::dataverse::dataverse_connection::put(
        _In_opt_z_ const wchar_t *resource,
        _In_z_ const wchar_t *path,
        _In_opt_z_ const wchar_t *content_type,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    _CHECK_ON_RESPONSE;
    _CHECK_ON_ERROR;

    if (path == nullptr) {
        throw std::invalid_argument("The file to be uploaded must be valid.");
    }

    auto& i = this->check_not_disposed();

    // Prepare the request.
    auto ctx = detail::io_context::create(i.make_url(resource),
        on_response, on_error, context);
    assert(ctx->client_data == context);
    ctx->configure_on_api_response(const_cast<void *>(on_api_response));

    ctx->option(CURLOPT_UPLOAD, 1L);
    ctx->option(CURLOPT_PUT, 1L);
    ctx->prepare_request(path);

    // Set the HTTP headers.
    i.add_auth_header(ctx);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));
}


/*
 * visus::dataverse::dataverse_connection::put
 */
void visus::dataverse::dataverse_connection::put(
        _In_ const const_narrow_string& resource,
        _In_ const const_narrow_string& path,
        _In_ const const_narrow_string& content_type,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto r = (resource != nullptr)
        ? convert<wchar_t>(resource)
        : std::wstring();
    const auto p = (path != nullptr)
        ? convert<wchar_t>(path)
        : std::wstring();
    const auto c = (content_type != nullptr)
        ? convert<wchar_t>(content_type)
        : std::wstring();

    this->put((resource != nullptr) ? r.c_str() : nullptr,
        (path != nullptr) ? p.c_str() : nullptr,
        (content_type != nullptr) ? c.c_str() : nullptr,
        on_response,
        on_api_response,
        on_error,
        context);
}

/*
 * visus::dataverse::dataverse_connection::put
 */
void visus::dataverse::dataverse_connection::put(
        _In_opt_z_ const wchar_t *resource,
        _In_reads_bytes_(cnt) const byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ const data_deleter_type data_deleter,
        _In_opt_z_ const wchar_t *content_type,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
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
    ctx->configure_on_api_response(const_cast<void *>(on_api_response));

    ctx->option(CURLOPT_UPLOAD, 1L);
    ctx->option(CURLOPT_PUT, 1L);
    ctx->option(CURLOPT_INFILESIZE_LARGE, cnt);

    // Move the data to the context.
    ctx->prepare_request(data, cnt, data_deleter);

    // Set the HTTP headers.
    i.add_auth_header(ctx);
    ctx->content_type(content_type);
    ctx->apply_headers();

    // Send the request to asynchronous processing.
    i.process(std::move(ctx));
}

/*
 * visus::dataverse::dataverse_connection::put
 */
void visus::dataverse::dataverse_connection::put(
        _In_ const const_narrow_string& resource,
        _In_reads_bytes_(cnt) const byte_type *data,
        _In_ const std::size_t cnt,
        _In_opt_ const data_deleter_type data_deleter,
        _In_ const const_narrow_string& content_type,
        _In_ const on_response_type on_response,
        _In_opt_ const void *on_api_response,
        _In_ const on_error_type on_error,
        _In_opt_ void *context) {
    const auto c = (content_type != nullptr)
        ? convert<wchar_t>(content_type)
        : std::wstring();
    const auto r = (resource != nullptr)
        ? convert<wchar_t>(resource)
        : std::wstring();

    this->put((resource != nullptr) ? r.c_str() : nullptr,
        data, cnt, data_deleter,
        (content_type != nullptr) ? c.c_str() : nullptr,
        on_response,
        on_api_response,
        on_error,
        context);
}

#undef _CHECK_API_ON_RESPONSE
#undef _CHECK_ON_RESPONSE
#undef _CHECK_ON_ERROR
#undef _DATAVERSE_HASH_ALGORITHMA
#undef _DATAVERSE_HASH_ALGORITHMW
