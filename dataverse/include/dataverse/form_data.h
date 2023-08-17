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
    class DATAVERSE_API form_data final {

    public:

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
