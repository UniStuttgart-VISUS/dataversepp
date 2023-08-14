// <copyright file="dataverse_connection.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/dataverse_connection.h"

#include <string>

#if defined(_WIN32)
#include <ws2ipdef.h>
#endif /* defined(_WIN32) */

#include "errors.h"
#include "io_completion_port.h"
#include "resolve.h"
#include "tls_context.h"
#include "tls_handshake.h"


/*
 * visus::dataverse::dataverse_connection::dataverse_connection
 */
visus::dataverse::dataverse_connection::dataverse_connection(void)
    : _tls(nullptr) { }


/*
 * visus::dataverse::dataverse_connection::~dataverse_connection
 */
visus::dataverse::dataverse_connection::~dataverse_connection(void) {
    delete this->_tls;
}


/*
 * visus::dataverse::dataverse_connection::connect
 */
void visus::dataverse::dataverse_connection::connect(
        _In_reads_bytes_(length) const sockaddr *address,
        _In_ const std::size_t length,
        _In_ const bool tls) {
    if (address == nullptr) {
        throw std::invalid_argument("The socket address to connect to must not "
            "be null.");
    }

    // Create a new socket and connect to the specified address.
    this->_socket = detail::socket(address->sa_family);
    this->_socket.connect(address, length);

    // Associate the socket with our I/O completion port.
    detail::io_completion_port::instance().associate(*this);
}


/*
 * visus::dataverse::dataverse_connection::connect
 */
void visus::dataverse::dataverse_connection::connect(
        _In_z_ const char_type *host,
        _In_ const std::uint16_t port,
        _In_ const bool tls) {
    const auto addresses = detail::resolve(host, port);

    if (addresses.empty()) {
        throw std::invalid_argument("The specified host could not be "
            "resolved.");
    }

    // Remember the error in the last iteration of the following loop such that
    // we know why the connection eventually failed.
    std::exception_ptr last_error;

    for (auto& a : addresses) {
        try {
            switch (a.ss_family) {
                case AF_INET:
                    this->connect(reinterpret_cast<const sockaddr *>(&a),
                        sizeof(sockaddr_in),
                        tls);
                    break;

                case AF_INET6:
                    this->connect(reinterpret_cast<const sockaddr *>(&a),
                        sizeof(sockaddr_in6),
                        tls);
                    break;

                default:
                    throw std::invalid_argument("The specified address family "
                        "is not supported");
            }

            last_error = std::exception_ptr();
            break;
        } catch (...) {
            last_error = std::current_exception();
        }
    }

    if (last_error) {
        // If the last iteration of the loop failed, rethrow the error.
        std::rethrow_exception(last_error);
    }

    // If we succeeded, the TLS context of any previous connection.
    delete this->_tls;
    this->_tls = nullptr;

    if (tls) {
        detail::tls_handshake handshake;
        auto f_context = handshake(this);
        f_context.wait();
        this->_tls = new detail::tls_context(std::move(f_context.get()));
    }
}


/*
 * visus::dataverse::dataverse_connection::disconnect
 */
void visus::dataverse::dataverse_connection::disconnect(void) {
    this->_socket.close();
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
