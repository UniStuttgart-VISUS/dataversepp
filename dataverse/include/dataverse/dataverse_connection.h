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
    class dataverse_connection;

    /// <summary>
    /// The callback to be invoked if an asynchronous network operation failed.
    /// </summary>
    typedef void (*network_failed_handler)(_In_ dataverse_connection *,
        _In_ const system_error_code, _In_ void *);

    /// <summary>
    /// The callback to be invoked if an asynchronous receive operation
    /// completed successfully.
    /// </summary>
    typedef void (*network_received_handler)(_In_ dataverse_connection *,
        _In_reads_bytes_(cnt) const std::uint8_t *, _In_ const std::size_t cnt,
        _In_ void *);

    /// <summary>
    /// The callback to be invoked if an asynchronous send operation completed
    /// sucessfully.
    /// </summary>
    typedef void (*network_sent_handler)(_In_ dataverse_connection *,
        _In_ void *);


    /// <summary>
    /// Represents a TCP/IP connection to the DataVerse API, which is basically a
    /// RAII wrapper around a TCP/IP socket.
    /// </summary>
    /// <remarks>
    /// <para>This API is only exposed for testing purposes.</para>
    /// </remarks>
    class DATAVERSE_API dataverse_connection {

    public:

        void __hack(void);


        void connect(_In_reads_bytes_(length) sockaddr *address,
            _In_ const std::size_t length);

        void disconnect(void);

    private:

        void receive(_In_ detail::io_context *context);

        void send(_In_ detail::io_context *context);

        visus::dataverse::detail::socket _socket;

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
