// <copyright file="form_data.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

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
        typedef const char *code_page_type;
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
        _Field_z_ value_type _value;
    };


    /// <summary>
    /// Creates a new instance of <see cref="const_narrow_string" />.
    /// </summary>
    /// <param name="string">The narrow string pointer. The caller remains
    /// owner of the memory and must ensure that it lives as long as the
    /// new object.</param>
    /// <param name="code_page">The code page used in
    /// <paramref name="string" />. Note that on non-Windows systems, this
    /// is a string and the caller must make sure that it lives as long
    /// as the new object.</param>
    /// <returns>A new <see cref="const_narrow_string" />.</returns>
    inline const_narrow_string make_const_narrow_string(
            _In_opt_z_ const char *string,
            _In_ const narrow_string::code_page_type code_page) noexcept {
        return const_narrow_string(string, code_page);
    }

    /// <summary>
    /// Creates a new instance of <see cref="const_narrow_string" />.
    /// </summary>
    /// <remarks>
    /// <para>This function is dangerous to use as it requires
    /// <paramref name="string" /> to stay constant while the return value of
    /// the function is in use!</para>
    /// </remarks>
    /// <param name="string">The narrow string to point to. The caller must
    /// make sure that the pointer returned by
    /// <see cref="std::string::c_str" /> remains unchanged as long as the
    /// new object lives. Therefore, <paramref name="value" /> must not
    /// destroyed nor changed while the new object is used.</param>
    /// <param name="code_page">The code page used in
    /// <paramref name="string" />. Note that on non-Windows systems, this
    /// is a string and the caller must make sure that it lives as long
    /// as the new object.</param>
    /// <returns>A new <see cref="const_narrow_string" />.</returns>
    inline const_narrow_string make_const_narrow_string(
            _In_ const std::string& string,
            _In_ const narrow_string::code_page_type code_page) noexcept {
        return const_narrow_string(string.c_str(), code_page);
    }

    /// <summary>
    /// This function always fails.
    /// </summary>
    /// <exception cref="std::logic_error">Unconditionally.</exception>
    inline const_narrow_string make_const_narrow_string(
            _In_ std::string&&,
            _In_ const narrow_string::code_page_type) {
        throw std::logic_error("make_const_narrow_string must never be used "
            "with temporary STL strings. Make sure to read the documentation "
            "in order to understand why this is wrong.");
    }

    /// <summary>
    /// Creates a new instance of <see cref="narrow_string" />.
    /// </summary>
    /// <param name="string">The narrow string pointer. The caller remains
    /// owner of the memory and must ensure that it lives as long as the
    /// new object.</param>
    /// <param name="code_page">The code page used in
    /// <paramref name="string" />. Note that on non-Windows systems, this
    /// is a string and the caller must make sure that it lives as long
    /// as the new object.</param>
    /// <returns>A new <see cref="narrow_string" />.</returns>
    inline narrow_string make_narrow_string(_In_opt_z_ char *string,
            _In_ const narrow_string::code_page_type code_page) noexcept {
        return narrow_string(string, code_page);
    }

    /// <summary>
    /// Creates a new instance of <see cref="const_narrow_string" />.
    /// </summary>
    /// <param name="string">The narrow string pointer. The caller remains
    /// owner of the memory and must ensure that it lives as long as the
    /// new object.</param>
    /// <param name="code_page">The code page used in
    /// <paramref name="string" />. Note that on non-Windows systems, this
    /// is a string and the caller must make sure that it lives as long
    /// as the new object.</param>
    /// <returns>A new <see cref="const_narrow_string" />.</returns>
    inline const_narrow_string make_narrow_string(_In_opt_z_ const char *string,
            _In_ const narrow_string::code_page_type code_page) noexcept {
        return const_narrow_string(string, code_page);
    }

    /// <summary>
    /// Creates a new instance of <see cref="const_narrow_string" />.
    /// </summary>
    /// <remarks>
    /// <para>This function is dangerous to use as it requires
    /// <paramref name="string" /> to stay constant while the return value of
    /// the function is in use!</para>
    /// </remarks>
    /// <param name="string">The narrow string to point to. The caller must
    /// make sure that the pointer returned by
    /// <see cref="std::string::c_str" /> remains unchanged as long as the
    /// new object lives. Therefore, <paramref name="value" /> must not
    /// destroyed nor changed while the new object is used.</param>
    /// <param name="code_page">The code page used in
    /// <paramref name="string" />. Note that on non-Windows systems, this
    /// is a string and the caller must make sure that it lives as long
    /// as the new object.</param>
    /// <returns>A new <see cref="const_narrow_string" />.</returns>
    inline const_narrow_string make_narrow_string(
            _In_ const std::string& string,
            _In_ const narrow_string::code_page_type code_page) noexcept {
        return const_narrow_string(string.c_str(), code_page);
    }

    /// <summary>
    /// This function always fails.
    /// </summary>
    /// <exception cref="std::logic_error">Unconditionally.</exception>
    inline const_narrow_string make_narrow_string(
            _In_ std::string&&,
            _In_ const narrow_string::code_page_type) {
        throw std::logic_error("make_narrow_string must never be used "
            "with temporary STL strings. Make sure to read the documentation "
            "in order to understand why this is wrong.");
    }

    /// <summary>
    /// Convert a range of narrow character strings using the specified
    /// <paramref name="code_page" /> into <see cref="narrow_string" />s that
    /// associate the string with the code page.
    /// </summary>
    /// <typeparam name="TIn">The type of the input iterator, which must iterate
    /// over <c>char *</c>, <c>const char *</c> or <see cref="std::string" />.
    /// Make sure to read the documentation of <see cref="make_narrow_string" />
    /// when using the latter to understand the dangers of doing this.
    /// </typeparam>
    /// <typeparam name="TOut">The type of the output iterator, which must
    /// iterate over <see cref="const_narrow_string" /> or
    /// <see cref="narrow_string" />, depending on the input.
    /// </typeparam>
    /// <param name="oit">The output iterator tow write to.</param>
    /// <param name="begin">The start of the range of strings to convert.
    /// </param>
    /// <param name="end">The end of the range of strings to convert.</param>
    /// <param name="cp">The code page of all of the strings.</param>
    /// <returns>The iterator past the last output written.</returns>
    template<class TIn, class TOut>
    inline TOut make_narrow_strings(_In_ TOut oit, _In_ const TIn begin,
            _In_ const TIn end, _In_ const narrow_string::code_page_type cp) {
        for (auto it = begin; it != end; ++it) {
            *oit++ = make_narrow_string(*it, cp);
        }
        return oit;
    }

    /// <summary>
    /// Convert a range of narrow character strings using the specified
    /// <paramref name="code_page" /> into an array of
    /// <see cref="narrow_string" />s that associate the string with the code
    /// page.
    /// </summary>
    /// <typeparam name="TIn">The type of the input iterator, which must iterate
    /// over <c>char *</c>, <c>const char *</c> or <see cref="std::string" />.
    /// Make sure to read the documentation of <see cref="make_narrow_string" />
    /// when using the latter to understand the dangers of doing this.
    /// </typeparam>
    /// <param name="begin">The start of the range of strings to convert.
    /// </param>
    /// <param name="end">The end of the range of strings to convert.</param>
    /// <param name="cp">The code page of all of the strings.</param>
    /// <returns>A vector of <see cref="narrow_string" />s pointing to each of
    /// the elements in <paramref name="strings" />.</returns>
    template<class TIn>
    inline auto make_narrow_strings(_In_ const TIn begin,
            _In_ const TIn end, _In_ const narrow_string::code_page_type cp)
        -> std::vector<decltype(make_narrow_string(std::declval<
            typename TIn::value_type>(), cp))> {
        typedef decltype(make_narrow_string(std::declval<
            typename TIn::value_type>(), cp)) out_type;
        std::vector<out_type> retval;
        make_narrow_strings(std::back_inserter(retval), begin, end, cp);
        return retval;
    }

    /// <summary>
    /// The default code page used for hard-coded strings.
    /// </summary>
    constexpr const narrow_string::code_page_type default_code_page
#if defined(_WIN32)
        = CP_OEMCP;
#else /* defined(_WIN32) */
        = nullptr;
#endif /* defined(_WIN32) */

} /* namespace dataverse */
} /* namespace visus */
