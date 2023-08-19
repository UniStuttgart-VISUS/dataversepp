// <copyright file="dataversecli.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <algorithm>
#include <cinttypes>
#include <iostream>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#include <tchar.h>

#if (defined(UNICODE) || defined(_UNICODE))
typedef std::wstring string_type;
#define _TT(s) string_type(_T(s))

#else /* (defined(UNICODE) || defined(_UNICODE)) */
typedef visus::dataverse::const_narrow_string string_type;
#define _TT(s) visus::dataverse::const_narrow_string(s, CP_OEMCP)
#endif /* (defined(UNICODE) || defined(_UNICODE)) */

#else /* defined(_WIN32) */
#define TCHAR char
#define _T(s) (s)

typedef visus::dataverse::const_narrow_string string_type;
#define _TT(s) visus::dataverse::make_narrow_string(s, nullptr)
#endif /* defined(_WIN32) */

#include <nlohmann/json.hpp>

#include "dataverse/dataverse_connection.h"


/// <summary>
/// Find the specified switch in the given range.
/// </summary>
template<class TIterator>
TIterator find_switch(_In_ const TIterator begin,
        _In_ const TIterator end,
        _In_z_ const TCHAR *argument) {
    typedef typename TIterator::value_type value_type;
    value_type a(argument);
    std::transform(a.begin(), a.end(), a.begin(), [](const TCHAR c) {
        return std::tolower(c);
    });

    return std::find_if(begin, end, [&a](const value_type& it) {
        for (auto l = a.cbegin(), r = it.begin(); l != a.cend(); ++l, ++r) {
            if (*l != std::tolower(*r)) {
                return false;
            }
        }
        return true;
    });
}


/// <summary>
/// Find the value of the specified argument in the given range.
/// </summary>
template<class TIterator>
TIterator find_argument(_In_ const TIterator begin,
        _In_ const TIterator end,
        _In_z_ const TCHAR *argument) {
    auto retval = ::find_switch(begin, end, argument);
    return (retval != end) ? ++retval : retval;
}


