// <copyright file="dataverse_connection_context.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "dataverse/socket.h"


namespace visus {
namespace dataverse {

    /* Forward declarations. */
    namespace detail { class io_completion_port; }
    namespace detail { class io_context; }
    namespace detail { class tls_context; }
    class dataverse_connection;

    /// <summary>
    /// The handler to be invoked if a graceful disconnect was detected.
    /// </summary>
    typedef void (*network_disconnected_handler)(_In_ dataverse_connection *,
        _In_opt_ void *);

    /// <summary>
    /// The callback to be invoked if an asynchronous network operation failed.
    /// </summary>
    typedef void (*network_failed_handler)(_In_ dataverse_connection *,
        _In_ const char_type *, _In_opt_ void *);

    /// <summary>
    /// The callback to be invoked if an asynchronous receive operation
    /// completed successfully.
    /// </summary>
    typedef void (*network_received_handler)(_In_ dataverse_connection *,
        _In_reads_bytes_(cnt) const std::uint8_t *, _In_ const std::size_t cnt,
        _In_opt_ void *);

    /// <summary>
    /// The callback to be invoked if an asynchronous send operation completed
    /// sucessfully.
    /// </summary>
    typedef void (*network_sent_handler)(_In_ dataverse_connection *,
        _In_opt_ void *);


    /// <summary>
    /// Represents a TCP/IP connection to the DataVerse API, which is basically a
    /// RAII wrapper around a TCP/IP socket.
    /// </summary>
    /// <remarks>
    /// <para>This API is only exposed for testing purposes.</para>
    /// </remarks>
    class DATAVERSE_API dataverse_connection {

    public:

        dataverse_connection(void);

        dataverse_connection(const dataverse_connection&) = delete;

        dataverse_connection(dataverse_connection&&) = delete;

        ~dataverse_connection(void);

        void connect(_In_reads_bytes_(length) const sockaddr *address,
            _In_ const std::size_t length,
            _In_ const bool tls = true);

        void connect(_In_z_ const char_type *host,
            _In_ const std::uint16_t port,
            _In_ const bool tls = true);

        void disconnect(void);

        dataverse_connection& operator =(const dataverse_connection&) = delete;

        dataverse_connection& operator =(dataverse_connection&&) = delete;

    private:

        detail::socket _socket;
        detail::tls_context *_tls;

        // Allow the completion port access to the socket.
        friend class visus::dataverse::detail::io_completion_port;

    };

} /* namespace dataverse */
} /* namespace visus */


  //    /// <summary>
//    /// 
//    /// </summary>
//    /// <param name="address"></param>
//    /// <param name="length"></param>
//    /// <returns></returns>
//DATAVERSE_EXTERN system_error_code DATAVERSE_API dataverse_connect(
//    _Out_ dataverse_connection **dst,
//    _In_reads_bytes_(length) sockaddr *address,
//    _In_ const std::size_t length);
//
///// <summary>
///// 
///// </summary>
///// <param name="connection"></param>
///// <returns></returns>
//DATAVERSE_EXTERN system_error_code DATAVERSE_API dataverse_disconnect(
//    _In_ dataverse_connection *connection);
//
///// <summary>
///// 
///// </summary>
///// <param name="connection"></param>
///// <param name="dst"></param>
///// <param name="cnt"></param>
///// <param name="on_received"></param>
///// <param name="on_failed"></param>
///// <param name="context"></param>
///// <returns></returns>
//DATAVERSE_EXTERN system_error_code DATAVERSE_API dataverse_receive(
//    _In_ dataverse_connection *connection,
//    _In_ const std::size_t cnt,
//    _In_ const network_received_handler on_received,
//    _In_ const network_failed_handler on_failed,
//    _In_opt_ void *context);
//
///// <summary>
///// 
///// </summary>
///// <param name="connection"></param>
///// <param name="src"></param>
///// <param name="cnt"></param>
///// <param name="on_sent"></param>
///// <param name="on_failed"></param>
///// <param name="context"></param>
///// <returns></returns>
//DATAVERSE_EXTERN system_error_code DATAVERSE_API dataverse_send(
//    _In_ dataverse_connection *connection,
//    _In_reads_bytes_(cnt) const std::uint8_t *src,
//    _In_ const std::size_t cnt,
//    _In_ const network_sent_handler on_sent,
//    _In_ const network_failed_handler on_failed,
//    _In_opt_ void *context);
