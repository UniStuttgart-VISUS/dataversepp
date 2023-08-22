// <copyright file="form_data.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include <cinttypes>
#include <cstdlib>
#include <fstream>

#if defined(_WIN32)
#include <Windows.h>
#endif /* defined(_WIN32) */

#include "dataverse/api.h"
#include "dataverse/blob.h"
#include "dataverse/convert.h"


/* Forward declarations. */
struct curl_mime;


namespace visus {
namespace dataverse {

    /// <summary>
    /// Container for multi-part form data to be submitted in a POST request.
    /// </summary>
    /// <remarks>
    /// Instances of this class can be obtained from
    /// <see cref="dataverse_connection::make_form" />.
    /// </remarks>
    class DATAVERSE_API form_data final {

    public:

        /// <summary>
        /// The type used to represent a single byte.
        /// </summary>
        typedef blob::byte_type byte_type;

        /// <summary>
        /// Signature of the data read function used for incrementally
        /// transferring large files.
        /// </summary>
        typedef std::size_t (*on_read_type)(
            _Out_writes_bytes_(cnt *size) char *dst,
            _In_ const size_t size,
            _In_ const size_t cnt,
            _In_opt_ void *handle);

        /// <summary>
        /// Signature of the seeking function used for incrementally
        /// transferring large files.
        /// </summary>
        typedef int (*on_seek_type)(_In_opt_ void *handle,
            _In_ const std::streamoff offset,
            _In_ const int origin);

        /// <summary>
        /// Signature of the cleanup function used for incrementally
        /// transferring large files.
        /// </summary>
        typedef void (*on_close_type)(_In_opt_ void *);

#if defined(_WIN32)
        /// <summary>
        /// An implementation of <see cref="on_read_type" /> for use with Win32
        /// <see cref="HANDLE" />s.
        /// </summary>
        /// <remarks>
        /// If you use a <see cref="FILE" /> pointer, you can also use
        /// <see cref="fread" /> without any modifications-
        /// </remarks>
        /// <param name="dst"></param>
        /// <param name="size"></param>
        /// <param name="cnt"></param>
        /// <param name="handle"></param>
        /// <returns></returns>
        static std::size_t CALLBACK win32_read(
            _Out_writes_bytes_(cnt *size) char *dst,
            _In_ const size_t size,
            _In_ const size_t cnt,
            _In_opt_ void *handle);
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        /// <summary>
        /// An implementation of <see cref="on_seek_type" /> for use with Win32
        /// <see cref="HANDLE" />s.
        /// </summary>
        /// <remarks>
        /// If you use a <see cref="FILE" /> pointer, you can also use
        /// <see cref="fseek" /> without any modifications-
        /// </remarks>
        /// <param name="handle"></param>
        /// <param name="offset"></param>
        /// <param name="origin"></param>
        /// <returns></returns>
        static int CALLBACK win32_seek(_In_ void *handle,
            _In_ const std::streamoff offset,
            _In_ const int origin);
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        /// <summary>
        /// An implementation of <see cref="on_close_type" /> for use with Win32
        /// <see cref="HANDLE" />s.
        /// </summary>
        /// <remarks>
        /// If you use a <see cref="FILE" /> pointer, you can also use
        /// <see cref="fclose" /> without any modifications-
        /// </remarks>
        /// <param name="handle"></param>
        static void win32_close(_In_ void *handle);
#endif /* defined(_WIN32) */

        /// <summary>
        /// Initialises a new and unusable instance.
        /// </summary>
        form_data(void);

        /// <summary>
        /// Initialise from move.
        /// </summary>
        /// <param name="rhs">The object to be moved.</param>
        form_data(_Inout_ form_data&& rhs) noexcept;

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        ~form_data(void) noexcept;

        form_data& add_field(_In_z_ const wchar_t *name,
            _In_z_ const wchar_t *value);

        form_data& add_field(_In_ const const_narrow_string& name,
            _In_ const const_narrow_string& value);

        form_data& add_field(_In_z_ const wchar_t *name,
            _In_reads_bytes_(cnt) _In_ const byte_type *data,
            _In_ const std::size_t cnt);

        form_data& add_field(_In_ const const_narrow_string& name,
            _In_reads_bytes_(cnt) _In_ const byte_type *data,
            _In_ const std::size_t cnt);

        form_data& add_file(_In_z_ const wchar_t *name,
            _In_z_ const wchar_t *path);

        form_data& add_file(_In_ const const_narrow_string& name,
            _In_ const const_narrow_string& path);

        form_data& add_file(_In_z_ const wchar_t *name,
            _In_ const std::size_t size,
            _In_ on_read_type on_read,
            _In_opt_ on_seek_type on_seek,
            _In_opt_ on_close_type on_close,
            _In_opt_ void *context);

        form_data& add_file(_In_ const const_narrow_string& name,
            _In_ const std::size_t size,
            _In_ on_read_type on_read,
            _In_opt_ on_seek_type on_seek,
            _In_opt_ on_close_type on_close,
            _In_opt_ void *context);

        /// <summary>
        /// Move assignment.
        /// </summary>
        /// <param name="rhs">The right-hand side operand.</param>
        /// <returns><c>*this</c>.</returns>
        form_data& operator =(_Inout_ form_data&& rhs) noexcept;

        /// <summary>
        /// Indicates whether the form data are valid.
        /// </summary>
        /// <returns><c>true</c> if the form data are valid and can be sent via
        /// a <see cref="dataverse_connection" />, <c>false</c> otherwise.
        /// </returns>
        inline operator bool(void) const noexcept {
            return (this->_form != nullptr);
        }

    private:

        explicit form_data(_In_ void *curl);

        form_data& add_field(_In_z_ const char *name,
            _In_z_ const char *value);

        void check_not_disposed(void);

        mutable void *_curl;
        curl_mime *_form;

        friend class dataverse_connection;
    };

} /* namespace dataverse */
} /* namespace visus */
