// <copyright file="errors.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <cerrno>
#include <stdexcept>
#include <system_error>

#if defined(_WIN32)
#include <Windows.h>
#endif /* defined(_WIN32) */


#if !defined(_WIN32)
#define ERROR_INVALID_HANDLE (EFAULT)
#define ERROR_INVALID_STATE (ENOTRECOVERABLE)
#define ERROR_NO_UNICODE_TRANSLATION (EINVAL)
#define ERROR_OUTOFMEMORY (ENOMEM)
#define ERROR_SUCCESS (0)
#endif /* !defined(_WIN32) */
