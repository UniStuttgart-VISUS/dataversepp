// <copyright file="io_operation.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// Defines possible operations that can be queued into a
    /// <see cref="io_completion_port" />.
    /// </summary>
    enum class io_operation {

        /// <summary>
        /// The kind of operation is unknown, which will yield an error in the
        /// I/O completion port.
        /// </summary>
        unknown,

        /// <summary>
        /// A asynchronous socket send.
        /// </summary>
        send,

        /// <summary>
        /// An asyncrhonous socket receive.
        /// </summary>
        receive
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */
