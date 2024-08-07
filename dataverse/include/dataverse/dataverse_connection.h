﻿// <copyright file="dataverse_connection.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>
#include <chrono>
#include <cstring>
#include <future>
#include <stdexcept>
#include <string>
#include <system_error>
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
        typedef void (*on_api_response_type)(_In_ const nlohmann::json&,
            _In_opt_ void *);
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
        /// The string used to identify a non-published draught version, e.g.
        /// when retrieving <see cref="files" /> of data set.
        /// </summary>
        static const wchar_t *const draught_version;

        /// <summary>
        /// The string used to identify the latest published version, e.g.
        /// when retrieving <see cref="files" /> of data set.
        /// </summary>
        static const wchar_t *const latest_published_version;

        /// <summary>
        /// The string used to identify the latest version, e.g.
        /// when retrieving <see cref="files" /> of data set.
        /// </summary>
        static const wchar_t *const latest_version;

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
        /// <remarks>
        /// The API key is considered a sensitive resource and therefore cannot
        /// be retrieved once set.
        /// </remarks>
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
        /// <remarks>
        /// The API key is considered a sensitive resource and therefore cannot
        /// be retrieved once set.
        /// </remarks>
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
        /// Answer the API base path used by the connection.
        /// </summary>
        /// <returns>The API base path</returns>
        const_narrow_string base_path(void) const;

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

        /// <summary>
        /// Gets the description of a data set, which is required for instance
        /// for enumerating the files in it.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set to get
        /// the description of.</param>
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
        dataverse_connection& data_set(_In_z_ const wchar_t *persistent_id,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        /// <summary>
        /// Gets the description of a data set, which is required for instance
        /// for enumerating the files in it.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set to get
        /// the description of.</param>
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
        dataverse_connection& data_set(
            _In_ const const_narrow_string& persistent_id,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

#if defined(DATAVERSE_WITH_JSON)
        /// <summary>
        /// Gets the description of a data set, which is required for instance
        /// for enumerating the files in it.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set to get
        /// the description of.</param>
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
        inline dataverse_connection& data_set(
                _In_ const std::wstring& persistent_id,
                _In_ const on_api_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context) {
            const auto url = std::wstring(L"/datasets/:persistentId/?"
                L"persistentId=") + persistent_id;
            this->get(url.c_str(),
                translate_api_reponse<nlohmann::json>,
                reinterpret_cast<const void *>(on_response),
                on_error,
                context);
            return *this;
        }

        /// <summary>
        /// Gets the description of a data set, which is required for instance
        /// for enumerating the files in it.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set to get
        /// the description of.</param>
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
        inline dataverse_connection& data_set(
                _In_ const const_narrow_string& persistent_id,
                _In_ const on_api_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context) {
            const auto url = std::wstring(L"/datasets/:persistentId/?"
                L"persistentId=") + convert<wchar_t>(persistent_id);
            this->get(url.c_str(),
                translate_api_reponse<nlohmann::json>,
                reinterpret_cast<const void *>(on_response),
                on_error,
                context);
            return *this;
        }
#endif /* defined(DATAVERSE_WITH_JSON) */

#if defined(DATAVERSE_WITH_JSON)
        /// <summary>
        /// Gets a future for the data set with the given persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set to get
        /// the description of.</param>
        /// <returns>A future for the response.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory
        inline std::future<nlohmann::json> data_set(
                _In_ const std::wstring& persistent_id) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const std::wstring&,
                const on_api_response_type,
                const on_error_type,
                void *);
            return invoke_async<nlohmann::json>(
                static_cast<actual_type>(&dataverse_connection::data_set),
                *this, persistent_id);
        }

        /// <summary>
        /// Gets a future for the data set with the given persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set to get
        /// the description of.</param>
        /// <returns>A future for the response.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<nlohmann::json> data_set(
                _In_ const const_narrow_string& persistent_id) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&,
                const on_api_response_type,
                const on_error_type,
                void *);
            return invoke_async<nlohmann::json>(
                static_cast<actual_type>(&dataverse_connection::data_set),
                *this, persistent_id);
        }
#else /* defined(DATAVERSE_WITH_JSON) */
        /// <summary>
        /// Gets a future for the data set with the given persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set to get
        /// the description of.</param>
        /// <returns>A future for the response.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory
        inline std::future<blob> data_set(
                _In_ const std::wstring& persistent_id) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const wchar_t *,
                const on_response_type,
                const on_error_type,
                void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::data_set),
                *this, persistent_id.c_str());
        }

        /// <summary>
        /// Gets a future for the data set with the given persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set to get
        /// the description of.</param>
        /// <returns>A future for the response.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> data_set(
                _In_ const const_narrow_string& persistent_id) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&,
                const on_response_type,
                const on_error_type,
                void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::data_set),
                *this, persistent_id);
        }
#endif /* defined(DATAVERSE_WITH_JSON) */

        /// <summary>
        /// Performs a &quot;direct upload&quot; of a data set to the S3
        /// backend.
        /// </summary>
        /// <remarks>
        /// <para>This method will only work if the administrator of the target
        /// Dataverse has enabled direct uploads.</para>
        /// </remarks>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="mime_type">The MIME type of the file to be uploaded.
        /// This must be manually set for direct uploads, because the Dataverse
        /// cannot determine this on its own using this upload path.</param>
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
        inline dataverse_connection& direct_upload(
                _In_z_ const wchar_t *persistent_id,
                _In_z_ const wchar_t *path,
                _In_opt_z_ const wchar_t *mime_type,
                _In_z_ const wchar_t *description,
                _In_z_ const wchar_t *directory,
                _In_reads_z_(cnt_cats) const wchar_t **categories,
                _In_ const std::size_t cnt_cats,
                _In_ const bool restricted,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->direct_upload(persistent_id, path, mime_type, description,
                directory, categories, cnt_cats, restricted, on_response,
                nullptr, on_error, context);
            return *this;
        }

        /// <summary>
        /// Performs a &quot;direct upload&quot; of a data set to the S3
        /// backend.
        /// </summary>
        /// <remarks>
        /// <para>This method will only work if the administrator of the target
        /// Dataverse has enabled direct uploads.</para>
        /// </remarks>
        /// <typeparam name="TTraits">The type of the character traits of a
        /// string.</typeparam>
        /// <typeparam name="TSAlloc">The allocator of a string.</typeparam>
        /// <typeparam name="TVAlloc">The allocator of a vector.</typeparam>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="mime_type">The MIME type of the file to be uploaded.
        /// This must be manually set for direct uploads, because the Dataverse
        /// cannot determine this on its own using this upload path.</param>
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
        inline dataverse_connection& direct_upload(
                _In_ const const_narrow_string& persistent_id,
                _In_ const const_narrow_string& path,
                _In_ const const_narrow_string& mime_type,
                _In_ const const_narrow_string& description,
                _In_ const const_narrow_string& directory,
                _In_reads_opt_(cnt_cats) const const_narrow_string *categories,
                _In_ const std::size_t cnt_cats,
                _In_ const bool restricted,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->direct_upload(persistent_id, path, mime_type, description,
                directory, categories, cnt_cats, restricted, on_response,
                nullptr, on_error, context);
            return *this;
        }

        /// <summary>
        /// Performs a &quot;direct upload&quot; of a data set to the S3
        /// backend.
        /// </summary>
        /// <remarks>
        /// <para>This method will only work if the administrator of the target
        /// Dataverse has enabled direct uploads.</para>
        /// </remarks>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="mime_type">The MIME type of the file to be uploaded.
        /// This must be manually set for direct uploads, because the Dataverse
        /// cannot determine this on its own using this upload path.</param>
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
        inline dataverse_connection& direct_upload(
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& persistent_id,
                _In_ const std::basic_string<wchar_t, TTraits, TSAlloc>& path,
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& mime_type,
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& description,
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& directory,
                _In_ const std::vector<std::basic_string<wchar_t, TTraits,
                    TSAlloc>, TVAlloc>& categories,
                _In_ const bool restricted,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            typedef std::basic_string<wchar_t, TTraits, TSAlloc> string_type;
            std::vector<const wchar_t *> cats(categories.size());
            std::transform(categories.begin(), categories.end(), cats.begin(),
                [](const string_type& c) { return c.c_str();  });
            this->direct_upload(persistent_id.c_str(), path.c_str(),
                mime_type.c_str(), description.c_str(), directory.c_str(),
                cats.data(), cats.size(), restricted, on_response, nullptr,
                on_error, context);
            return *this;
        }

        /// <summary>
        /// Performs a &quot;direct upload&quot; of a data set to the S3
        /// backend.
        /// </summary>
        /// <remarks>
        /// <para>This method will only work if the administrator of the target
        /// Dataverse has enabled direct uploads.</para>
        /// </remarks>
        /// <typeparam name="TAlloc">The allocator of a vector.</typeparam>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="mime_type">The MIME type of the file to be uploaded.
        /// This must be manually set for direct uploads, because the Dataverse
        /// cannot determine this on its own using this upload path.</param>
        /// <param name="description">A description of the file.</param>
        /// <param name="directory">The name of the folder to organise the file
        /// in a tree structure. If this is an empty string, the file will be
        /// placed at root level. Make sure to terminate the path with a
        /// slash.</param>
        /// <param name="categories">A list of categories to be assigned to
        /// the file.</param>
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
        inline dataverse_connection& direct_upload(
                _In_ const const_narrow_string& persistent_id,
                _In_ const const_narrow_string& path,
                _In_ const const_narrow_string& mime_type,
                _In_ const const_narrow_string& description,
                _In_ const const_narrow_string& directory,
                _In_ const std::vector<const_narrow_string>& categories,
                _In_ const std::size_t cnt_cats,
                _In_ const bool restricted,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->direct_upload(persistent_id, path, mime_type,
                description, directory, categories.data(), categories.size(),
                restricted, on_response, nullptr, on_error, context);
            return *this;
        }

        /// <summary>
        /// Performs a &quot;direct upload&quot; of a data set to the S3
        /// backend and returns a future for the operation.
        /// </summary>
        /// <remarks>
        /// <para>This method will only work if the administrator of the target
        /// Dataverse has enabled direct uploads.</para>
        /// </remarks>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="mime_type">The MIME type of the file to be uploaded.
        /// This must be manually set for direct uploads, because the Dataverse
        /// cannot determine this on its own using this upload path.</param>
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
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        template<class TTraits, class TSAlloc, class TVAlloc>
        inline std::future<blob> direct_upload(
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& persistent_id,
                _In_ const std::basic_string<wchar_t, TTraits, TSAlloc>& path,
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& mime_type,
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& description,
                _In_ const std::basic_string<wchar_t, TTraits,
                    TSAlloc>& directory,
                _In_ const std::vector<std::basic_string<wchar_t, TTraits,
                    TSAlloc>, TVAlloc> categories,
                _In_ const bool restricted) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const std::basic_string<wchar_t, TTraits, TSAlloc>&,
                const std::basic_string<wchar_t, TTraits, TSAlloc>&,
                const std::basic_string<wchar_t, TTraits, TSAlloc>&,
                const std::basic_string<wchar_t, TTraits, TSAlloc>&,
                const std::basic_string<wchar_t, TTraits, TSAlloc>&,
                const std::vector<std::basic_string<wchar_t, TTraits,TSAlloc>,
                    TVAlloc>&,
                const bool,
                const on_response_type, const on_error_type, void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::direct_upload),
                *this, persistent_id, path, mime_type, description, directory,
                categories, restricted);
        }

        /// <summary>
        /// Performs a &quot;direct upload&quot; of a data set to the S3
        /// backend and returns a future for the operation.
        /// </summary>
        /// <remarks>
        /// <para>This method will only work if the administrator of the target
        /// Dataverse has enabled direct uploads.</para>
        /// </remarks>
        /// <typeparam name="TAlloc">The allocator of a vector.</typeparam>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <param name="mime_type">The MIME type of the file to be uploaded.
        /// This must be manually set for direct uploads, because the Dataverse
        /// cannot determine this on its own using this upload path.</param>
        /// <param name="description">A description of the file.</param>
        /// <param name="directory">The name of the folder to organise the file
        /// in a tree structure. If this is an empty string, the file will be
        /// placed at root level. Make sure to terminate the path with a
        /// slash.</param>
        /// <param name="categories">A list of categories to be assigned to
        /// the file.</param>
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
        inline std::future<blob> direct_upload(
                _In_ const const_narrow_string& persistent_id,
                _In_ const const_narrow_string& path,
                _In_ const const_narrow_string& mime_type,
                _In_ const const_narrow_string& description,
                _In_ const const_narrow_string& directory,
                _In_ const std::vector<const_narrow_string>& categories,
                _In_ const std::size_t cnt_cats,
                _In_ const bool restricted) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&,
                const const_narrow_string&,
                const const_narrow_string&,
                const const_narrow_string&,
                const const_narrow_string&,
                const std::vector<const_narrow_string, TAlloc>&,
                const bool,
                const on_response_type, const on_error_type, void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::direct_upload),
                *this, persistent_id, path, mime_type, description, directory,
                categories, restricted);
        }

        /// <summary>
        /// Download the file with the specified ID into a memory buffer.
        /// </summary>
        /// <param name="id">The unqiue ID of the file.</param>
        /// <param name="format">The format of the file to retrieve, which can
        /// be something like &quot;original&quot; or &quot;RData&quot;.</param>
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
        dataverse_connection& download(_In_ const std::uint64_t id,
            _In_z_ const wchar_t *format,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Download the file with the specified ID into a memory buffer.
        /// </summary>
        /// <param name="id">The unqiue ID of the file.</param>
        /// <param name="format">The format of the file to retrieve, which can
        /// be something like &quot;original&quot; or &quot;RData&quot;.</param>
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
        dataverse_connection& download(_In_ const std::uint64_t id,
            _In_ const const_narrow_string& format,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Gets a future for the contents of the file with the specified ID.
        /// </summary>
        /// <param name="id">The unqiue ID of the file.</param>
        /// <param name="format">The format of the file to retrieve, which can
        /// be something like &quot;original&quot; or &quot;RData&quot;.</param>
        /// <returns>A future for the contents of the file.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> download(_In_ const std::uint64_t id,
                _In_z_ const wchar_t *format = L"original") {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const std::uint64_t,
                const wchar_t *,
                const on_response_type,
                const on_error_type,
                void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::download),
                *this, id, format);
        }

        /// <summary>
        /// Gets a future for the contents of the file with the specified ID.
        /// </summary>
        /// <param name="id">The unqiue ID of the file.</param>
        /// <param name="format">The format of the file to retrieve, which can
        /// be something like &quot;original&quot; or &quot;RData&quot;.</param>
        /// <returns>A future for the contents of the file.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> download(_In_ const std::uint64_t id,
                _In_ const const_narrow_string& format) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const std::uint64_t,
                const const_narrow_string&,
                const on_response_type,
                const on_error_type,
                void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::download),
                *this, id, format);
        }
        /// <summary>
        /// Download the file with the specified persistent identifier into a
        /// memory buffer.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the file, which is
        /// the DOI of the data set concatenated with some unique ID of the
        /// file. Note that this might not work for unpublished data.</param>
        /// <param name="format">The format of the file to retrieve, which can
        /// be something like &quot;original&quot; or &quot;RData&quot;.</param>
        /// <param name="version">The version of the file to be retrieved. This
        /// can be one of the special constants like
        /// <see cref="dataverse_connection::latest_version" />.</param>
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
        dataverse_connection& download(_In_z_ const wchar_t *persistent_id,
            _In_z_ const wchar_t *format,
            _In_z_ const wchar_t *version,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Download the file with the specified persistent identifier into a
        /// memory buffer.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the file, which is
        /// the DOI of the data set concatenated with some unique ID of the
        /// file. Note that this might not work for unpublished data.</param>
        /// <param name="format">The format of the file to retrieve, which can
        /// be something like &quot;original&quot; or &quot;RData&quot;.</param>
        /// <param name="version">The version of the file to be retrieved. This
        /// can be one of the special constants like
        /// <see cref="dataverse_connection::latest_version" />.</param>
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
        dataverse_connection& download(
            _In_ const const_narrow_string& persistent_id,
            _In_ const const_narrow_string& format,
            _In_ const const_narrow_string& version,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Gets a future for the contents of the file with the specified
        /// persistent identifier.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the file, which is
        /// the DOI of the data set concatenated with some unique ID of the
        /// file. Note that this might not work for unpublished data.</param>
        /// <param name="format">The format of the file to retrieve, which can
        /// be something like &quot;original&quot; or &quot;RData&quot;.</param>
        /// <param name="version">The version of the file to be retrieved. This
        /// can be one of the special constants like
        /// <see cref="dataverse_connection::latest_version" />.</param>
        /// <returns>A future for the contents of the file.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> download(_In_z_ const wchar_t *persistent_id,
                _In_z_ const wchar_t *format = L"original",
                _In_z_ const wchar_t *version = latest_version) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const wchar_t *,
                const wchar_t *,
                const wchar_t *,
                const on_response_type,
                const on_error_type,
                void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::download),
                *this, persistent_id, format, version);
        }

        /// <summary>
        /// Gets a future for the contents of the file with the specified
        /// persistent identifier.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the file, which is
        /// the DOI of the data set concatenated with some unique ID of the
        /// file. Note that this might not work for unpublished data.</param>
        /// <param name="format">The format of the file to retrieve, which can
        /// be something like &quot;original&quot; or &quot;RData&quot;.</param>
        /// <param name="version">The version of the file to be retrieved. This
        /// can be one of the special constants like
        /// <see cref="dataverse_connection::latest_version" />.</param>
        /// <returns>A future for the contents of the file.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> download(
                _In_ const const_narrow_string& persistent_id,
                _In_ const const_narrow_string& format,
                _In_ const const_narrow_string& version) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&,
                const const_narrow_string&,
                const const_narrow_string&,
                const on_response_type,
                const on_error_type,
                void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::download),
                *this, persistent_id, format, version);
        }

        /// <summary>
        /// Deletes the specified resource.
        /// </summary>
        /// <remarks>
        /// This method performs an HTTP <c>DELETE</c>, but we could not name it
        /// this way, because <c>delete</c> is a C++ keyword.
        /// </remarks>
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
        inline dataverse_connection& erase(_In_opt_z_ const wchar_t *resource,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context) {
            this->delete_resource(resource, on_response, nullptr, on_error,
                context);
            return *this;
        }

        /// <summary>
        /// Deletes the specified resource.
        /// </summary>
        /// <remarks>
        /// This method performs an HTTP <c>DELETE</c>, but we could not name it
        /// this way, because <c>delete</c> is a C++ keyword.
        /// </remarks>
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
        inline dataverse_connection& erase(
                _In_ const const_narrow_string& resource,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context) {
            this->delete_resource(resource, on_response, nullptr, on_error,
                context);
            return *this;
        }

        /// <summary>
        /// Deletes the specified resource and returns a future that can be used
        /// to determine whether the operation succeeded.
        /// </summary>
        /// <remarks>
        /// This method performs an HTTP <c>DELETE</c>, but we could not name it
        /// this way, because <c>delete</c> is a C++ keyword.
        /// </remarks>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <returns>A future for the result of the opration.</returns>
        inline std::future<void> erase(_In_ const std::wstring &resource) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const wchar_t *, const on_response_type, const on_error_type,
                void *);
            return invoke_async(
                static_cast<actual_type>(&dataverse_connection::erase),
                *this, resource.c_str());
        }

        /// <summary>
        /// Deletes the specified resource and returns a future that can be used
        /// to determine whether the operation succeeded.
        /// </summary>
        /// <remarks>
        /// This method performs an HTTP <c>DELETE</c>, but we could not name it
        /// this way, because <c>delete</c> is a C++ keyword.
        /// </remarks>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <returns>A future for the result of the opration.</returns>
        inline std::future<void> erase(_In_ const const_narrow_string &resource) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&, const on_response_type,
                const on_error_type, void *);
            return invoke_async(
                static_cast<actual_type>(&dataverse_connection::erase),
                *this, resource);
        }

        /// <summary>
        /// Gets the files in the data set with the given ID.
        /// </summary>
        /// <param name="id">The ID of the data set, which is unfortunately not
        /// the persistent identifier, but the primary key, which can be
        /// retrieved using <see cref="data_set" /> from the persistent
        /// identifier.</param>
        /// <param name="version">The version of the data set to retrieve, which
        /// is typically something like &quot;1.0&quot;. You can also use the
        /// constants for special versions like
        /// <see cref="dataverse_connection::latest_version" />.</param>
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
        dataverse_connection& files(_In_ const std::uint64_t id,
            _In_z_ const wchar_t *version,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Gets the files in the data set with the given ID.
        /// </summary>
        /// <param name="id">The ID of the data set, which is unfortunately not
        /// the persistent identifier, but the primary key, which can be
        /// retrieved using <see cref="data_set" /> from the persistent
        /// identifier.</param>
        /// <param name="version">The version of the data set to retrieve, which
        /// is typically something like &quot;1.0&quot;</param>
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
        dataverse_connection& files(_In_ const std::uint64_t id,
            _In_ const const_narrow_string& version,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

#if defined(DATAVERSE_WITH_JSON)
        /// <summary>
        /// Gets the files in the data set with the given ID.
        /// </summary>
        /// <param name="id">The ID of the data set, which is unfortunately not
        /// the persistent identifier, but the primary key, which can be
        /// retrieved using <see cref="data_set" /> from the persistent
        /// identifier.</param>
        /// <param name="version">The version of the data set to retrieve, which
        /// is typically something like &quot;1.0&quot;. You can also use the
        /// constants for special versions like
        /// <see cref="dataverse_connection::latest_version" />.</param>
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
        inline dataverse_connection& files(_In_ const std::uint64_t id,
                _In_ const std::wstring& version,
                _In_ const on_api_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context) {
            const auto url = std::wstring(L"/datasets/") + std::to_wstring(id)
                + std::wstring(L"/versions/") + version
                + std::wstring(L"/files");
            this->get(url.c_str(),
                translate_api_reponse<nlohmann::json>,
                reinterpret_cast<const void *>(on_response),
                on_error,
                context);
            return *this;
        }

        /// <summary>
        /// Gets the files in the data set with the given ID.
        /// </summary>
        /// <param name="id">The ID of the data set, which is unfortunately not
        /// the persistent identifier, but the primary key, which can be
        /// retrieved using <see cref="data_set" /> from the persistent
        /// identifier.</param>
        /// <param name="version">The version of the data set to retrieve, which
        /// is typically something like &quot;1.0&quot;.</param>
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
        inline dataverse_connection& files(_In_ const std::uint64_t id,
                _In_ const const_narrow_string& version,
                _In_ const on_api_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context) {
            const auto url = std::wstring(L"/datasets/") + std::to_wstring(id)
                + std::wstring(L"/versions/") + convert<wchar_t>(version)
                + std::wstring(L"/files");
            this->get(url.c_str(),
                translate_api_reponse<nlohmann::json>,
                reinterpret_cast<const void *>(on_response),
                on_error,
                context);
            return *this;
        }
#endif /* defined(DATAVERSE_WITH_JSON) */

#if defined(DATAVERSE_WITH_JSON)
        /// <summary>
        /// Gets a future for the files in the data set with the given ID.
        /// </summary>
        /// <param name="id">The ID of the data set, which is unfortunately not
        /// the persistent identifier, but the primary key, which can be
        /// retrieved using <see cref="data_set" /> from the persistent
        /// identifier.</param>
        /// <param name="version">The version of the data set to retrieve, which
        /// is typically something like &quot;1.0&quot;. You can also use the
        /// constants for special versions like
        /// <see cref="dataverse_connection::latest_version" />.</param>
        /// <returns>A future for the files.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<nlohmann::json> files(_In_ const std::uint64_t id,
                _In_ const std::wstring& version) {
            typedef dataverse_connection &(dataverse_connection:: *actual_type)(
                const std::uint64_t,
                const std::wstring&,
                const on_api_response_type,
                const on_error_type,
                void *);
            return invoke_async<nlohmann::json>(
                static_cast<actual_type>(&dataverse_connection::files),
                *this, id, version);
        }

        /// <summary>
        /// Gets a future for the files in the data set with the given ID.
        /// </summary>
        /// <param name="id">The ID of the data set, which is unfortunately not
        /// the persistent identifier, but the primary key, which can be
        /// retrieved using <see cref="data_set" /> from the persistent
        /// identifier.</param>
        /// <param name="version">The version of the data set to retrieve, which
        /// is typically something like &quot;1.0&quot;.</param>
        /// <returns>A future for the files.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<nlohmann::json> files(_In_ const std::uint64_t id,
                _In_ const const_narrow_string& version) {
            typedef dataverse_connection &(dataverse_connection:: *actual_type)(
                const std::uint64_t,
                const const_narrow_string&,
                const on_api_response_type,
                const on_error_type,
                void *);
            return invoke_async<nlohmann::json>(
                static_cast<actual_type>(&dataverse_connection::files),
                *this, id, version);
        }
