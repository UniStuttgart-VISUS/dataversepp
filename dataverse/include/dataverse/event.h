// <copyright file="event.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2018 - 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#if defined(_WIN32)
#include <Windows.h>
#else /*defined(_WIN32) */
#define DATAVERSE_EVENT_EMULATION
#endif /* defined(_WIN32) */

#include "dataverse/api.h"


namespace visus {
namespace dataverse {

    /* Forward declarations. */
#if defined(DATAVERSE_EVENT_EMULATION)
    namespace detail { struct event_impl; }
#endif /* defined(DATAVERSE_EVENT_EMULATION) */

    /// <summary>
    /// Represents an opaque event handle.
    /// </summary>
    /// <remarks>
    /// Callers should not make any assumptions about the internals of the event
    /// handle.
    /// </remarks>
#if defined(DATAVERSE_EVENT_EMULATION)
    typedef detail::event_impl *event_type;
#else /* defined(DATAVERSE_EVENT_EMULATION) */
    typedef HANDLE event_type;
#endif /* defined(DATAVERSE_EVENT_EMULATION) */

    /// <summary>
    /// Allocates a new event object.
    /// </summary>
    /// <param name="manual_reset"></param>
    /// <param name="initially_signalled"></param>
    /// <returns></returns>
    /// <exception cref="std::bad_alloc"></exception>
    /// <exception cref="std::system_error"></exception>
    extern _Ret_valid_ event_type DATAVERSE_API create_event(
        _In_ const bool manual_reset = false,
        _In_ const bool initially_signalled = false);

    /// <summary>
    /// Destroys the given event.
    /// </summary>
    /// <param name="event">The event to be released.</param>
    extern void DATAVERSE_API destroy_event(
        _Inout_opt_ event_type& event);

    /// <summary>
    /// Resets a manual reset event.
    /// </summary>
    /// <param name="event">The handle of the event to reset.</param>
    /// <exception cref="std::system_error">In case the operation failed.
    /// </exception>
    extern void DATAVERSE_API reset_event(_In_ event_type event);

    /// <summary>
    /// Signals an event.
    /// </summary>
    /// <param name="event">The handle of the event to set.</param>
    /// <exception cref="std::system_error">In case the operation failed.
    /// </exception>
    extern void DATAVERSE_API set_event(_In_ event_type event);

    /// <summary>
    /// Waits for an event to become signalled.
    /// </summary>
    /// <param name="event">The handle of the event to wait for.</param>
    /// <exception cref="std::system_error">In case the operation failed.
    /// </exception>
    extern void DATAVERSE_API wait_event(_In_ event_type event);

    /// <summary>
    /// Waits for an event to become signalled.
    /// </summary>
    /// <param name="event">The handle of the event to wait for.</param>
    /// <param name="timeout">The timeout to wait in milliseconds.</param>
    /// <returns><c>true</c> if the event was signalled in time, <c>false</c> if
    /// the operation timed out without another error.</returns>
    /// <exception cref="std::system_error">In case the operation failed.
    /// </exception>
    extern bool DATAVERSE_API wait_event(_In_ event_type event,
        _In_ const unsigned int timeout);

} /* namespace dataverse */
} /* namespace visus */
