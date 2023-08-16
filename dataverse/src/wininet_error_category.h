// <copyright file="wininet_error_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#if defined(_WIN32)
#include <string>
#include <system_error>


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// An error category for WinINet error codes.
    /// </summary>
    class wininet_error_category final : public std::error_category {

    public:

        using std::error_category::error_category;

        /// <summary>
        /// Converts the error code into a portable description.
        /// </summary>
        /// <param name="error">The error code to be converted.</param>
        /// <returns>The portable description of the WinINet error.</returns>
        std::error_condition default_error_condition(
            _In_ int error) const noexcept override;

        /// <summary>
        /// Convert the given error code into a string.
        /// </summary>
        /// <param name="error">The error code to get the message for.</param>
        /// <returns>The error message associated with the error code.
        /// </returns>
        std::string message(_In_ int error) const override;

        /// <summary>
        /// Answer the name of the error category.
        /// </summary>
        /// <returns>The name of the category.</returns>
        inline _Ret_z_ const char *name(void) const noexcept override {
            return "WinINet";
        }
    };


    /// <summary>
    /// Answer the one and only <see cref="wininet_error_category" />.
    /// </summary>
    /// <returns>The only instance of <see cref="wininet_error_category" />.
    /// </returns>
    _Ret_valid_ const std::error_category& wininet_category(void) noexcept;

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */

#endif /* defined(_WIN32) */
