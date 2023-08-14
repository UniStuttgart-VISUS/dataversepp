// <copyright file="com_error_category.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#if defined(_WIN32)
#include <system_error>

#include <WinSock2.h>
#include <Windows.h>

#include "dataverse/api.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// An error category for <c>HRESULT</c>s from COM.
    /// </summary>
    class com_error_category final : public std::error_category {

    public:

        using std::error_category::error_category;

        /// <summary>
        /// Converts the error code into a portable description.
        /// </summary>
        /// <param name="error">The error code to be converted.</param>
        /// <returns>The portable description of the COM error.</returns>
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
            return "Component Object Model";
        }
    };


    /// <summary>
    /// Answer the one and only <see cref="com_error_category" />.
    /// </summary>
    /// <returns>The only instance of <see cref="com_error_category" />.
    /// </returns>
    _Ret_valid_ const std::error_category& com_category(void) noexcept;

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */


namespace std {

    /// <summary>
    /// Tell STL that <c>HRESULT</c>s are error codes.
    /// </summary>
    template<> struct is_error_code_enum<HRESULT> : public true_type { };

    /// <summary>
    /// Allow STL to convert an COM error code into a generic error code.
    /// </summary>
    /// <param name="e">The OpenSSL error to be converted.</param>
    /// <returns>The generic error code.</returns>
    inline std::error_code make_error_code(
            _In_ const HRESULT e) noexcept {
        return std::error_code(static_cast<int>(e),
            visus::dataverse::detail::com_category());
    }
}

#endif /* defined(_WIN32) */