/// <summary>
/// Entry point of the command line demo application.
/// </summary>
/// <param name="argc">The size of the argument list.</param>
/// <param name="argv">The command line arguments.</param>
/// <returns>Zero in case of success, -1 in case of an error.</returns>
int main(_In_ const int argc, const TCHAR **argv) {
    using namespace visus::dataverse;

#if (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)))
    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //::_CrtSetBreakAlloc(190);
#endif /* (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG))) */

    try {
        const std::vector<std::basic_string<TCHAR>> cmd_line(argv, argv + argc);
        dataverse_connection dataverse;
        string_type description;
        string_type doi;
        string_type file;
        string_type path;
        std::vector<string_type> tags;

        {
            // The API end point to connect to (a URL).
            auto it = ::find_argument(cmd_line.begin(), cmd_line.end(),
                _T("/endpoint"));
            if (it != cmd_line.end()) {
                dataverse.base_path(it->c_str());
            } else {
                dataverse.base_path(L"https://darus.uni-stuttgart.de/api/");
            }
        }

        {
            // The API key to authenticate with.
            auto it = ::find_argument(cmd_line.begin(), cmd_line.end(),
                _T("/apikey"));
            if (it != cmd_line.end()) {
                dataverse.api_key(it->c_str());
            }
        }

        {
            // The DOI of the data set to modify.
            auto it = ::find_argument(cmd_line.begin(), cmd_line.end(),
                _T("/doi"));
            if (it != cmd_line.end()) {
                doi = *it;
            }
        }

        {
            // The local path to the file to be uploaded.
            auto it = ::find_argument(cmd_line.begin(), cmd_line.end(),
                _T("/file"));
            if (it != cmd_line.end()) {
                file = *it;
            }
        }

        {
            // The logical path to build the tree structure of files in the data
            // set.
            auto it = ::find_argument(cmd_line.begin(), cmd_line.end(),
                _T("/path"));
            if (it != cmd_line.end()) {
                path = *it;
            }
        }

        {
            // A description of the file.
            auto it = ::find_argument(cmd_line.begin(), cmd_line.end(),
                _T("/description"));
            if (it != cmd_line.end()) {
                description = *it;
            }
        }

        // Indicates whether access to the file is restricted.
        const auto restricted = (::find_switch(cmd_line.begin(), cmd_line.end(),
            _T("/restricted")) != cmd_line.end());

        {
            // User-defined tags to be assigned to the file.
            auto it = cmd_line.begin();
            while (it != cmd_line.end()) {
                it = ::find_argument(cmd_line.begin(), cmd_line.end(),
                    _T("/tag"));
                if (it != cmd_line.end()) {
                    tags.push_back(*it++);
                }
            }
        }

        // Do the upload.
        auto evt_done = create_event();

        dataverse.upload(doi, file, description, path, tags, restricted,
                    [](const blob& r,   // Response from server.
                        void *e) {      // Event handle
                const auto response = std::string(r.as<char>(), r.size());
#if defined(_WIN32)
                auto rr = convert<char>(convert<wchar_t>(response, CP_UTF8),
                    CP_OEMCP);
#else /* defined(_WIN32) */
                auto rr = response;
#endif /* defined(_WIN32) */
                std::cout << rr << std::endl;
                std::cout << std::endl;
                set_event(*static_cast<event_type *>(e));
            }, [](const int,                                // Error code
                    const char *m,                          // Message
                    const char *,                           // Message category
                    const narrow_string::code_page_type p,  // Code page
                    void *e) {                              // Event handle
#if defined(_WIN32)
                auto mm = convert<char>(convert<wchar_t>(m, 0, p), CP_OEMCP);
#else /* defined(_WIN32) */
                auto mm = m;
#endif /* defined(_WIN32) */
                std::cout << mm << std::endl;
                std::cout << std::endl;
                set_event(*static_cast<event_type *>(e));
            }, &evt_done);

        wait_event(evt_done);

#if false
        auto data_set = nlohmann::json({ });
        data_set["datasetVersion"]["license"]["name"] = "CC BY 4.0";
        data_set["datasetVersion"]["license"]["uri"] = "https://creativecommons.org/licenses/by/4.0/legalcode.de";

        auto citation_metadata = nlohmann::json::array();
        citation_metadata.push_back(json::make_meta_field(
            L"title",
            L"primitive",
            false,
            to_utf8(L"Energy consumption of scientific visualisation and data visualisation algorithms")));
        citation_metadata.push_back(
            json::make_meta_field(L"author", L"compound", true,
                json::make_author(L"Müller, Christoph"),
                json::make_author(L"Heinemann, Moritz"),
                json::make_author(L"Weiskopf, Daniel"),
                json::make_author(L"Ertl, Thomas"))
        );
        citation_metadata.push_back(
            json::make_meta_field(L"datasetContact", L"compound", true,
                json::make_contact(L"Querulant", L"querulant@visus.uni-stuttgart.de"))
        );
        citation_metadata.push_back(
            json::make_meta_field(L"dsDescription", L"compound", true,
                json::make_data_desc(L"This data set comprises a series of measurements of GPU power consumption.")
            )
        );
        citation_metadata.push_back(
            json::make_meta_field(L"subject", L"controlledVocabulary", true, to_utf8(L"Computer and Information Science"))
        );

        data_set["datasetVersion"]["metadataBlocks"]["citation"]["displayName"] = to_utf8(L"Citation Metadata");
        data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"] = citation_metadata;

        std::cout << convert<char>(convert<wchar_t>(data_set.dump(), CP_UTF8), CP_OEMCP) << std::endl;
#endif

        return 0;
    } catch (std::exception& ex) {
        std::cout << ex.what() << std::endl << std::endl;
        return -1;
    }
}
