// <copyright file="io_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>

#include <wil/resource.h>
#endif /* defined(_WIN32) */

#include <nlohmann/json.hpp>

#include "dataverse/blob.h"
#include "dataverse/form_data.h"
#include "dataverse/dataverse_connection.h"

#include "dataverse_connection_impl.h"
#include "invoke_handler.h"
#include "posix_handle.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// The head of an in-flight context of an asynchronous I/O operation.
    /// </summary>
    struct io_context final {

        /// <summary>
        /// The type used to represent a single byte.
        /// </summary>
        typedef dataverse_connection::byte_type byte_type;

#if defined(_WIN32)
        /// <summary>
        /// The type of a file handle.
        /// </summary>
        typedef wil::unique_hfile file_type;
#else  /* defined(_WIN32) */
        typedef posix_handle file_type;
#endif /* defined(_WIN32) */

        /// <summary>
        /// Creates or reuses a context without configuring it except for the
        /// output callback.
        /// </summary>
        static std::unique_ptr<io_context> create(
            _In_opt_ CURL *curl = nullptr);

        /// <summary>
        /// Creates or reuses a context and partially configures it using the
        /// specified cURL handle
        /// </summary>
        static std::unique_ptr<io_context> create(
            _In_opt_ CURL *curl,
            _In_ const std::string& url,
            _In_ dataverse_connection::on_response_type on_response,
            _In_ dataverse_connection::on_error_type on_error,
            _In_opt_ void *client_data);

        /// <summary>
        /// Creates or reuses a context and partially configures it.
        /// </summary>
        static inline std::unique_ptr<io_context> create(
                _In_ const std::string& url,
                _In_ dataverse_connection::on_response_type on_response,
                _In_ dataverse_connection::on_error_type on_error,
                _In_opt_ void *client_data) {
            return create(nullptr, url, on_response, on_error, client_data);
        }

        /// <summary>
        /// Retrieves the context embedded in the easy handle.
        /// </summary>
        static std::unique_ptr<io_context> get(_In_opt_ CURL *curl);

        /// <summary>
        /// Opens the file at the given location for reading.
        /// </summary>
        static file_type open_file(_In_z_ const wchar_t *path);

        /// <summary>
        /// I/O callback to read the response from our buffer.
        /// </summary>
        static std::size_t CALLBACK read_request(
            _Out_writes_bytes_(cnt *size) char *dst,
            _In_ const size_t size,
            _In_ const size_t cnt,
            _In_opt_ void *context);

        /// <summary>
        /// Put the given context into the pool of recycleable context if it is
        /// a valid pointer.
        /// </summary>
        static void recycle(_Inout_ std::unique_ptr<io_context>&& context);

        /// <summary>
        /// The I/O callback passed to cURL for writing the response to our
        /// buffer.
        /// </summary>
        static std::size_t CALLBACK write_response(
            _In_reads_bytes_(cnt *element_size) const void *data,
            _In_ const std::size_t size,
            _In_ const std::size_t cnt,
            _In_ void *context);

        /// <summary>
        /// An internal data pointer that the API can use to transport arbitraty
        /// data along the request.
        /// </summary>
        void *api_data;

        /// <summary>
        /// The user data to be passed to the final callback.
        /// </summary>
        void *client_data;

        /// <summary>
        /// The library handle used for the request.
        /// </summary>
        dataverse_connection_impl::curl_type curl;

        /// <summary>
        /// The file to be uploaded, if any.
        /// </summary>
        file_type file;

        /// <summary>
        /// The form to be sent, if any.
        /// </summary>
        form_data form;

        /// <summary>
        /// The HTTP headers associated with the request, which must be stored
        /// until the request has been processed asynchronously.
        /// </summary>
        dataverse_connection_impl::string_list_type headers;

        /// <summary>
        /// The user-defined callback if the user requested a parsed API
        /// response.
        /// </summary>
        /// <remarks>
        /// <para>The type of this pointer must be created <c>inline</c> in the
        /// client programme to account for the expected version of the JSON
        /// object. It therefore cannot be type-safe.</para>
        /// </remarks>
        void *on_api_response;

        /// <summary>
        /// The user-provided error callback.
        /// </summary>
        dataverse_connection::on_error_type on_error;

        /// <summary>
        /// The user-provided response callback.
        /// </summary>
        dataverse_connection::on_response_type on_response;

        /// <summary>
        /// A pointer to the caller-provided request data.
        /// </summary>
        const byte_type *request;

        /// <summary>
        /// A user-defined deleter to be called once the request has been
        /// processed.
        /// </summary>
        dataverse_connection::data_deleter_type request_deleter;

        /// <summary>
        /// The amount of data, in bytes, from <see cref="request" /> that we
        /// have not yet delivered to cURL.
        /// </summary>
        std::size_t request_remaining;

        /// <summary>
        /// The overall size of the data designated by <see cref="request" />.
        /// </summary>
        std::size_t request_size;

        /// <summary>
        /// Receives the response from cURL.
        /// </summary>
        blob response;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        io_context(void);

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~io_context(void);

        /// <summary>
        /// Adds the given header to the header list.
        /// </summary>
        void add_header(_In_opt_z_ const char *header);

        /// <summary>
        /// Applies <see cref="headers" /> to <see cref="curl" />.
        /// </summary>
        void apply_headers(void);

        /// <summary>
        /// Sets <see cref="on_api_response" /> and configures the &quot;context
        /// switch&quot; for it.
        /// </summary>
        void configure_on_api_response(_In_opt_ void *on_api_response);

        /// <summary>
        /// Adds a content type header if non-<c>nullptr</c>.
        /// </summary>
        void content_type(_In_opt_ const wchar_t *content_type);

        /// <summary>
        /// Runs the <see cref="request_deleter" /> if one is set.
        /// </summary>
        void delete_request(void);

        /// <summary>
        /// Runs <paramref name="function" /> in a try/catch and, in case of an
        /// error, invokes the error handler.
        /// </summary>
        template<class TFunction>
        inline void handle_errors(TFunction function) {
            try {
                function();
            } catch (std::system_error ex) {
                invoke_handler(this->on_error, ex, this->client_data);
            } catch (std::exception &ex) {
                invoke_handler(this->on_error, ex, this->client_data);
            } catch (...) {
                invoke_handler(this->on_error, this->client_data);
            }
        }

        /// <summary>
        /// Sets a cURL option on the handle of the context.
        /// </summary>
        template<class... TArgs>
        inline void option(_In_ const CURLoption option, TArgs&&... arguments) {
            detail::dataverse_connection_impl::check_code(::curl_easy_setopt(
                this->curl.get(), option, std::forward<TArgs>(arguments)...));
        }

        /// <summary>
        /// Prepares the I/O context for uploading the specified file.
        /// </summary>
        void prepare_request(_In_z_ const wchar_t *path);

        /// <summary>
        /// Prepares the I/O context for uploading the specified data.
        /// </summary>
        void prepare_request(_In_reads_bytes_(cnt) const byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ const dataverse_connection::data_deleter_type deleter);

    private:

        static std::vector<std::unique_ptr<io_context>> cache;
        static std::mutex lock;
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
