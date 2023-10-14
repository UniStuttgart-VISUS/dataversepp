// <copyright file="curl_worker_state.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// Defines possible states used for the CURLM worker thread used in
    /// <see cref="dataverse_connection_impl" />.
    /// </summary>
    enum class curl_worker_state {

        /// <summary>
        /// The thread has stopped and can be started.
        /// </summary>
        stopped,

        /// <summary>
        /// The thread is currently starting up. In this state, it cannot be
        /// modified and the destructor must potentially wait for it to enter
        /// the <see cref="running" /> state.
        /// </summary>
        starting,

        /// <summary>
        /// The thread is currently running. Its state can only be transitioned
        /// to <see cref="stopping" /> from there and only the worker thread
        /// itself is allowed to do that.
        /// </summary>
        running,

        /// <summary>
        /// The thread was requested to stop, but is still running. The typical
        /// transition that can be made here is to <see cref="stopped" /> and
        /// only the worker thread is allowed to do that. Alternatively, any
        /// other thread could transition is to <see cref="starting" />, but it
        /// needs to detach the active thread in this case.
        /// </summary>
        stopping
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
