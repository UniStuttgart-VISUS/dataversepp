﻿// <copyright file="io_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <system_error>
#include <vector>

#include "dataverse/blob.h"
#include "dataverse/form_data.h"
#include "dataverse/dataverse_connection.h"

#include "dataverse_connection_impl.h"


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

        /// <summary>
        /// Creates or reuses a context without configuring it except for the
        /// output callback.
        /// </summary>
        static _Ret_valid_ std::unique_ptr<io_context> create(
            _In_ CURL *curl = nullptr);

        /// <summary>
        /// Creates or reuses a context and partially configures it using the
        /// specified cURL handle
        /// </summary>
        static _Ret_valid_ std::unique_ptr<io_context> create(
            _In_ CURL *curl,
            _In_z_ const std::string& url,
            _In_ dataverse_connection::on_response_type on_response,
            _In_ dataverse_connection::on_error_type on_error,
            _In_opt_ void *client_data);

        /// <summary>
        /// Creates or reuses a context and partially configures it.
        /// </summary>
        static _Ret_valid_ std::unique_ptr<io_context> create(
            _In_z_ const std::string& url,
            _In_ dataverse_connection::on_response_type on_response,
            _In_ dataverse_connection::on_error_type on_error,
            _In_opt_ void *client_data);

        /// <summary>
        /// Retrieves the context embedded in the easy handle.
        /// </summary>
        static std::unique_ptr<io_context> get(_In_ CURL *curl);

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
        /// The user data to be passed to the final callback.
        /// </summary>
        void *client_data;

        /// <summary>
        /// The library handle used for the request.
        /// </summary>
        dataverse_connection_impl::curl_type curl;

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
        /// Adds a content type header.
        /// </summary>
        void content_type(_In_ const wchar_t *content_type);

        /// <summary>
        /// Runs the <see cref="request_deleter" /> if one is set.
        /// </summary>
        void delete_request(void);

        /// <summary>
        /// Sets a cURL option on the handle of the context.
        /// </summary>
        template<class... TArgs>
        inline void option(_In_ const CURLoption option, TArgs&&... arguments) {
            detail::dataverse_connection_impl::check_code(::curl_easy_setopt(
                this->curl.get(), option, std::forward<TArgs>(arguments)...));
        }

        void prepare_request(_In_reads_bytes_(cnt) const byte_type *data,
            _In_ const std::size_t cnt,
            _In_opt_ const dataverse_connection::data_deleter_type deleter);

    private:

        static std::vector<std::unique_ptr<io_context>> cache;
        static std::mutex lock;

        ///// <summary>
        ///// The user-defined handler to be invoked in case of an I/O error.
        ///// </summary>
        //std::function<void(_In_ dataverse_connection *,
        //    _In_ const std::system_error&,
        //    _In_ io_context *)> on_failed;

        ///// <summary>
        ///// The opration-specific handler for a successful operation.
        ///// </summary>
        ///// <remarks>
        ///// The current state of this unrestricted union is tracked by the
        ///// <see cref="operation" /> field. Make sure to keep both fields
        ///// consistent all the time. See
        ///// https://learn.microsoft.com/en-us/cpp/cpp/unions?view=msvc-160.
        ///// This is achieved by setting the operation and callback using
        ///// <see cref="operation_receive" /> etc.
        ///// </remarks>
        //union {
        //    std::function<std::size_t(_In_ dataverse_connection *,
        //        _In_ const std::size_t cnt,
        //        _In_ io_context *)> on_received;
        //    std::function<void(_In_ dataverse_connection *,
        //        _In_ io_context *)> on_sent;
        //};

        ///// <summary>
        ///// The requested kind of I/O operation.
        ///// </summary>
        ///// <remarks>
        ///// <b>Do not modify this field directly as it is used to track the
        ///// contents of <see cref="on_succeeded" />!</b>
        ///// </remarks>
        //io_operation operation;

        ///// <summary>
        ///// The actual size of the payload buffer directly following this
        ///// structure, in bytes.
        ///// </summary>
        //std::size_t size;

        ///// <summary>
        ///// Initialises a new instance.
        ///// </summary>
        //io_context(_In_ const std::size_t size = 0) noexcept;

        ///// <summary>
        ///// Finalises the instance.
        ///// </summary>
        //~io_context(void) noexcept;

        ///// <summary>
        ///// Invoke <see cref="on_failed" /> if it is set.
        ///// </summary>
        //void invoke_on_failed(_In_ dataverse_connection *connection,
        //    _In_ const std::system_error& ex);

        ///// <summary>
        ///// Invoke <see cref="on_succeeded::received" /> if it is set.
        ///// </summary>
        //std::size_t invoke_on_received(_In_ dataverse_connection *connection,
        //    _In_ const std::size_t cnt);

        ///// <summary>
        ///// Invoke <see cref="on_succeeded::sent" /> if it is set.
        ///// </summary>
        //void invoke_on_sent(_In_ dataverse_connection *connection);

        ///// <summary>
        ///// Mark the context as a receive context and set the specified
        ///// callback, which may be invalid.
        ///// </summary>
        //void operation_receive(
        //    _In_ const decltype(io_context::on_received)& on_received);

        ///// <summary>
        ///// Mark the context as a send context and set the specified
        ///// callback, which may be invalid.
        ///// </summary>
        //void operation_send(_In_ const decltype(io_context::on_sent)& on_sent);

        ///// <summary>
        ///// Resets the operation to <see cref="io_operation::unknown" /> and
        ///// clears <see cref="on_succeeded" />.
        ///// </summary>
        //void operation_unknown(void) noexcept;

    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
