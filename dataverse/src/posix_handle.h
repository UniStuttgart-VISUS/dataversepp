// <copyright file="posix_handle.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <fcntl.h>

#if defined(_WIN32)
#include <io.h>
#endif /* _WIN32 */

#include "dataverse/api.h"


namespace visus {
namespace dataverse {
namespace detail {

    /// <summary>
    /// RAII wrapper around a POSIX file handle.
    /// </summary>
    class posix_handle final {

    public:

        inline posix_handle(_In_ const int handle = -1) : _handle(handle) { }

        posix_handle(_Inout_ posix_handle&& rhs) noexcept;

        ~posix_handle(void) noexcept;

        inline int get(void) noexcept {
            return this->_handle;
        }

        inline int release(void) noexcept {
            auto retval = this->_handle;
            this->_handle = -1;
            return retval;
        }

        inline operator int(void) const noexcept {
            return this->_handle;
        }

        inline operator bool(void) const {
            return (this->_handle != -1);
        }

        posix_handle& operator =(_Inout_ posix_handle&& rhs) noexcept;

        posix_handle& operator =(_In_ const int handle) noexcept;

    private:

        int _handle;
    };

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */

