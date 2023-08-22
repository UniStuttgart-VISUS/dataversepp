// <copyright file="dataverse_connection.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>
#include <cstring>
#include <future>
#include <stdexcept>
#include <string>
#include <vector>

#include "dataverse/blob.h"
#include "dataverse/convert.h"
#include "dataverse/event.h"
#include "dataverse/form_data.h"
#include "dataverse/json.h"


namespace visus {
namespace dataverse {

    /* Forward declarations. */
    namespace detail { class dataverse_connection_impl; }

    /// <summary>
    /// Represents the connection to the Dataverse and stores the API path
    /// and the authentication token for the requests to make.
    /// </summary>
    class DATAVERSE_API dataverse_connection {

    public:

        /// <summary>
        /// The type used to represent a single byte.
        /// </summary>
        typedef blob::byte_type byte_type;

        /// <summary>
        /// The type of a deleter callback that allows the caller to have the
        /// API free data that it passed to it when this data is no longer
        /// needed.
        /// </summary>
        typedef void (*data_deleter_type)(_In_opt_ const void *);

#if defined(DATAVERSE_WITH_JSON)
        /// <summary>
        /// The callback to be invoked for a parsed API response.
        /// </summary>
        typedef void (*on_api_response_type(_In_ const nlohmann::json&,
            _In_opt_ void *));
#endif /* defined(DATAVERSE_WITH_JSON) */

        /// <summary>
        /// The callback to be invoked for a raw response.
        /// </summary>
        typedef void (*on_response_type)(_In_ const blob&,
            _In_opt_ void *);

        /// <summary>
        /// The callback to be invoked for an error.
        /// </summary>
        typedef void (*on_error_type)(_In_ const int,
            _In_z_ const char *,
            _In_z_ const char *,
            _In_ const narrow_string::code_page_type,
            _In_opt_ void *);

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        dataverse_connection(void);

        /// <summary>
        /// Initialise from move.
        /// </summary>
        /// <param name="rhs">The object to be moved.</param>
        dataverse_connection(_Inout_ dataverse_connection&& rhs) noexcept;

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~dataverse_connection(void);

        /// <summary>
        /// Sets a new API key to authenticate with Dataverse.
        /// </summary>
        /// <param name="api_key">The API key to use. It is safe to pass
        /// <c>nullptr</c>, in which case the object tries to make
        /// unauthenticated requests.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to store the
        /// data could not be alloctated.</exception>
        dataverse_connection& api_key(_In_opt_z_ const wchar_t *api_key);

        /// <summary>
        /// Sets a new API key to authenticate with Dataverse.
        /// </summary>
        /// <param name="api_key">The API key to use. It is safe to pass
        /// <c>nullptr</c>, in which case the object tries to make
        /// unauthenticated requests.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to store the
        /// data could not be alloctated.</exception>
        dataverse_connection& api_key(_In_ const const_narrow_string& api_key);

        /// <summary>
        /// Sets the base path to the API end point such that you do not have to
        /// specify the common part with every request.
        /// </summary>
        /// <remarks>
        /// <para>Certain convenience overloads to interactive with the API
        /// cannot be used without setting the correct base path.</para>
        /// </remarks>
        /// <param name="base_path">The base path to prepend to every request.
        /// It is safe to pass <c>nullptr</c>.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to store the
        /// data could not be alloctated.</exception>
        dataverse_connection& base_path(_In_opt_z_ const wchar_t *base_path);

        /// <summary>
        /// Sets the base path to the API end point such that you do not have to
        /// specify the common part with every request.
        /// </summary>
        /// <remarks>
        /// <para>Certain convenience overloads to interactive with the API
        /// cannot be used without setting the correct base path.</para>
        /// </remarks>
        /// <param name="base_path">The base path to prepend to every request.
        /// It is safe to pass <c>nullptr</c>.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to store the
        /// data could not be alloctated.</exception>
        dataverse_connection& base_path(
            _In_ const const_narrow_string& base_path);

        dataverse_connection& direct_upload(_In_z_ const wchar_t *persistent_id,
            _In_z_ const wchar_t *path,
            _In_z_ const wchar_t *description,
            _In_z_ const wchar_t *directory,
            const wchar_t **categories,
            _In_ const std::size_t cnt_cats,
            _In_ const bool restricted,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Retrieves the resource at the specified location using a GET
        /// request.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& get(_In_opt_z_ const wchar_t *resource,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Asynchronously retrieves the resource at the specified location
        /// using a GET request and provides a future for the result.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <returns>A future for the result of the opration.</returns>
        inline std::future<blob> get(_In_ const std::wstring& resource) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const wchar_t *, const on_response_type, const on_error_type,
                void *);
            return invoke_async(
                static_cast<actual_type>(&dataverse_connection::get),
                *this, resource.c_str());
        }

        /// <summary>
        /// Retrieves the resource at the specified location using a GET
        /// request.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& get(_In_ const const_narrow_string& resource,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Asynchronously retrieves the resource at the specified location
        /// using a GET request and provides a future for the result.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <returns>A future for the result of the opration.</returns>
        inline std::future<blob> get(_In_ const const_narrow_string& resource) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&, const on_response_type,
                const on_error_type, void *);
            return invoke_async(
                static_cast<actual_type>(&dataverse_connection::get),
                *this, resource);
        }

        /// <summary>
        /// Create a new and empty form for a POST request.
        /// </summary>
        /// <returns></returns>
        form_data make_form(void) const;

        /// <summary>
        /// Posts the specified form to the specified resource location.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="form">The form data to post. The connection object
        /// takes ownership of the form.</param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& post(_In_opt_z_ const wchar_t *resource,
            _Inout_ form_data&& form,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Posts the specified form to the specified resource location and
        /// returns a future for the result.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="form">The form data to post. The connection object
        /// takes ownership of the form.</param>
        /// <returns>A future for the result of the opration.</returns>
        inline std::future<blob> post(_In_opt_z_ const wchar_t *resource,
                _Inout_ form_data&& form) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const wchar_t *, form_data&&, const on_response_type,
                const on_error_type, void *);
            return invoke_async(
                static_cast<actual_type>(&dataverse_connection::post),
                *this, resource, std::move(form));
        }

        /// <summary>
        /// Posts the specified form to the specified resource location.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="form">The form data to post. The connection object
        /// takes ownership of the form.</param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& post(_In_ const const_narrow_string& resource,
            _Inout_ form_data&& form,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Posts the specified form to the specified resource location and
        /// returns a future for the result.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="form">The form data to post.</param>
        /// <returns>A future for the result of the opration.</returns>
        inline std::future<blob> post(
                _In_opt_z_ const const_narrow_string& resource,
                _Inout_ form_data&& form) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&, form_data&&,
                const on_response_type, const on_error_type, void *);
            return invoke_async(
                static_cast<actual_type>(&dataverse_connection::post),
                *this, resource, std::move(form));
        }

        /// <summary>
        /// Posts the given data to the given resource location.
        /// </summary>
        /// <remarks>
        /// This method can be used to create data sets.
        /// </remarks>
        /// <param name="resource">The path to the resource to post. The
        /// base path configured will be prepended if set.</param>
        /// <param name="data">The data to post. The caller remains owner
        /// of this memory if no <see cref="data_deleter" /> is set and must
        /// make sure that the data remain valid until the request completed or
        /// failed.</param>
        /// <param name="cnt">The size of the data in bytes.</param>
        /// <param name="data_deleter">If not <c>nullptr</c>, the object will
        /// eventually free <paramref name="data" /> using this callback.
        /// </param>
        /// <param name="content_type">The MIME type of the data to be posted.
        /// </param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& post(_In_opt_z_ const wchar_t *resource,
            _In_reads_bytes_(cnt) const byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ const data_deleter_type data_deleter,
            _In_opt_z_ const wchar_t *content_type,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Posts the specified form to the specified resource location and
        /// returns a future for the result.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="data">The data to post. The caller remains owner
        /// of this memory if no <see cref="data_deleter" /> is set and must
        /// make sure that the data remain valid until the request completed or
        /// failed.</param>
        /// <param name="cnt">The size of the data in bytes.</param>
        /// <param name="data_deleter">If not <c>nullptr</c>, the object will
        /// eventually free <paramref name="data" /> using this callback.
        /// </param>
        /// <param name="content_type">The MIME type of the data to be posted.
        /// </param>
        /// <returns>A future for the result of the opration.</returns>
        inline std::future<blob> post(_In_opt_z_ const wchar_t *resource,
                _In_reads_bytes_(cnt) const byte_type *data,
                _In_ const std::size_t cnt,
                _In_opt_ const data_deleter_type data_deleter,
                _In_opt_z_ const wchar_t *content_type) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const wchar_t *, const byte_type *, const std::size_t,
                const data_deleter_type, const wchar_t *,
                const on_response_type, const on_error_type, void *);
            return invoke_async(
                static_cast<actual_type>(&dataverse_connection::post),
                *this, resource, data, cnt, data_deleter, content_type);
        }

        /// <summary>
        /// Posts the given data to the given resource location.
        /// </summary>
        /// <remarks>
        /// This method can be used to create data sets.
        /// </remarks>
        /// <param name="resource">The path to the resource to post. The
        /// base path configured will be prepended if set.</param>
        /// <param name="data">The data to post. The caller remains owner
        /// of this memory if no <see cref="data_deleter" /> is set and must
        /// make sure that the data remain valid until the request completed or
        /// failed.</param>
        /// <param name="cnt">The size of the data in bytes.</param>
        /// <param name="data_deleter">If not <c>nullptr</c>, the object will
        /// eventually free <paramref name="data" /> using this callback.
        /// </param>
        /// <param name="content_type">The MIME type of the data to be posted.
        /// </param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& post(_In_ const const_narrow_string& resource,
            _In_reads_bytes_(cnt) const byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ const data_deleter_type data_deleter,
            _In_ const const_narrow_string& content_type,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Posts the specified form to the specified resource location and
        /// returns a future for the result.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="data">The data to post. The caller remains owner
        /// of this memory if no <see cref="data_deleter" /> is set and must
        /// make sure that the data remain valid until the request completed or
        /// failed.</param>
        /// <param name="cnt">The size of the data in bytes.</param>
        /// <param name="data_deleter">If not <c>nullptr</c>, the object will
        /// eventually free <paramref name="data" /> using this callback.
        /// </param>
        /// <param name="content_type">The MIME type of the data to be posted.
        /// </param>
        /// <returns>A future for the result of the opration.</returns>
        inline std::future<blob> post(
                _In_opt_z_ const const_narrow_string& resource,
                _In_reads_bytes_(cnt) const byte_type *data,
                _In_ const std::size_t cnt,
                _In_opt_ const data_deleter_type data_deleter,
                _In_opt_z_ const const_narrow_string& content_type) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&, const byte_type *,
                const std::size_t, const data_deleter_type,
                const const_narrow_string&, const on_response_type,
                const on_error_type, void *);
            return invoke_async(
                static_cast<actual_type>(&dataverse_connection::post),
                *this, resource, data, cnt, data_deleter, content_type);
        }

#if defined(DATAVERSE_WITH_JSON)
        /// <summary>
        /// Posts the specified JSON data to the specified resource location.
        /// </summary>
        /// <remarks>
        /// This method can be used to create data sets.
        /// </remarks>
        /// <param name="resource">The path to the resource to post. The
        /// base path configured will be prepended if set.</param>
        /// <param name="json">The JSON object to post.</param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline dataverse_connection& post(_In_opt_z_ const wchar_t *resource,
                _In_ const nlohmann::json& json,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            auto dump = json.dump();
            auto octets = new byte_type[dump.size()];
            ::memcpy(octets, dump.data(), dump.size());
            return this->post(resource, octets, dump.size(),
                [](const void *o) { delete[] static_cast<const byte_type *>(o); },
                L"application/json", on_response, on_error, context);
        }

        /// <summary>
        /// Posts the specified JSON data to the specified resource location.
        /// </summary>
        /// <remarks>
        /// This method can be used to create data sets.
        /// </remarks>
        /// <param name="resource">The path to the resource to post. The
        /// base path configured will be prepended if set.</param>
        /// <param name="json">The JSON object to post.</param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline dataverse_connection& post(
                _In_ const const_narrow_string& resource,
                _In_ const nlohmann::json& json,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            auto dump = json.dump();
            auto octets = new byte_type[dump.size()];
            ::memcpy(octets, dump.data(), dump.size());
            return this->post(resource, octets, dump.size(),
                [](const void *o) { delete[] static_cast<const byte_type *>(o); },
#if defined(_WIN32)
                const_narrow_string("application/json", CP_OEMCP),
#else /* defined(_WIN32) */
                const_narrow_string("application/json", nullptr),
#endif /* defined(_WIN32) */
                on_response, on_error, context);
        }

        ///// <summary>
        ///// Posts the specified JSON data to the specified resource location.
        ///// </summary>
        ///// <remarks>
        ///// This method can be used to create data sets.
        ///// </remarks>
        ///// <param name="resource">The path to the resource to post. The
        ///// base path configured will be prepended if set.</param>
        ///// <param name="json">The JSON object to post.</param>
        ///// <param name="on_response">A callback to be invoked if the response
        ///// to the request was received.</param>
        ///// <param name="on_error">A callback to be invoked if the request
        ///// failed asynchronously.</param>
        ///// <param name="context">A user-defined context pointer passed to the
        ///// callbacks.</param>
        ///// <returns><c>*this</c>.</returns>
        ///// <exception cref="std::system_error">If the method was called on an
        ///// object that has been moved.</exception>
        ///// <exception cref="std::system_error">If the request failed right away.
        ///// Note that even if the request initially succeeded, it might still
        ///// fail and call <paramref name="on_error" /> later.</exception>
        ///// <exception cref="std::bad_alloc">If the memory required to build the
        ///// request could not be alloctated.</exception>
        //inline dataverse_connection& post(_In_opt_z_ const wchar_t *resource,
        //        _In_ const nlohmann::json& json,
        //        _In_ const on_api_response_type on_response,
        //        _In_ const on_error_type on_error,
        //        _In_opt_ void *context = nullptr) {
        //    return this->post(resource, json, on_response, on_error, context);
        //}
#endif /* defined(DATAVERSE_WITH_JSON) */

        /// <summary>
        /// Puts the given data to the given resource location.
        /// </summary>
        /// <remarks>
        /// This method can be used to upload files using the S3 interface.
        /// </remarks>
        /// <param name="resource">The path to the resource to post. The
        /// base path configured will be prepended if set.</param>
        /// <param name="data">The data to post. The caller remains owner
        /// of this memory if no <see cref="data_deleter" /> is set and must
        /// make sure that the data remain valid until the request completed or
        /// failed.</param>
        /// <param name="cnt">The size of the data in bytes.</param>
        /// <param name="data_deleter">If not <c>nullptr</c>, the object will
        /// eventually free <paramref name="data" /> using this callback.
        /// </param>
        /// <param name="content_type">The MIME type of the data to be stored.
        /// </param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& put(_In_opt_z_ const wchar_t *resource,
            _In_reads_bytes_(cnt) const byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ const data_deleter_type data_deleter,
            _In_opt_z_ const wchar_t *content_type,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Puts the given data to the given resource location.
        /// </summary>
        /// <remarks>
        /// This method can be used to upload files using the S3 interface.
        /// </remarks>
        /// <param name="resource">The path to the resource to post. The
        /// base path configured will be prepended if set.</param>
        /// <param name="data">The data to post. The caller remains owner
        /// of this memory if no <see cref="data_deleter" /> is set and must
        /// make sure that the data remain valid until the request completed or
        /// failed.</param>
        /// <param name="cnt">The size of the data in bytes.</param>
        /// <param name="data_deleter">If not <c>nullptr</c>, the object will
        /// eventually free <paramref name="data" /> using this callback.
        /// </param>
        /// <param name="content_type">The MIME type of the data to be stored.
        /// </param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& put(_In_ const const_narrow_string& resource,
            _In_reads_bytes_(cnt) const byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ const data_deleter_type data_deleter,
            _In_ const const_narrow_string& content_type,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <summary>
        /// Posts the specified form to the specified resource location.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="form">The form data to post.</param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& upload(_In_z_ const wchar_t *persistent_id,
            _In_z_ const wchar_t *path,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <summary>
        /// Posts the specified form to the specified resource location.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="form">The form data to post.</param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& upload(
            _In_ const const_narrow_string& persistent_id,
            _In_ const const_narrow_string& path,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="description">A description of the file.</param>
        /// <param name="directory">The name of the folder to organise the file
        /// in a tree structure. If this is an empty string, the file will be
        /// placed at root level. Make sure to terminate the path with a
        /// slash.</param>
        /// <param name="categories">A list of
        /// <paramref name="cnt_cats" /> categories to be assigned to
        /// the file. It is safe to pass <c>nullptr</c>.</param>
        /// <param name="cnt_cats">The number of categories to add.
        /// </param>
        /// <param name="restricted"><c>true</c> for marking the file as
        /// restricted such that it can only be uploaded when registering in
        /// the guestbook. <c>false</c> for making the file freely available.
        /// </param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& upload(_In_z_ const wchar_t *persistent_id,
            _In_z_ const wchar_t *path,
            _In_z_ const wchar_t *description,
            _In_z_ const wchar_t *directory,
            const wchar_t **categories,
            _In_ const std::size_t cnt_cats,
            _In_ const bool restricted,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="description">A description of the file.</param>
        /// <param name="directory">The name of the folder to organise the file
        /// in a tree structure. If this is an empty string, the file will be
        /// placed at root level. Make sure to terminate the path with a
        /// slash.</param>
        /// <param name="categories">A list of
        /// <paramref name="cnt_cats" /> categories to be assigned to
        /// the file. It is safe to pass <c>nullptr</c>.</param>
        /// <param name="cnt_cats">The number of categories to add.
        /// </param>
        /// <param name="restricted"><c>true</c> for marking the file as
        /// restricted such that it can only be uploaded when registering in
        /// the guestbook. <c>false</c> for making the file freely available.
        /// </param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& upload(
            _In_ const const_narrow_string& persistent_id,
            _In_ const const_narrow_string& path,
            _In_ const const_narrow_string& description,
            _In_ const const_narrow_string& directory,
            _In_reads_opt_(cnt_cats) const const_narrow_string *categories,
            _In_ const std::size_t cnt_cats,
            _In_ const bool restricted,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="description">A description of the file.</param>
        /// <param name="directory">The name of the folder to organise the file
        /// in a tree structure. If this is an empty string, the file will be
        /// placed at root level. Make sure to terminate the path with a
        /// slash.</param>
        /// <param name="categories">A list of categories to be assigned to
        /// the file.</param>
        /// <param name="restricted"><c>true</c> for marking the file as
        /// restricted such that it can only be uploaded when registering in
        /// the guestbook. <c>false</c> for making the file freely available.
        /// </param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        template<class TTraits, class TSAlloc, class TVAlloc>
        inline dataverse_connection& upload(
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& persistent_id,
                _In_ const std::basic_string<wchar_t, TTraits, TSAlloc>& path,
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& description,
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& directory,
                _In_ const std::vector<std::basic_string<wchar_t, TTraits,
                    TSAlloc>, TVAlloc> categories,
                _In_ const bool restricted,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            std::vector<const wchar_t *> cats(categories.size());
            std::transform(categories.begin(), categories.end(), cats.begin(),
                [](const std::wstring& c) { return c.c_str();  });
            return this->upload(persistent_id.c_str(), path.c_str(),
                description.c_str(), directory.c_str(),
                cats.data(), cats.size(), restricted, on_response, on_error,
                context);
        }

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="description">A description of the file.</param>
        /// <param name="directory">The name of the folder to organise the file
        /// in a tree structure. If this is an empty string, the file will be
        /// placed at root level. Make sure to terminate the path with a
        /// slash.</param>
        /// <param name="categories">A list of categories to be assigned to
        /// the file.</param>
        /// <param name="restricted"><c>true</c> for marking the file as
        /// restricted such that it can only be uploaded when registering in
        /// the guestbook. <c>false</c> for making the file freely available.
        /// </param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        template<class TAlloc>
        inline dataverse_connection& upload(
                _In_ const const_narrow_string& persistent_id,
                _In_ const const_narrow_string& path,
                _In_ const const_narrow_string& description,
                _In_ const const_narrow_string& directory,
                _In_ const std::vector<const_narrow_string, TAlloc> categories,
                _In_ const bool restricted,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            return this->upload(persistent_id, path, description, directory,
                categories.data(), categories.size(), restricted, on_response,
                on_error, context);
        }

