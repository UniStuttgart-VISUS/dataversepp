// <copyright file="form_data.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cstddef>
#include <string>

#include "dataverse/api.h"


namespace visus {
namespace dataverse {

    /// <summary>
    /// Provides the ability to pass the encoding of a string with the string
    /// pointer.
    /// </summary>
    /// <remarks>
    /// <para>This structure is mainly used for passing encoding information
    /// along with string parameters. It is mainly intended for keeping the
    /// interface of methods accepting narrow strings clean. The string is
    /// treated like the raw string pointer it contains, ie the caller remains
    /// owner of the memory and must make sure that the string lives as long
    /// as this structure lives. The structure will never create any copy of
    /// the data.</para>
    /// </remarks>
    class DATAVERSE_API narrow_string final {

    public:

        /// <summary>
        /// The type of a narrow string.
        /// </summary>
        typedef char *value_type;

#if defined(_WIN32)
        /// <summary>
        /// The type used to specify a code page.
        /// </summary>
        typedef unsigned int code_page_type;
#else /* defined(_WIN32) */
        typedef char *code_page_type;
#endif /* defined(_WIN32) */

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name="value">The narrow string pointer. The caller remains
        /// owner of the memory and must ensure that it lives as long as the
        /// new object.</param>
        /// <param name="code_page">The code page used in
        /// <paramref name="value" />. Note that on non-Windows systems, this
        /// is a string and the caller must make sure that it lives as long
        /// as the new object.</param>
        inline narrow_string(_In_opt_z_ value_type value,
                _In_ const code_page_type code_page) noexcept
            : _code_page(code_page), _value(value) { }

        /// <summary>
        /// Answer the code page used for the string.
        /// </summary>
        /// <returns>The code page used for the string.</returns>
        inline const code_page_type code_page(void) const noexcept {
            return this->_code_page;
        }

        /// <summary>
        /// Answer the string.
        /// </summary>
        /// <returns>The string.</returns>
        inline _Ret_maybenull_z_ const value_type value(void) const noexcept {
            return this->_value;
        }

        /// <summary>
        /// Answer the string.
        /// </summary>
        /// <returns>The string.</returns>
        inline _Ret_maybenull_z_ value_type value(void) noexcept {
            return this->_value;
        }

        /// <summary>
        /// Answer the string.
        /// </summary>
        /// <returns>The string.</returns>
        inline _Ret_maybenull_z_ operator const value_type(
                void) const noexcept {
            return this->_value;
        }

        /// <summary>
        /// Answer the string.
        /// </summary>
        /// <returns>The string.</returns>
        inline _Ret_maybenull_z_ operator value_type(void) noexcept {
            return this->_value;
        }

    private:

        code_page_type _code_page;
        value_type _value;
    };


    /// <summary>
    /// The constant variant of <see cref="narrow_string" />.
    /// </summary>
    class DATAVERSE_API const_narrow_string final {

    public:

        /// <summary>
        /// The type of a narrow string.
        /// </summary>
        typedef const char *value_type;

        /// <summary>
        /// The type used to specify a code page.
        /// </summary>
        typedef narrow_string::code_page_type code_page_type;

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name="value">The narrow string pointer. The caller remains
        /// owner of the memory and must ensure that it lives as long as the
        /// new object.</param>
        /// <param name="code_page">The code page used in
        /// <paramref name="value" />. Note that on non-Windows systems, this
        /// is a string and the caller must make sure that it lives as long
        /// as the new object.</param>
        inline const_narrow_string(_In_opt_z_ value_type value,
                _In_ const code_page_type code_page) noexcept
            : _code_page(code_page), _value(value) { }

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <remarks>
        /// This constructor allows for implicit conversions from
        /// non-constant <see cref="narrow_string" />s.
        /// </remarks>
        /// <param name="str">The <see cref="narrow_string" /> to convert.
        /// </param>
        inline const_narrow_string(_In_ const narrow_string& str) noexcept
            : _code_page(str.code_page()), _value(str) { }

        /// <summary>
        /// Answer the code page used for the string.
        /// </summary>
        /// <returns>The code page used for the string.</returns>
        inline const code_page_type code_page(void) const noexcept {
            return this->_code_page;
        }

        /// <summary>
        /// Answer the string.
        /// </summary>
        /// <returns>The string.</returns>
        inline _Ret_maybenull_z_ value_type value(void) const noexcept {
            return this->_value;
        }

        /// <summary>
        /// Answer the string.
        /// </summary>
        /// <returns>The string.</returns>
        inline _Ret_maybenull_z_ operator value_type(void) const noexcept {
            return this->_value;
        }

        /// <summary>
        /// Assign a non-constant narrow string.
        /// </summary>
        /// <param name="rhs">The right-hand side operand.</param>
        /// <returns><c>*this</c>.</returns>
        inline const_narrow_string& operator =(
                _In_ const narrow_string& rhs) noexcept {
            this->_code_page = rhs.code_page();
            this->_value = rhs;
            return *this;
        }

    private:

        code_page_type _code_page;
        value_type _value;
    };

} /* namespace dataverse */
} /* namespace visus */
