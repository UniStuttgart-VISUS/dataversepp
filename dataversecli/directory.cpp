// <copyright file="directory.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "directory.h"

#include <algorithm>
#include <cerrno>
#include <stack>
#include <system_error>

#if defined(_WIN32)
#include <wil/resource.h>
#else /* defined(_WIN32) */
#include <unistd.h>
#include <sys/stat.h>
#endif /* defined(_WIN32) */

#include "dataverse/convert.h"


/*
 * ::get_files
 */
std::vector<std::wstring> get_files(_In_ const std::wstring& path,
        _In_ const bool recurse) {
    std::vector<std::wstring> retval;

#if defined(_WIN32)
    WIN32_FIND_DATAW data;
    std::stack<std::wstring> stack;

    // Initialise the stack and make sure that the path is terminated with a
    // path separator to formulate the correct query.
    stack.push(path);

    if (stack.top().back() != L'\\') {
        stack.top() += L'\\';
    }

    while (!stack.empty()) {
        const auto p = stack.top();
        stack.pop();
        const auto q = p + L"*";

        wil::unique_hfind handle(::FindFirstFileW(q.c_str(), &data));
        if (!handle) {
            throw std::system_error(::GetLastError(), std::system_category());
        }

        do {
            if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                if (recurse
                        && (::wcscmp(data.cFileName, L".") != 0)
                        && (::wcscmp(data.cFileName, L"..") != 0)) {
                    stack.push((p + data.cFileName) + L"\\");
                }
            } else {
                retval.push_back(p + data.cFileName);
            }
        } while (::FindNextFileW(handle.get(), &data));

        {
            auto status = ::GetLastError();
            if (status != ERROR_NO_MORE_FILES) {
                throw std::system_error(status, std::system_category());
            }
        }
    }
#else /* defined(_WIN32) */
    auto p = visus::dataverse::convert<char>(path, nullptr);
    auto r =  ::get_files(visus::dataverse::make_narrow_string(p, nullptr),
        recurse);
    retval.resize(r.size());
    std::transform(r.begin(), r.end(), retval.begin(),
            [](const std::string& s) {
        return visus::dataverse::convert<wchar_t>(s, nullptr);
    });
#endif /* defined(_WIN32) */

    return retval;
}


/*
 * ::get_files
 */
std::vector<std::string> get_files(
        _In_ const visus::dataverse::const_narrow_string& path,
        _In_ const bool recurse) {
    std::vector<std::string> retval;

#if defined(_WIN32)
    WIN32_FIND_DATAA data;
    std::stack<std::string> stack;

    // Initialise the stack and make sure that the path is terminated with a
    // path separator to formulate the correct query.
    stack.push(path.value());

    if (stack.top().back() != '\\') {
        stack.top() += '\\';
    }

    while (!stack.empty()) {
        const auto p = stack.top();
        stack.pop();
        const auto q = p + "*";

        wil::unique_hfind handle(::FindFirstFileA(q.c_str(), &data));
        if (!handle) {
            throw std::system_error(::GetLastError(), std::system_category());
        }

        do {
            if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                if (recurse
                        && (::strcmp(data.cFileName, ".") != 0)
                        && (::strcmp(data.cFileName, "..") != 0)) {
                    stack.push((p + data.cFileName) + "\\");
                }
            } else {
                retval.push_back(p + data.cFileName);
            }
        } while (::FindNextFileA(handle.get(), &data));

        {
            auto status = ::GetLastError();
            if (status != ERROR_NO_MORE_FILES) {
                throw std::system_error(status, std::system_category());
            }
        }
    }
#else /* defined(_WIN32) */
    throw "TODO";
#endif /* defined(_WIN32) */
    return retval;
}

/*
 * is_directory
 */
bool is_directory(_In_ const std::wstring& path) {
#if defined(_WIN32)
    const auto attribs = GetFileAttributesW(path.c_str());
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        throw std::system_error(::GetLastError(), std::system_category());
    }
    return ((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0);
#else /* defined(_WIN32) */
    auto p = visus::dataverse::convert<char>(path, nullptr);
    return is_directory(visus::dataverse::make_narrow_string(p, nullptr));
#endif /* defined(_WIN32) */
}


/*
 * is_directory
 */
bool is_directory(_In_ const visus::dataverse::const_narrow_string& path) {
#if defined(_WIN32)
    const auto attribs = GetFileAttributesA(path);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        throw std::system_error(::GetLastError(), std::system_category());
    }
    return ((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0);
#else /* defined(_WIN32) */
    struct stat stat;
    if (::stat(path, &stat) != 0) {
        throw std::system_error(errno, std::system_category());
    }
    return ((stat.st_mode & S_IFDIR) != 0);
#endif /* defined(_WIN32) */
}