#else /* defined(DATAVERSE_WITH_JSON) */
        /// <summary>
        /// Gets a future for the files in the data set with the given ID.
        /// </summary>
        /// <param name="id">The ID of the data set, which is unfortunately not
        /// the persistent identifier, but the primary key, which can be
        /// retrieved using <see cref="data_set" /> from the persistent
        /// identifier.</param>
        /// <param name="version">The version of the data set to retrieve, which
        /// is typically something like &quot;1.0&quot;. You can also use the
        /// constants for special versions like
        /// <see cref="dataverse_connection::latest_version" />.</param>
        /// <returns>A future for the files.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> files(_In_ const std::uint64_t id,
                _In_ const std::wstring& version) {
            typedef dataverse_connection &(dataverse_connection:: *actual_type)(
                const std::uint64_t,
                const wchar_t *,
                const on_response_type,
                const on_error_type,
                void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::files),
                *this, id, version.c_str());
        }

        /// <summary>
        /// Gets a future for the files in the data set with the given ID.
        /// </summary>
        /// <param name="id">The ID of the data set, which is unfortunately not
        /// the persistent identifier, but the primary key, which can be
        /// retrieved using <see cref="data_set" /> from the persistent
        /// identifier.</param>
        /// <param name="version">The version of the data set to retrieve, which
        /// is typically something like &quot;1.0&quot;.</param>
        /// <returns>A future for the files.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> files(_In_ const std::uint64_t id,
                _In_ const const_narrow_string& version) {
            typedef dataverse_connection &(dataverse_connection:: *actual_type)(
                const std::uint64_t,
                const const_narrow_string&,
                const on_response_type,
                const on_error_type,
                void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::files),
                *this, id, version);
        }