#if defined(DATAVERSE_WITH_JSON)
        /// <summary>
        /// Upload a file for the data set with the specified persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="description">The JSON-formatted description of the
        /// file. See
        /// <a href="https://guides.dataverse.org/en/latest/api/native-api.html">
        /// the Dataverse documentation</a> for details on how to format this
        /// JSON object.</param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        template<class TTraits, class TAlloc>
        inline dataverse_connection& upload(_In_ const std::basic_string<
                    wchar_t, TTraits, TAlloc>& persistent_id,
                _In_ const std::basic_string<wchar_t, TTraits, TAlloc>& path,
                _In_ const nlohmann::json& description,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            const auto url = std::wstring(L"/datasets/:persistentId/add?"
                L"persistentId=") + persistent_id;
            const auto dump = description.dump();
            const auto d = reinterpret_cast<const std::uint8_t *>(dump.data());
            const auto s = dump.size();
            return this->post(url.c_str(),
                std::move(this->make_form()
                    .add_file(L"file", path.c_str())
                    .add_field(L"jsonData", d, s)),
                on_response,
                on_error,
                context);
        }

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="description">The JSON-formatted description of the
        /// file. See
        /// <a href="https://guides.dataverse.org/en/latest/api/native-api.html">
        /// the Dataverse documentation</a> for details on how to format this
        /// JSON object.</param>
        /// <param name="on_response">A callback to be invoked if the response
        /// to the request was received.</param>
        /// <param name="on_error">A callback to be invoked if the request
        /// failed asynchronously.</param>
        /// <param name="context">A user-defined context pointer passed to the
        /// callbacks.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        dataverse_connection& upload(
            _In_ const const_narrow_string& persistent_id,
            _In_ const const_narrow_string& path,
            _In_ const nlohmann::json& description,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);
#endif /* defined(DATAVERSE_WITH_JSON) */

        /// <summary>
        /// Move assignment.
        /// </summary>
        /// <param name="rhs">The right-hand side operant.</param>
        /// <returns><c>*this</c>.</returns>
        dataverse_connection& operator =(
            _Inout_ dataverse_connection&& rhs) noexcept;

    private:

        template<class TOperation, class... TArgs>
        static std::future<blob> invoke_async(TOperation&& operation,
            dataverse_connection& that, TArgs&&... arguments);

        detail::dataverse_connection_impl& check_not_disposed(void) const;

        detail::dataverse_connection_impl *_impl;
    };

} /* namespace dataverse */
} /* namespace visus */

#include "dataverse/dataverse_connection.inl"
