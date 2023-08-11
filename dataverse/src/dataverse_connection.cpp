// <copyright file="dataverse_connection.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/dataverse_connection.h"

#include "errors.h"
#include "io_completion_port.h"


void visus::dataverse::dataverse_connection::__hack(void) {
    auto ctx = detail::io_completion_port::instance().context(detail::io_operation::receive, this, 42, nullptr, nullptr);
    ::ZeroMemory(ctx->payload(), 42);
}


/*
 * visus::dataverse::dataverse_connection::connect
 */
void visus::dataverse::dataverse_connection::connect(
        _In_reads_bytes_(length) sockaddr *address,
        _In_ const std::size_t length) {
    if (address == nullptr) {
        throw std::invalid_argument("The socket address to connect to must not "
            "be null.");
    }

    // Create a new socket and connect to the specified address.
    this->_socket = detail::socket(address->sa_family);
    this->_socket.connect(address, length);

    // Associate the socket with our I/O completion port.
    detail::io_completion_port::instance().associate(this->_socket);
}


/*
 * visus::dataverse::dataverse_connection::disconnect
 */
void visus::dataverse::dataverse_connection::disconnect(void) {
    this->_socket.close();
}


/*
 * visus::dataverse::dataverse_connection::receive
 */
void visus::dataverse::dataverse_connection::receive(
        _In_ detail::io_context *context) {
#if defined(_WIN32)
    DWORD received = 0;
    if (::WSARecv(this->_socket,
            &context->buffer, 1,
            &received,
            0,
            &context->overlapped,
            nullptr)
            == SOCKET_ERROR) {
        const auto error = ::WSAGetLastError();
        if (error != WSA_IO_PENDING) {
            throw std::system_error(error, std::system_category());
        }
    }
#else /* defined(_WIN32) */
    throw "TODO: implement send on linux"
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::dataverse_connection::send
 */
void visus::dataverse::dataverse_connection::send(
        _In_ detail::io_context *context) {
#if defined(_WIN32)
    DWORD sent = 0;
    if (::WSASend(this->_socket,
            &context->buffer, 1,
            &sent,
            0,
            &context->overlapped,
            nullptr)
            == SOCKET_ERROR) {
        const auto error = ::WSAGetLastError();
        if (error != WSA_IO_PENDING) {
            throw std::system_error(error, std::system_category());
        }
    }
#else /* defined(_WIN32) */
    throw "TODO: implement send on linux"
#endif /* defined(_WIN32) */
}


///*
// * ::dataverse_connect
// */
//extern "C" system_error_code visus::dataverse::dataverse_connect(
//    _Out_ dataverse_connection * dst,
//    _In_ sockaddr * address,
//    _In_ const std::size_t length) {
//    try {
//        *dst = new dataverse_connection_context();
//        (**dst)._socket = visus::dataverse::detail::socket(address->sa_family);
//        (**dst)._socket.connect(address, length);
//
//    } catch (std::bad_alloc) {
//        return ERROR_OUTOFMEMORY;
//    } catch (std::system_error ex) {
//        return ex.code().value();
//    }
//}
//
//
///*
// * ::dataverse_disconnect
// */
//extern "C" system_error_code dataverse_disconnect(
//    _In_ dataverse_connection connection) {
//    delete connection;
//    return ERROR_SUCCESS;
//}
//
//
///*
// * ::dataverse_receive
// */
//extern "C" system_error_code dataverse_receive(
//    _In_ dataverse_connection connection,
//    _Out_writes_bytes_(cnt) std::uint8_t * dst,
//    _In_ const std::size_t cnt,
//    _In_ const dataverse_on_net_received on_received,
//    _In_ const dataverse_on_net_failed on_failed,
//    _In_opt_ void *context) {
//    return ERROR_SUCCESS;
//}
//
//
///*
// * ::dataverse_send
// */
//extern "C" system_error_code dataverse_send(
//    _In_ dataverse_connection connection,
//    _In_reads_bytes_(cnt) const std::uint8_t * src,
//    _In_ const std::size_t cnt,
//    _In_ const dataverse_on_net_received on_received,
//    _In_ const dataverse_on_net_failed on_failed,
//    _In_opt_ void *context) {
//    return ERROR_SUCCESS;
//}
