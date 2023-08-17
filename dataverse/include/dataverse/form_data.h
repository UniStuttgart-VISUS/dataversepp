// <copyright file="form_data.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

#include "dataverse/api.h"
#include "dataverse/types.h"


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
        static std::size_t CALLBACK win32_read(
            _Out_writes_bytes_(cnt *size) char *dst,
            _In_ const size_t size,
            _In_ const size_t cnt,
            _In_opt_ void *handle);
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        static int CALLBACK win32_seek(_In_ void *handle,
            _In_ const std::streamoff offset,
            _In_ const int origin);
#endif /* defined(_WIN32) */

#if defined(_WIN32)
        static void win32_close(_In_ void *handle);
#endif /* defined(_WIN32) */

        form_data(void);

        form_data(_Inout_ form_data&& rhs) noexcept;

        ~form_data(void) noexcept;

        form_data& add_field(_In_z_ const char_type *name,
            _In_z_ const char_type *value);

        form_data& add_field(_In_z_ const char_type *name,
            _In_reads_bytes_(cnt) _In_ const std::uint8_t *data,
            _In_ const std::size_t cnt);

        form_data& add_file(_In_z_ const char_type *name,
            _In_z_ const char_type *path);

        form_data& add_file(_In_z_ const char_type *name,
            _In_ const std::size_t size,
            _In_ on_read_type on_read,
            _In_opt_ on_seek_type on_seek,
            _In_opt_ on_close_type on_close,
            _In_opt_ void *context);

        form_data& operator =(_Inout_ form_data&& rhs) noexcept;

        inline operator bool(void) const noexcept {
            return (this->_form != nullptr);
        }

    private:

        explicit form_data(_In_ void *curl);

        void check_not_disposed(void);

        curl_mime *_form;

        friend class dataverse_connection;
    };

} /* namespace dataverse */
} /* namespace visus */
