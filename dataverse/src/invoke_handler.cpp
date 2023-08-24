// <copyright file="invoke_handler.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "invoke_handler.h"


#if defined(_WIN32)
#define EXCEPTION_CODE_PAGE (CP_OEMCP)
#else /* defined(_WIN32) */
#define EXCEPTION_CODE_PAGE (nullptr)
#endif /* defined(_WIN32) */


/*
 * visus::dataverse::detail::invoke_handler
 */
void visus::dataverse::detail::invoke_handler(
        _In_opt_ const dataverse_connection::on_error_type on_error,
        _In_ const std::system_error& ex,
        _In_opt_ void *client_data) {
    if (on_error != nullptr) {
        on_error(ex.code().value(),
            ex.what(),
            ex.code().category().name(),
            EXCEPTION_CODE_PAGE,
            client_data);
    }
}



/*
 * visus::dataverse::detail::invoke_handler
 */
void visus::dataverse::detail::invoke_handler(
        _In_opt_ const dataverse_connection::on_error_type on_error,
        _In_ const std::exception& ex,
        _In_opt_ void *client_data) {
    if (on_error != nullptr) {
        on_error(0,
            ex.what(),
            "Generic STL Exception",
            EXCEPTION_CODE_PAGE,
            client_data);
    }
}


/*
 * visus::dataverse::detail::invoke_handler
 */
void visus::dataverse::detail::invoke_handler(
        _In_opt_ const dataverse_connection::on_error_type on_error,
        _In_z_ const char *message,
        _In_z_ const char *category,
        _In_ narrow_string::code_page_type code_page,
        _In_opt_ void *client_data) {
    if (on_error != nullptr) {
        on_error(0, message, category, code_page, client_data);
    }
}


/*
 * visus::dataverse::detail::invoke_handler
 */
void visus::dataverse::detail::invoke_handler(
        _In_opt_ const dataverse_connection::on_error_type on_error,
        _In_opt_ void *client_data) {
    if (on_error != nullptr) {
        on_error(0, "An unexpected error was encountered while processing a "
            "request.",
            "Unexpected Exception",
            EXCEPTION_CODE_PAGE,
            client_data);
    }
}