#endif /* defined(DATAVERSE_WITH_JSON) */

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
        inline dataverse_connection& get(_In_opt_z_ const wchar_t *resource,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->get(resource, on_response, nullptr, on_error, context);
            return *this;
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
        inline dataverse_connection& get(
                _In_ const const_narrow_string& resource,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->get(resource, on_response, nullptr, on_error, context);
            return *this;
        }

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
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::get),
                *this, resource.c_str());
        }

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
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::get),
                *this, resource);
        }

        /// <summary>
        /// Sets the wait timeout of the I/O thread in milliseconds.
        /// </summary>
        /// <remarks>
        /// This should only be done before making the first request.
        /// </remarks>
        /// <param name="millis">The timeout of the I/O thread in milliseconds.
        /// </param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        dataverse_connection& io_timeout(_In_ const int millis);

        /// <summary>
        /// Sets the wait timeout of the I/O thread in milliseconds.
        /// </summary>
        /// <remarks>
        /// This should only be done before making the first request.
        /// </remarks>
        /// <typeparam name="TRep"></typeparam>
        /// <typeparam name="TRatio"></typeparam>
        /// <param name="millis">The timeout of the I/O thread in milliseconds.
        /// </param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        template<class TRep, class TRatio>
        inline dataverse_connection& io_timeout(
                _In_ std::chrono::duration<TRep, TRatio> timeout) {
            typedef std::chrono::duration<int, std::milli> millis_type;
            auto millis = std::chrono::duration_cast<millis_type>(timeout);
            return this->io_timeout(millis.count());
        }

        /// <summary>
        /// Answers the I/O timeout in milliseconds.
        /// </summary>
        /// <returns>The timeout of the I/O thread when checking for activity.
        /// </returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        int io_timeout(void) const;

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
        inline dataverse_connection& post(_In_opt_z_ const wchar_t *resource,
                _Inout_ form_data&& form,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->post(resource, std::move(form), on_response, nullptr,
                on_error, context);
            return *this;
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
        inline dataverse_connection& post(
                _In_ const const_narrow_string& resource,
                _Inout_ form_data&& form,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->post(resource, std::move(form), on_response, nullptr,
                on_error, context);
            return *this;
        }

        /// <summary>
        /// Posts the specified form to the specified resource location and
        /// returns a future for the result.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="form">The form data to post. The connection object
        /// takes ownership of the form.</param>
        /// <returns>A future for the result of the opration.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> post(_In_opt_z_ const wchar_t *resource,
                _Inout_ form_data&& form) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const wchar_t *, form_data&&, const on_response_type,
                const on_error_type, void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::post),
                *this, resource, std::move(form));
        }

        /// <summary>
        /// Posts the specified form to the specified resource location and
        /// returns a future for the result.
        /// </summary>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <param name="form">The form data to post.</param>
        /// <returns>A future for the result of the opration.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> post(
                _In_ const const_narrow_string& resource,
                _Inout_ form_data&& form) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&, form_data&&,
                const on_response_type, const on_error_type, void *);
            return invoke_async<blob>(
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
        inline dataverse_connection& post(_In_opt_z_ const wchar_t *resource,
                _In_reads_bytes_(cnt) const byte_type *data,
                _In_ const std::size_t cnt,
                _In_opt_ const data_deleter_type data_deleter,
                _In_opt_z_ const wchar_t *content_type,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->post(resource, data, cnt, data_deleter, content_type,
                on_response, nullptr, on_error, context);
            return *this;
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
        inline dataverse_connection& post(
                _In_ const const_narrow_string& resource,
                _In_reads_bytes_(cnt) const byte_type *data,
                _In_ const std::size_t cnt,
                _In_opt_ const data_deleter_type data_deleter,
                _In_ const const_narrow_string& content_type,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->post(resource, data, cnt, data_deleter, content_type,
                on_response, nullptr, on_error, context);
            return *this;
        }

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
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::post),
                *this, resource, data, cnt, data_deleter, content_type);
        }

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
                _In_ const const_narrow_string& resource,
                _In_reads_bytes_(cnt) const byte_type *data,
                _In_ const std::size_t cnt,
                _In_opt_ const data_deleter_type data_deleter,
                _In_ const const_narrow_string& content_type) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&, const byte_type *,
                const std::size_t, const data_deleter_type,
                const const_narrow_string&, const on_response_type,
                const on_error_type, void *);
            return invoke_async<blob>(
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
            auto deleter = [](const void *o) {
                delete[] static_cast<const byte_type *>(o);
            };
            ::memcpy(octets, dump.data(), dump.size());
            this->post(resource,
                octets, dump.size(), deleter,
                L"application/json",
                on_response,
                nullptr,
                on_error,
                context);
            return *this;
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
            auto deleter = [](const void *o) {
                delete[] static_cast<const byte_type *>(o);
            };
            ::memcpy(octets, dump.data(), dump.size());
            this->post(resource,
                octets, dump.size(), deleter,
                const_narrow_string("application/json", dataversepp_code_page),
                on_response,
                nullptr,
                on_error,
                context);
            return *this;
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
        inline dataverse_connection& post(_In_opt_z_ const wchar_t *resource,
                _In_ const nlohmann::json& json,
                _In_ const on_api_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            auto dump = json.dump();
            auto octets = new byte_type[dump.size()];
            ::memcpy(octets, dump.data(), dump.size());
            auto deleter = [](const void *o) {
                delete[] static_cast<const byte_type *>(o);
            };
            this->post(resource,
                octets, dump.size(), deleter,
                L"application/json",
                translate_api_reponse<nlohmann::json>,
                reinterpret_cast<const void *>(on_response),
                on_error,
                context);
            return *this;
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
                _In_ const on_api_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            auto dump = json.dump();
            auto octets = new byte_type[dump.size()];
            ::memcpy(octets, dump.data(), dump.size());
            auto deleter = [](const void *o) {
                delete[] static_cast<const byte_type *>(o);
            };
            this->post(resource,
                octets, dump.size(), deleter,
                const_narrow_string("application/json", dataversepp_code_page),
                translate_api_reponse<nlohmann::json>,
                reinterpret_cast<const void *>(on_response),
                on_error,
                context);
            return *this;
        }

        /// <summary>
        /// Posts the specified JSON data to the specified resource location and
        /// returns a future for the API result.
        /// </summary>
        /// <remarks>
        /// This method can be used to create data sets.
        /// </remarks>
        /// <param name="resource">The path to the resource to post. The
        /// base path configured will be prepended if set.</param>
        /// <param name="json">The JSON object to post.</param>
        /// <returns>A future for the API result..</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<nlohmann::json> post(
                _In_opt_z_ const wchar_t *resource,
                _In_ const nlohmann::json& json) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const wchar_t *,
                const nlohmann::json&,
                const on_api_response_type, const on_error_type, void *);
            return invoke_async<nlohmann::json>(
                static_cast<actual_type>(&dataverse_connection::post), *this,
                resource, json);
        }

        /// <summary>
        /// Posts the specified JSON data to the specified resource location and
        /// returns a future for the API result.
        /// </summary>
        /// <remarks>
        /// This method can be used to create data sets.
        /// </remarks>
        /// <param name="resource">The path to the resource to post. The
        /// base path configured will be prepended if set.</param>
        /// <param name="json">The JSON object to post.</param>
        /// <returns>A future for the API result..</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<nlohmann::json> post(
                _In_ const const_narrow_string& resource,
                _In_ const nlohmann::json& json) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&,
                const nlohmann::json&,
                const on_api_response_type, const on_error_type, void *);
            return invoke_async<nlohmann::json>(
                static_cast<actual_type>(&dataverse_connection::post), *this,
                resource, json);
        }
