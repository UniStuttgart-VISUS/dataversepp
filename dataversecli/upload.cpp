// <copyright file="upload.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "upload.h"

#include <atomic>
#include <iostream>
#include <thread>

#include "dataverse/event.h"

#include "directory.h"


/// <summary>
/// Print the API error on the console.
/// </summary>
void print_error(_In_ const int cone,
        _In_z_ const char *message,
        _In_z_ const char *category,
        _In_ const visus::dataverse::narrow_string::code_page_type code_page) {
    using visus::dataverse::convert;
#if defined(_WIN32)
    auto mm = convert<char>(convert<wchar_t>(message, 0, code_page), CP_OEMCP);
#else /* defined(_WIN32) */
    auto mm = m;
#endif /* defined(_WIN32) */
    std::cerr << mm << std::endl;
}


/// <summary>
/// Print the the API response on the console.
/// </summary>
void print_response(_In_ const visus::dataverse::blob& response) {
    using visus::dataverse::convert;
    const auto r = response.as<char>();
#if defined(_WIN32)
    auto rr = convert<char>(convert<wchar_t>(r, response.size(), CP_UTF8),
        CP_OEMCP);
#else /* defined(_WIN32) */
    auto rr = std::string(r, r + response.size());
#endif /* defined(_WIN32) */
    std::cout << rr << std::endl;
}


/*
 * ::upload
 */
void upload(_In_ visus::dataverse::dataverse_connection& dataverse,
        _In_ const std::basic_string<TCHAR>& doi,
        _In_ const std::basic_string<TCHAR>& file,
        _In_ const std::basic_string<TCHAR>& description,
        _In_ const std::basic_string<TCHAR>& path,
        _In_ const std::vector< std::basic_string<TCHAR>>& tags,
        _In_ const bool restricted) {
    using namespace visus::dataverse;
    auto evt_done = create_event();
    _MAKE_NARROW_VECTOR(tags);

    dataverse.upload(_TT(doi), _TT(file), _TT(description), _TT(path),
            _TTV(tags), restricted, [](const blob& r, void *e) {
        ::print_response(r);
        std::cout << std::endl;
        set_event(*static_cast<event_type *>(e));
        }, [](const int c, const char *m, const char *a,
                const narrow_string::code_page_type p, void *e) {
        ::print_error(c, m, a, p);
        std::cerr << std::endl;
        set_event(*static_cast<event_type *>(e));
    }, &evt_done);

    wait_event(evt_done);
}


/*
 * ::upload
 */
void upload(_In_ visus::dataverse::dataverse_connection& dataverse,
        _In_ const std::basic_string<TCHAR>& doi,
        _In_ const std::basic_string<TCHAR>& directory,
        _In_ const bool recurse,
        _In_ const std::chrono::milliseconds wait) {
    using namespace visus::dataverse;
    auto files = ::get_files(_TTS(directory), recurse);
    std::atomic<std::size_t> remaining(files.size());

    for (auto& f : files) {
        dataverse.upload(_TT(doi), _TTS(f), [](const blob& r, void *e) {
            ::print_response(r);
            std::cout << std::endl;
            --(*static_cast<decltype(remaining) *>(e));
        }, [](const int c, const char *m, const char *a,
                const narrow_string::code_page_type p, void *e) {
            ::print_error(c, m, a, p);
            std::cerr << std::endl;
            --(*static_cast<decltype(remaining) *>(e));
        }, &remaining);
    }

    // Wait for all transfers to complete.
    while (remaining.load() > 0) {
        std::this_thread::sleep_for(wait);
    }
}
