// <copyright file="event.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/event.h"

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <system_error>

#include <errno.h>


namespace visus {
namespace dataverse {
namespace detail {

    struct event_impl {
        std::condition_variable condition;
        std::mutex mutex;
        bool manual_reset;
        bool signalled;
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */


/*
 * visus::dataverse::create_event
 */
_Ret_valid_ visus::dataverse::event_type
visus::dataverse::create_event(_In_ const bool manual_reset,
        _In_ const bool initially_signalled) {
#if defined(DATAVERSE_EVENT_EMULATION)
    auto retval = new detail::event_impl;

    retval->manual_reset = manual_reset;
    retval->signalled = false;

    if (initially_signalled) {
        set_event(retval);
    }

    return retval;

#else /* defined(DATAVERSE_EVENT_EMULATION) */
    auto retval = ::CreateEvent(nullptr, manual_reset, initially_signalled,
        nullptr);

    if (retval == NULL) {
        throw std::system_error(::GetLastError(), std::system_category());
    }

    return retval;
#endif /* defined(DATAVERSE_EVENT_EMULATION) */
}


/*
 * visus::dataverse::destroy_event
 */
void visus::dataverse::destroy_event(_Inout_opt_ event_type& event) {
#if defined(DATAVERSE_EVENT_EMULATION)
    delete event;
    event = nullptr;

#else /* defined(DATAVERSE_EVENT_EMULATION) */
    ::CloseHandle(event);
    event = NULL;
#endif /* defined(DATAVERSE_EVENT_EMULATION) */
}


/*
 * visus::dataverse::reset_event
 */
void visus::dataverse::reset_event(_In_ event_type event) {
#if defined(DATAVERSE_EVENT_EMULATION)
    if (event == nullptr) {
        throw std::system_error(EINVAL, std::system_category());
    }

    std::unique_lock<decltype(event->mutex)> lock(event->mutex);
    event->signalled = false;

#else /* defined(DATAVERSE_EVENT_EMULATION) */
    if (!::ResetEvent(event)) {
        throw std::system_error(::GetLastError(),
            std::system_category());
    }
#endif /* defined(DATAVERSE_EVENT_EMULATION) */
}


/*
 * visus::dataverse::set_event
 */
void visus::dataverse::set_event(_In_ event_type event) {
#if defined(DATAVERSE_EVENT_EMULATION)
    if (event == nullptr) {
        throw std::system_error(EINVAL, std::system_category());
    }

    std::unique_lock<decltype(event->mutex)> lock(event->mutex);
    event->signalled = true;

    if (event->manual_reset) {
        // In case of a manual reset event, signal one thread.
        event->condition.notify_one();
    } else {
        // In case of an auto reset event, signal all threads.
        event->condition.notify_all();
    }

#else /* defined(DATAVERSE_EVENT_EMULATION) */
    if (!::SetEvent(event)) {
        throw std::system_error(::GetLastError(),
            std::system_category());
    }
#endif /* defined(DATAVERSE_EVENT_EMULATION) */
}


/*
 * visus::dataverse::wait_event
 */
void visus::dataverse::wait_event(_In_ event_type event) {
#if defined(DATAVERSE_EVENT_EMULATION)
    if (event == nullptr) {
        throw std::system_error(EINVAL, std::system_category());
    }

    std::unique_lock<decltype(event->mutex)> lock(event->mutex);
    event->condition.wait(lock, [event]() { return event->signalled; });

    if (!event->manual_reset) {
        // In case of an auto reset event, reset it now.
        event->signalled = false;
    }

#else /* defined(DATAVERSE_EVENT_EMULATION) */
    switch (::WaitForSingleObject(event, INFINITE)) {
        case WAIT_ABANDONED:
        case WAIT_OBJECT_0:
            break;

        case WAIT_TIMEOUT:
            assert(false);
        case WAIT_FAILED:
        default:
            throw std::system_error(::GetLastError(),
                std::system_category());
    }
#endif /* defined(DATAVERSE_EVENT_EMULATION) */
}


/*
 * visus::dataverse::wait_event
 */
bool visus::dataverse::wait_event(_In_ event_type event,
        _In_ const unsigned int timeout) {
#if defined(DATAVERSE_EVENT_EMULATION)
    if (event == nullptr) {
        throw std::system_error(EINVAL, std::system_category());
    }

    const auto dt = std::chrono::milliseconds(timeout);

    std::unique_lock<decltype(event->mutex)> lock(event->mutex);

    auto retval = event->condition.wait_for(lock, dt,
        [event]() { return event->signalled; });

    if (retval && !event->manual_reset) {
        // In case of an auto reset event, reset it now.
        event->signalled = false;
    }

    return retval;

#else /* defined(DATAVERSE_EVENT_EMULATION) */
    auto retval = ::WaitForSingleObject(event, timeout);

    switch (retval) {
        case WAIT_OBJECT_0:
            return true;

        case WAIT_TIMEOUT:
            return false;

        default:
            throw std::system_error(::GetLastError(),
                std::system_category());
    }
#endif /* defined(DATAVERSE_EVENT_EMULATION) */
}