#endif /* defined(DATAVERSE_WITH_JSON) */

        /// <summary>
        /// Puts the given data to the given resource location.
        /// </summary>
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
        inline dataverse_connection& put(_In_opt_z_ const wchar_t *resource,
                _In_reads_bytes_(cnt) const byte_type *data,
                _In_ const std::size_t cnt,
                _In_opt_ const data_deleter_type data_deleter,
                _In_opt_z_ const wchar_t *content_type,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->put(resource, data, cnt, data_deleter, content_type,
                on_response, nullptr, on_error, context);
            return *this;
        }

        /// <summary>
        /// Puts the given data to the given resource location.
        /// </summary>
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
        inline dataverse_connection& put(
                _In_ const const_narrow_string& resource,
                _In_reads_bytes_(cnt) const byte_type *data,
                _In_ const std::size_t cnt,
                _In_opt_ const data_deleter_type data_deleter,
                _In_ const const_narrow_string& content_type,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            this->put(resource, data, cnt, data_deleter, content_type,
                on_response, nullptr, on_error, context);
            return *this;
        }

        /// <summary>
        /// Deletes the specified resource.
        /// </summary>
        /// <remarks>
        /// This is an alias for <see cref="dataverse_connection::erase" />.
        /// </remarks>
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
        inline dataverse_connection& remove(_In_opt_z_ const wchar_t *resource,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context) {
            return this->erase(resource, on_response, on_error, context);
        }

        /// <summary>
        /// Deletes the specified resource.
        /// </summary>
        /// <remarks>
        /// This is an alias for <see cref="dataverse_connection::erase" />.
        /// </remarks>
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
        inline dataverse_connection& remove(
                _In_ const const_narrow_string& resource,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context) {
            return this->erase(resource, on_response, on_error, context);
        }

        /// <summary>
        /// Deletes the specified resource and returns a future that can be used
        /// to determine whether the operation succeeded.
        /// </summary>
        /// <remarks>
        /// This is an alias for <see cref="dataverse_connection::erase" />.
        /// </remarks>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <returns>A future for the result of the opration.</returns>
        inline std::future<void> remove(_In_ const std::wstring& resource) {
            return this->erase(resource);
        }

        /// <summary>
        /// Deletes the specified resource and returns a future that can be used
        /// to determine whether the operation succeeded.
        /// </summary>
        /// <remarks>
        /// This is an alias for <see cref="dataverse_connection::erase" />.
        /// </remarks>
        /// <param name="resource">The path to the resource. The
        /// <see cref="base_path" /> will be prepended if it is set.</param>
        /// <returns>A future for the result of the opration.</returns>
        inline std::future<void> remove(
                _In_ const const_narrow_string& resource) {
            return this->erase(resource);
        }

        /// <summary>
        /// Replaces the file with the sepcified database identifier with the
        /// content of the file at the specified location.
        /// </summary>
        /// <param name="id">The database ID of the file to be replaced.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
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
        dataverse_connection& replace(_In_ const std::uint64_t id,
            _In_z_ const wchar_t *path,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Replaces the file with the sepcified database identifier with the
        /// content of the file at the specified location.
        /// </summary>
        /// <param name="id">The database ID of the file to be replaced.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
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
        dataverse_connection& replace(_In_ const std::uint64_t id,
            _In_ const const_narrow_string& path,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Replaces the file with the sepcified database identifier with the
        /// content of the file at the specified location.
        /// </summary>
        /// <param name="id">The database ID of the file to be replaced.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> replace(_In_ const std::uint64_t id,
                _In_ const std::wstring& path) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const std::uint64_t,
                const wchar_t *,
                const on_response_type, const on_error_type, void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::replace), *this,
                id, path.c_str());
        }

        /// <summary>
        /// Replaces the file with the sepcified database identifier with the
        /// content of the file at the specified location.
        /// </summary>
        /// <param name="id">The database ID of the file to be replaced.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<blob> replace(_In_ const std::uint64_t id,
                _In_ const const_narrow_string& path) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const std::uint64_t,
                const const_narrow_string&,
                const on_response_type, const on_error_type, void *);
            return invoke_async<blob>(
                static_cast<actual_type>(&dataverse_connection::replace), *this,
                id, path);
        }

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID.
        /// </summary>
        /// <param name="persistent_id">The persistent ID of the data set the
        /// file should be added to, which typically has the form
        /// &quot;doi:the-doi&quot;.</param>
        /// <param name="path">The path to the file to be uploaded.</param>
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
        /// <typeparam name="TTraits">The type of the character traits of a
        /// string.</typeparam>
        /// <typeparam name="TSAlloc">The allocator of a string.</typeparam>
        /// <typeparam name="TVAlloc">The allocator of a vector.</tyepparam>
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
                    TSAlloc>, TVAlloc>& categories,
                _In_ const bool restricted,
                _In_ const on_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            typedef std::basic_string<wchar_t, TTraits, TSAlloc> string_type;
            std::vector<const wchar_t *> cats(categories.size());
            std::transform(categories.begin(), categories.end(), cats.begin(),
                [](const string_type& c) { return c.c_str();  });
            return this->upload(persistent_id.c_str(), path.c_str(),
                description.c_str(), directory.c_str(),
                cats.data(), cats.size(), restricted, on_response, on_error,
                context);
        }

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID.
        /// </summary>
        /// <typeparam name="TAlloc">The allocator of a vector.</typeparam>
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
                _In_ const on_api_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            const auto url = std::wstring(L"/datasets/:persistentId/add?"
                L"persistentId=") + persistent_id;
            const auto dump = description.dump();
            const auto d = reinterpret_cast<const std::uint8_t *>(dump.data());
            const auto s = dump.size();
            this->post(url.c_str(),
                std::move(this->make_form()
                    .add_file(L"file", path.c_str())
                    .add_field(L"jsonData", d, s)),
                translate_api_reponse<nlohmann::json>,
                reinterpret_cast<const void *>(on_response),
                on_error,
                context);
            return *this;
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
        inline dataverse_connection& upload(
                _In_ const const_narrow_string& persistent_id,
                _In_ const const_narrow_string& path,
                _In_ const nlohmann::json& description,
                _In_ const on_api_response_type on_response,
                _In_ const on_error_type on_error,
                _In_opt_ void *context = nullptr) {
            const auto url = std::wstring(L"/datasets/:persistentId/add?"
                L"persistentId=") + convert<wchar_t>(persistent_id);
            const auto dump = description.dump();
            const auto d = reinterpret_cast<const std::uint8_t *>(dump.data());
            const auto s = dump.size();
            const auto p = convert<wchar_t>(path);
            this->post(url.c_str(),
                std::move(this->make_form()
                    .add_file(L"file", p.c_str())
                    .add_field(L"jsonData", d, s)),
                translate_api_reponse<nlohmann::json>,
                reinterpret_cast<const void *>(on_response),
                on_error,
                context);
            return *this;
        }

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID and
        /// returns a future for the response.
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
        /// <returns>A future for the response.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        template<class TTraits, class TAlloc>
        inline std::future<nlohmann::json> upload(_In_ const std::basic_string<
                    wchar_t, TTraits, TAlloc>& persistent_id,
                _In_ const std::basic_string<wchar_t, TTraits, TAlloc>& path,
                _In_ const nlohmann::json& description) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const std::basic_string<wchar_t, TTraits, TAlloc>&,
                const std::basic_string<wchar_t, TTraits, TAlloc>&,
                const nlohmann::json&,
                const on_api_response_type, const on_error_type, void *);
            return invoke_async<nlohmann::json>(
                static_cast<actual_type>(&dataverse_connection::upload), *this,
                persistent_id, path, description);
        }

        /// <summary>
        /// Upload a file for the data set with the specified persistent ID and
        /// returns a future for the response.
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
        /// <returns>A future for the response.</returns>
        /// <exception cref="std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::system_error">If the request failed right away.
        /// Note that even if the request initially succeeded, it might still
        /// fail and call <paramref name="on_error" /> later.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to build the
        /// request could not be alloctated.</exception>
        inline std::future<nlohmann::json> upload(
                _In_ const const_narrow_string& persistent_id,
                _In_ const const_narrow_string& path,
                _In_ const nlohmann::json& description) {
            typedef dataverse_connection& (dataverse_connection:: *actual_type)(
                const const_narrow_string&,
                const const_narrow_string&,
                const nlohmann::json&,
                const on_api_response_type, const on_error_type, void *);
            return invoke_async<nlohmann::json>(
                static_cast<actual_type>(&dataverse_connection::upload), *this,
                persistent_id, path, description);
        }
