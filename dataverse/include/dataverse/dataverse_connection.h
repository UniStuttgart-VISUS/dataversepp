// <copyright file="dataverse_connection_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "dataverse/blob.h"
#include "dataverse/convert.h"
#include "dataverse/form_data.h"
#include "dataverse/types.h"


namespace visus {
namespace dataverse {

    /* Forward declarations. */
    namespace detail { class dataverse_connection_impl; }

    /// <summary>
    /// Represents a TCP/IP connection to the DataVerse API, which is basically a
    /// RAII wrapper around a TCP/IP socket.
    /// </summary>
    /// <remarks>
    /// <para>This API is only exposed for testing purposes.</para>
    /// </remarks>
    class DATAVERSE_API dataverse_connection {

    public:

        /// <summary>
        /// The type used to represent a single byte.
        /// </summary>
        typedef blob::byte_type byte_type;

        /// <summary>
        /// The callback to be invoked for a raw response.
        /// </summary>
        typedef void (*on_response_type)(_In_ const blob&,
            _In_opt_ void *);

        /// <summary>
        /// The callback to be invoked for an error.
        /// </summary>
        typedef void (*on_error_type)(_In_ const int,
            _In_z_ const char_type *,
            _In_z_ const char_type *,
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
        /// Sets a new API key to authenticate with DataVerse.
        /// </summary>
        /// <param name="api_key">The API key to use. It is safe to pass
        /// <c>nullptr</c>, in which case the object tries to make
        /// unauthenticated requests.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to store the
        /// data could not be alloctated.</exception>
        dataverse_connection& api_key(_In_opt_z_ const char_type *api_key);

        /// <summary>
        /// Sets the base path to the API end point such that you do not have to
        /// specify the common part with every request.
        /// </summary>
        /// <param name="base_path">The base path to prepend to every request.
        /// It is safe to pass <c>nullptr</c>.</param>
        /// <returns><c>*this</c>.</returns>
        /// <exception cref std::system_error">If the method was called on an
        /// object that has been moved.</exception>
        /// <exception cref="std::bad_alloc">If the memory required to store the
        /// data could not be alloctated.</exception>
        dataverse_connection& base_path(_In_opt_z_ const char_type *base_path);

        dataverse_connection& get(_In_opt_z_ const char_type *resource,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

        /// <summary>
        /// Create a new and empty form for a POST request.
        /// </summary>
        /// <returns></returns>
        form_data make_form(void) const;

        /// <summary>
        /// Posts the specified form to the specified resource location.
        /// </summary>
        /// <param name="resource"></param>
        /// <param name="form"></param>
        /// <param name="on_response"></param>
        /// <param name="on_error"></param>
        /// <param name="context"></param>
        /// <returns></returns>
        dataverse_connection& post(_In_opt_z_ const char_type *resource,
            _In_ const form_data& form,
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
        /// <param name="on_response"></param>
        /// <param name="on_error"></param>
        /// <param name="context"></param>
        /// <returns></returns>
        dataverse_connection& upload(_In_z_ const char_type *persistent_id,
            _In_z_ const char_type *path,
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
        /// <paramref name="cnt_categories" /> categories to be assigned to
        /// the file. It is safe to pass <c>nullptr</c>.</param>
        /// <param name="cnt_categories">The number of categories to add.
        /// </param>
        /// <param name="restricted"><c>true</c> for marking the file as
        /// restricted such that it can only be uploaded when registering in
        /// the guestbook. <c>false</c> for making the file freely available.
        /// </param>
        /// <param name="on_response"></param>
        /// <param name="on_error"></param>
        /// <param name="context"></param>
        /// <returns></returns>
        dataverse_connection& upload(_In_z_ const char_type *persistent_id,
            _In_z_ const char_type *path,
            _In_z_ const char_type *description,
            _In_z_ const char_type *directory,
            const char_type **categories,
            _In_ const std::size_t cnt_categories,
            _In_ const bool restricted,
            _In_ const on_response_type on_response,
            _In_ const on_error_type on_error,
            _In_opt_ void *context = nullptr);

//#if defined(nlohmann_json_FOUND)
//        inline dataverse_connection& upload(
//                _In_opt_z_ const char_type *persistent_id,
//                _In_ const char_type *path,
//                _In_ const nlohmann::json& description,
//                _In_ const on_response_type on_response,
//                _In_ const on_error_type on_error,
//                _In_opt_ void *context = nullptr) {
//            const auto url = std::basic_string<char_type>(DVSL("/datasets/")
//                DVSL(":persistentId/add?persistentId=")) + persistent_id;
//            auto desc = description.dump();
//            auto data = reinterpret_cast<const std::uint8_t *>(desc.c_str());
//            auto size = desc.size();
//            return this->post(url.c_str(),
//                this->make_form()
//                    .add_file(DVSL("file"), path)
//                    .add_field(DVSL("jsonData"), data, size),
//                on_response,
//                on_error,
//                context);
//        }
//#endif /* defined(nlohmann_json_FOUND) */

        dataverse_connection& operator =(
            _Inout_ dataverse_connection&& rhs) noexcept;

    private:

        detail::dataverse_connection_impl& check_not_disposed(void) const;

        detail::dataverse_connection_impl *_impl;

    };

} /* namespace dataverse */
} /* namespace visus */
