// <copyright file="dataversecli.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <cinttypes>
#include <iostream>

#include "convert.h"
#include "directory.h"
#include "upload.h"


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
int _tmain(_In_ const int argc, const TCHAR **argv) {
    typedef std::basic_string<TCHAR> string_type;

#if (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG)))
    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //::_CrtSetBreakAlloc(190);
#endif /* (defined(_WIN32) && (defined(DEBUG) || defined(_DEBUG))) */

    try {
        const std::vector<std::basic_string<TCHAR>> cmd_line(argv, argv + argc);
        visus::dataverse::dataverse_connection dataverse;
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
                dataverse.base_path(_TTS(it->c_str()));
            } else {
                dataverse.base_path(L"https://darus.uni-stuttgart.de/api/");
            }
        }

        {
            // The API key to authenticate with.
            auto it = ::find_argument(cmd_line.begin(), cmd_line.end(),
                _T("/apikey"));
            if (it != cmd_line.end()) {
                dataverse.api_key(_TTS(it->c_str()));
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
            // The local path to the file or directory to be uploaded.
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

        // Indicates whether, in case the input is a directory, the directory
        // should be recursively uploaded.
        const auto recurse = (::find_switch(cmd_line.begin(), cmd_line.end(),
            _T("/recurse")) != cmd_line.end());

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

        // Make the DOI a persistent ID.
        doi = _T("doi:") + doi;

        // Do the upload.
        if (::is_directory(_TTC(file))) {
            ::upload(dataverse, doi, file, recurse,
                std::chrono::milliseconds(100));
        } else {
            ::upload(dataverse, doi, file, description, path, tags, restricted);
        }

        return 0;
    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        std::cerr << std::endl;
        return -1;
    }
}