#endif /* defined(DATAVERSE_WITH_JSON) */

        /// <summary>
        /// Move assignment.
        /// </summary>
        /// <param name="rhs">The right-hand side operant.</param>
        /// <returns><c>*this</c>.</returns>
        dataverse_connection& operator =(
            _Inout_ dataverse_connection&& rhs) noexcept;

    private:

        /// <summary>
        /// Assuming <paramref name="client_data" /> is a
        /// <see cref="details::io_context" />, retrieve the actual client
        /// data.
        /// </summary>
        static void *get_api_response_client_data(_In_ void *client_data);

        /// <summary>
        /// Assuming <paramref name="client_data" /> is an
        /// <see cref="details::io_context" />, retrieve the JSON callback.
        /// </summary>
        static void *get_on_api_response(_In_ void *client_data);

        /// <summary>
        /// Assuming <paramref name="client_data" /> is an
        /// <see cref="details::io_context" />, retrieve the error callback.
        /// </summary>
        static on_error_type get_on_error(_In_ void *client_data);

        /// <summary>
        /// Assuming <paramref name="client_data" /> is an
        /// <see cref="details::io_context" />, report the specified API error
        /// to the error handler.
        /// </summary>
        static void report_api_error(_In_ void *client_data,
            _In_z_ const char *error);

        /// <summary>
        /// Converts an asynchronous call to <paramref name="operation" /> to
        /// an <see cref="std::future" />.
        /// </summary>
        template<class TResult, class TOperation, class... TArgs>
        static std::future<TResult> invoke_async(TOperation&& operation,
            dataverse_connection& that, TArgs&&... arguments);

        /// <summary>
        /// Converts an asynchronous call to <paramref name="operation" /> that
        /// does not yield a result into an <see cref="std::future" />.
        /// </summary>
        template<class TOperation, class... TArgs>
        static std::future<void> invoke_async(TOperation&& operation,
            dataverse_connection& that, TArgs&&... arguments);

        /// <summary>
        /// The callback to convert the native callback to a JSON callback.
        /// </summary>
        template<class TJson>
        static void translate_api_reponse(_In_ const blob& response,
            _In_ void *client_data);

        /// <summary>
        /// Checks that the API has not been disposed and returns its
        /// implementation if this is the case.
        /// </summary>
        detail::dataverse_connection_impl& check_not_disposed(void) const;

        void delete_resource(_In_opt_z_ const wchar_t *resource,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void delete_resource(_In_ const const_narrow_string& resource,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void direct_upload(_In_z_ const wchar_t *persistent_id,
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
            _In_opt_ void *context);

        void direct_upload(_In_ const const_narrow_string& persistent_id,
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
            _In_opt_ void *context);

        void get(_In_opt_z_ const wchar_t *resource,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void get(_In_ const const_narrow_string& resource,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void post(_In_opt_z_ const wchar_t *resource,
            _In_z_ const wchar_t *path,
            _In_opt_z_ const wchar_t *content_type,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void post(_In_ const const_narrow_string& resource,
            _In_ const const_narrow_string& path,
            _In_ const const_narrow_string& content_type,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void post(_In_opt_z_ const wchar_t *resource,
            _Inout_ form_data&& form,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void post(_In_ const const_narrow_string& resource,
            _Inout_ form_data&& form,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void post(_In_opt_z_ const wchar_t *resource,
            _In_reads_bytes_(cnt) const byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ const data_deleter_type data_deleter,
            _In_opt_z_ const wchar_t *content_type,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void post(_In_ const const_narrow_string& resource,
            _In_reads_bytes_(cnt) const byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ const data_deleter_type data_deleter,
            _In_ const const_narrow_string& content_type,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void put(_In_opt_z_ const wchar_t *resource,
            _In_z_ const wchar_t *path,
            _In_opt_z_ const wchar_t *content_type,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void put(_In_ const const_narrow_string& resource,
            _In_ const const_narrow_string& path,
            _In_ const const_narrow_string& content_type,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void put(_In_opt_z_ const wchar_t *resource,
            _In_reads_bytes_(cnt) const byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ const data_deleter_type data_deleter,
            _In_opt_z_ const wchar_t *content_type,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        void put(_In_ const const_narrow_string& resource,
            _In_reads_bytes_(cnt) const byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ const data_deleter_type data_deleter,
            _In_ const const_narrow_string& content_type,
            _In_ const on_response_type on_response,
            _In_opt_ const void *on_api_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context);

        detail::dataverse_connection_impl *_impl;
    };

} /* namespace dataverse */
} /* namespace visus */

#include "dataverse/dataverse_connection.inl"
