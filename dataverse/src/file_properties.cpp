// <copyright file="file_properties.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "file_properties.h"

#include <array>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#pragma push_macro("WIN32_NO_STATUS")
#define WIN32_NO_STATUS
#include <Windows.h>
#pragma pop_macro("WIN32_NO_STATUS")

#include <bcrypt.h>
#include <ntstatus.h>
#include <wincrypt.h>
#include <winerror.h>
#include <winternl.h>

#include <wil/resource.h>
#endif /* defined(_WIN32) */

#include "dataverse/convert.h"

#include "ntstatus_error_category.h"


namespace visus {
namespace dataverse {
namespace detail {

#if defined(_WIN32)
    static nlohmann::json get_file_properties(_In_ wil::unique_hfile& file) {
        wil::unique_bcrypt_algorithm algorithm;
        nlohmann::json retval;
        wil::unique_bcrypt_hash hash;
        std::vector<UCHAR> hash_value;

        // Open the MD5 provider.
        {
            auto status = ::BCryptOpenAlgorithmProvider(algorithm.addressof(),
                BCRYPT_MD5_ALGORITHM, nullptr, 0);
            if (!NT_SUCCESS(status)) {
                throw std::system_error(status, detail::ntstatus_category());
            }
        }

        // Determine the size of the hash and allocate space for it.
        {
            DWORD hash_length = 0;
            DWORD len = 0;
            auto status = ::BCryptGetProperty(algorithm.get(),
                BCRYPT_HASH_LENGTH, reinterpret_cast<PUCHAR>(&hash_length),
                sizeof(hash_length), &len, 0);
            if (!NT_SUCCESS(status)) {
                throw std::system_error(status, detail::ntstatus_category());
            }

            hash_value.resize(hash_length);
        }

        // Create the hash object.
        {
            auto status = ::BCryptCreateHash(algorithm.get(), hash.addressof(),
                nullptr, 0, nullptr, 0, 0);
            if (!NT_SUCCESS(status)) {
                throw std::system_error(status, detail::ntstatus_category());
            }
        }

        // Update the hash block by block.
        {
            std::vector<UCHAR> buffer(8 * retval.size());
            const auto cnt_buffer = static_cast<DWORD>(buffer.size());
            DWORD cnt_read = 0;

            do {
                if (!::ReadFile(file.get(), buffer.data(), cnt_buffer,
                        &cnt_read, nullptr)) {
                    auto error = ::GetLastError();
                    if (error != ERROR_HANDLE_EOF) {
                        throw std::system_error(error, std::system_category());
                    }
                }

                auto status = ::BCryptHashData(hash.get(), buffer.data(),
                    cnt_read, 0);

                if (!NT_SUCCESS(status)) {
                    throw std::system_error(status,
                        detail::ntstatus_category());
                }
            } while (cnt_read > 0);
        }

        // Finalise the hash.
        {
            auto size = static_cast<ULONG>(hash_value.size());
            auto status = ::BCryptFinishHash(hash.get(), hash_value.data(),
                size, 0);
            if (!NT_SUCCESS(status)) {
                throw std::system_error(status, detail::ntstatus_category());
            }
        }

        // Convert the hash to a hex string.
        {
            const DWORD cnt_input = static_cast<DWORD>(hash_value.size());
            DWORD cnt_output = 0;
            const auto flags = CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF;

            if (!::CryptBinaryToStringA(hash_value.data(), cnt_input,
                    flags, nullptr, &cnt_output)) {
                throw std::system_error(::GetLastError(),
                    std::system_category());
            }

            std::vector<char> output(cnt_output);

            if (!::CryptBinaryToStringA(hash_value.data(), cnt_input,
                    flags, output.data(), &cnt_output)) {
                throw std::system_error(::GetLastError(),
                    std::system_category());
            }

            retval["md5Hash"] = output.data();
        }

        // Determine the total size of the file.
        {
            LARGE_INTEGER size { 0 };
            if (!::GetFileSizeEx(file.get(), &size)) {
                throw std::system_error(::GetLastError(), std::system_category());
            }

            retval["fileSize"] = size.QuadPart;
        }

        // Determine the name of the file.
        {
            std::vector<wchar_t> path(MAX_PATH);

            if (!::GetFinalPathNameByHandleW(file.get(), path.data(),
                    static_cast<DWORD>(path.size()),
                    FILE_NAME_NORMALIZED | VOLUME_NAME_NONE)) {
                throw std::system_error(::GetLastError(),
                    std::system_category());
            }

            path.resize(::wcslen(path.data()) + 1);

            auto begin = std::find(path.rbegin(), path.rend(), '\\');
            if (begin != path.rend()) {
                path.erase(path.begin(), begin.base());
            }

            retval["fileName"] = to_utf8(path.data());
        }

        return retval;
    }
#endif /* defined(_WIN32) */

} /* namespace detail */
} /* namespace dataverse */
} /* namespace visus */


/*
 * nlohmann::json visus::dataverse::detail::get_file_properties
 */
nlohmann::json visus::dataverse::detail::get_file_properties(
        _In_z_ const wchar_t *path) {
#if defined(_WIN32)
    wil::unique_hfile file(::CreateFileW(path, GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, 0, NULL));
    if (!file) {
        throw std::system_error(::GetLastError(), std::system_category());
    }

    return get_file_properties(file);
#else /* defined(_WIN32) */
    auto s = convert<char>(path, 0, nullptr);
    return get_file_properties(make_narrow_string(s, nullptr));
#endif /* defined(_WIN32) */
    
}


/*
 * nlohmann::json visus::dataverse::detail::get_file_properties
 */
nlohmann::json visus::dataverse::detail::get_file_properties(
        _In_ const const_narrow_string& path) {
#if defined(_WIN32)
    wil::unique_hfile file(::CreateFileA(path, GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, 0, NULL));
    if (!file) {
        throw std::system_error(::GetLastError(), std::system_category());
    }

    return get_file_properties(file);
#else /* defined(_WIN32) */
    throw "TODO";
#endif /* defined(_WIN32) */
}
