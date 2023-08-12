// <copyright file="io_completion_port.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "io_completion_port.h"

#include <algorithm>
#include <cassert>

#include "errors.h"


/*
 * visus::dataverse::detail::io_completion_port::instance
 */
visus::dataverse::detail::io_completion_port&
visus::dataverse::detail::io_completion_port::instance(void) {
    static io_completion_port retval;
    return retval;
}


/*
 * visus::dataverse::detail::io_completion_port::~io_completion_port
 */
visus::dataverse::detail::io_completion_port::~io_completion_port(void) {
    this->post_status(0, 0, nullptr);

    ::OutputDebugString(_T("The I/O completion helper waits for its threads ")
        _T("to exit.\r\n"));
    for (auto& t : this->_threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    ::OutputDebugString(_T("All I/O completion threads have exited.\r\n"));
}


/*
 * visus::dataverse::detail::io_completion_port::associate
 */
void visus::dataverse::detail::io_completion_port::associate(
        _In_ dataverse_connection& connection) {
#if defined(_WIN32)
    const auto key = reinterpret_cast<ULONG_PTR>(std::addressof(connection));
    auto handle = ::CreateIoCompletionPort(
        static_cast<HANDLE>(connection._socket), this->_handle.get(), key, 0);
    if (handle == NULL) {
        throw std::system_error(::GetLastError(), std::system_category());
    }

#else /* defined(_WIN32) */

#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::detail::io_completion_port::context
 */
visus::dataverse::detail::io_completion_port::context_type
visus::dataverse::detail::io_completion_port::context(
        _In_ const io_operation operation,
        _In_ dataverse_connection *connection,
        _In_ const std::size_t size,
        _In_ const network_failed_handler on_failed,
        _In_opt_ void *context) {
    auto retval = this->context(size);
    assert(retval->size >= size);
    retval->operation = operation;
    retval->on_failed = on_failed;
    retval->user_data = context;
    return retval;
}



/*
 * visus::dataverse::detail::io_completion_port::receive
 */
void visus::dataverse::detail::io_completion_port::receive(
        _In_ socket& socket,
        _Inout_ context_type&& context) {
#if defined(_WIN32)
    DWORD received = 0;
    if (::WSARecv(socket,
            &context->buffer, 1,
            &received,
            0,
            &context->overlapped,
            nullptr)
            != SOCKET_ERROR) {
        // The operation completed synchronously, so we can recycle the context.
        this->recycle(std::move(context));

    } else {
        // Check whether we have a failure or the operation is just pending.
        const auto error = ::WSAGetLastError();
        if (error == WSA_IO_PENDING) {
            // Release the context as it is now owned by the port.
            context.release();

        } else {
            // In case of failure, we do not touch the context and leave it with
            // the caller.
            throw std::system_error(error, std::system_category());
        }
    }

#else /* defined(_WIN32) */
    throw "TODO: implement send on linux"
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::detail::io_completion_port::send
 */
void visus::dataverse::detail::io_completion_port::send(
        _In_ socket& socket,
        _Inout_ context_type&& context) {
#if defined(_WIN32)
    DWORD sent = 0;
    if (::WSASend(socket,
            &context->buffer, 1,
            &sent,
            0,
            &context->overlapped,
            nullptr)
            != SOCKET_ERROR) {
        // The operation completed synchronously, so we can recycle the context.
        this->recycle(std::move(context));

    } else {
        // Check whether we have a failure or the operation is just pending.
        const auto error = ::WSAGetLastError();
        if (error == WSA_IO_PENDING) {
            // Release the context as it is now owned by the port.
            context.release();

        } else {
            // In case of failure, we do not touch the context and leave it with
            // the caller.
            throw std::system_error(error, std::system_category());
        }
    }
#else /* defined(_WIN32) */
    throw "TODO: implement send on linux"
#endif /* defined(_WIN32) */
}


#if defined(_WIN32)
/*
 * visus::dataverse::detail::io_completion_port::context
 */
visus::dataverse::detail::io_completion_port::context_type
visus::dataverse::detail::io_completion_port::context(
        _In_ OVERLAPPED *overlapped) {
    auto context = reinterpret_cast<io_context *>(overlapped);
    return context_type(context);
}
#endif /* defined(_WIN32) */


/*
 * visus::dataverse::detail::io_completion_port::io_completion_port
 */
visus::dataverse::detail::io_completion_port::io_completion_port(void) {
#if defined(_WIN32)
    this->_handle.reset(::CreateIoCompletionPort(INVALID_HANDLE_VALUE,
        NULL, 0, 0));
    if (!this->_handle) {
        throw std::system_error(::GetLastError(), std::system_category());
    }
#else /* defined(_WIN32) */

#endif /* defined(_WIN32) */

    // Fill the thread pool.
    this->_threads.reserve(std::thread::hardware_concurrency());
    for (std::size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
        this->_threads.emplace_back(&io_completion_port::pool_thread, this);
    }
}



/*
 * visus::dataverse::detail::io_completion_port::context
 */
visus::dataverse::detail::io_completion_port::context_type
visus::dataverse::detail::io_completion_port::context(
        _In_ const std::size_t size) {
    // Find a buffer that is large enough that we can reuse.
    {
        std::lock_guard<decltype(this->_contexts_lock)> l(this->_contexts_lock);
        auto it = std::find_if(this->_contexts.begin(), this->_contexts.end(),
            [size](context_type& c) { return (c->size > size); });
        if (it != this->_contexts.end()) {
            auto retval = std::move(*it);
#if defined(_WIN32)
            assert(retval->size >= size);
            retval->buffer.buf = reinterpret_cast<CHAR *>(retval->payload());
            retval->buffer.len = static_cast<ULONG>(size);
#endif /* defined(_WIN32) */
            this->_contexts.erase(it);
            return retval;
        }
    }

    // If we are here, there is no suitable context to reuse, so we create a new
    // one.
    auto retval = context_type(new (size) io_context(size));
    assert(retval->size >= size);
#if defined(_WIN32)
    retval->buffer.buf = reinterpret_cast<CHAR *>(retval->payload());
    retval->buffer.len = static_cast<ULONG>(size);
#endif /* defined(_WIN32) */
    return retval;
}


/*
 * visus::dataverse::detail::io_completion_port::pool_thread
 */
void visus::dataverse::detail::io_completion_port::pool_thread(void) {
#if defined(_WIN32)
    ULONG_PTR completion_key = 0llu;
    LPOVERLAPPED overlapped = nullptr;
    bool running = true;
    DWORD transferred = 0lu;

    ::OutputDebugString(_T("An I/O completion pool thread has started.\r\n"));

    while (running) {
        // Get the next I/O completion packet.
        if (::GetQueuedCompletionStatus(this->_handle.get(),
                &transferred,
                &completion_key,
                &overlapped,
                INFINITE)) {
            auto connection = reinterpret_cast<dataverse_connection *>(
                completion_key);

            if ((transferred == 0)
                    && (completion_key == 0)
                    && (overlapped == nullptr)) {
                // Zero size, zero key and nullptr for overlapped is the exit
                // signal for the thread pool. Repost it until all worker
                // threads have exited.
                ::OutputDebugString(_T("An I/O completion worker has been ")
                    _T("requested to exit.\r\n"));
                this->post_status(0, 0, nullptr);
                running = false;

            } else {
                // Get the context of the I/O completion packet and perfrom the
                // operation based on the type.
                ::OutputDebugString(_T("An I/O completion worker processes a ")
                    _T("completed operation.\r\n"));
                auto context = io_completion_port::context(overlapped);

                switch (context->operation) {
                    case io_operation::receive:
                        context->invoke_on_received(connection, transferred);
                        break;

                    case io_operation::send:
                        if (context->buffer.len > transferred) {
                            // We were unable to send everything at once, so
                            // continue sending.
                            context->buffer.buf += transferred;
                            context->buffer.len -= transferred;
                            try {
                                this->send(connection->_socket,
                                    std::move(context));
                                assert(context == nullptr);
                            } catch (std::system_error ex) {
                                // If the continuation failed, consider the
                                // whole send failed and notify the user.
                                context->invoke_on_failed(connection, ex);
                            }

                        } else {
                            // We sent everything, so notify the user.
                            context->invoke_on_sent(connection);
                        }
                        break;

                    default:
                        assert(false);
                        break;
                }

                this->recycle(std::move(context));
            } /* if ((transferred == 0) ... */

        } else {
            // If GetQueuedCompletionStatus returns FALSE, there are two cases
            // to be handled: If the overlapped structure is null, no packet
            // was dequeued and we need to check for ERROR_ABANDONED_WAIT_0 to
            // find out whether the I/O completion port was closes while there
            // was still outstanding I/O.
            // If the overlapped structure is not null, a failed packet has
            // been dequeued and we need to report the I/O error.
            ::OutputDebugString(_T("An I/O completion worker processes an ")
                _T("error.\r\n"));
            const auto error = ::GetLastError();
            const auto packet_dequeued = (overlapped != nullptr);

            // Free the context, if necessary.
            if (packet_dequeued) {
                auto context = io_completion_port::context(overlapped);
                auto connection = reinterpret_cast<dataverse_connection *>(
                    completion_key);

                switch (context->operation) {
                    case io_operation::receive:
                    case io_operation::send:
                        context->invoke_on_failed(connection, error);
                        break;

                    default:
                        // This is an invalid operation, so do not notify anyone
                        // and fail it.
                        throw std::system_error(error, std::system_category());
                }

                // In case the user has handled the error, recycle the context
                // for future use.
                this->recycle(std::move(context));

            } else if (error == ERROR_ABANDONED_WAIT_0) {
                // The handle was closed while I/O was pending.
                throw std::system_error(error, std::system_category());
            }
        }
    } /* while (running) */

#else /* defined(_WIN32) */
    throw "TODO: implement linux I/O worker."

#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::detail::io_completion_port::recycle
 */
void visus::dataverse::detail::io_completion_port::recycle(
        _Inout_ context_type&& context) {
    if (context != nullptr) {
        std::lock_guard<decltype(this->_contexts_lock)> l(this->_contexts_lock);
        this->_contexts.push_back(std::move(context));
    }
}


#if defined(_WIN32)
/*
 * visus::dataverse::detail::io_completion_port::post_status
 */
void visus::dataverse::detail::io_completion_port::post_status(
        _In_ const DWORD cnt,
        _In_ const ULONG_PTR key,
        _In_ LPOVERLAPPED overlapped) {
    if (!::PostQueuedCompletionStatus(this->_handle.get(), cnt, key,
            overlapped)) {
        throw std::system_error(::GetLastError(), std::system_category());
    }
}
#endif /* defined(_WIN32) */
