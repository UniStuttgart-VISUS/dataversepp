// <copyright file="hash.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/hash.h"

#include <stdexcept>
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


/*
 * visus::dataverse::hash
 */
std::size_t visus::dataverse::hash(
        _Out_writes_to_opt_(cnt, return) std::uint8_t *dst,
        _In_ std::size_t cnt,
        _In_z_ const wchar_t *path,
        _In_z_ const wchar_t *algorithm) {
#if defined(_WIN32)
    wil::unique_bcrypt_algorithm alg;
    wil::unique_bcrypt_hash hash;
    DWORD retval = 0;

    {
        auto status = ::BCryptOpenAlgorithmProvider(alg.addressof(), algorithm,
            nullptr, 0);
        if (!NT_SUCCESS(status)) {
            throw std::system_error(status, detail::ntstatus_category());
        }
    }

    {
        DWORD len = 0;
        auto status = ::BCryptGetProperty(alg.get(), BCRYPT_HASH_LENGTH,
            reinterpret_cast<PUCHAR>(&retval), sizeof(retval), &len, 0);
        if (!NT_SUCCESS(status)) {
            throw std::system_error(status, detail::ntstatus_category());
        }
    }

    if ((dst != nullptr) && (cnt >= retval)) {
        {
            auto status = ::BCryptCreateHash(alg.get(), hash.addressof(),
                nullptr, 0, nullptr, 0, 0);
            if (!NT_SUCCESS(status)) {
                throw std::system_error(status, detail::ntstatus_category());
            }
        }

        wil::unique_hfile file(::CreateFileW(path, GENERIC_READ,
            FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL));
        if (!file) {
            throw std::system_error(::GetLastError(), std::system_category());
        }

        std::vector<UCHAR> buffer(8 * retval);
        const auto cnt_buffer = static_cast<DWORD>(buffer.size());
        DWORD cnt_read = 0;

        do {
            if (!::ReadFile(file.get(), buffer.data(), cnt_buffer, &cnt_read,
                    nullptr)) {
                auto error = ::GetLastError();
                if (error != ERROR_HANDLE_EOF) {
                    throw std::system_error(error, std::system_category());
                }
            }

            auto status = ::BCryptHashData(hash.get(), buffer.data(), cnt_read,
                0);

            if (!NT_SUCCESS(status)) {
                throw std::system_error(status, detail::ntstatus_category());
            }
        } while (cnt_read > 0);

        {
            auto status = ::BCryptFinishHash(hash.get(), dst, cnt, 0);
            if (!NT_SUCCESS(status)) {
                throw std::system_error(status, detail::ntstatus_category());
            }
        }
    }

    return retval;

#else /* defined(_WIN32) */
    auto p = convert<char>(path, 0, nullptr);
    auto a = convert<char>(algorithm, 0, nullptr);
    return hash(dst,
        cnt,
        make_narrow_string(p, nullptr),
        make_narrow_string(a, nullptr));
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::hash
 */
std::size_t visus::dataverse::hash(
        _Out_writes_to_opt_(cnt, return) std::uint8_t *dst,
        _In_ std::size_t cnt,
        _In_ const const_narrow_string& path,
        _In_ const const_narrow_string& algorithm) {
#if defined(_WIN32)
    auto p = convert<wchar_t>(path);
    auto a = convert<wchar_t>(algorithm);
    return hash(dst, cnt, p.c_str(), a.c_str());
#else /* defined(_WIN32) */
    throw "TODO";
#endif /* defined(_WIN32) */
}


/*
 * visus::dataverse::to_base64
 */
std::size_t visus::dataverse::to_base64(
        _Out_writes_to_opt_(cnt_dst, return) wchar_t *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_(cnt_src) const std::uint8_t *src,
        _In_ const std::size_t cnt_src) {
#if defined(_WIN32)
    auto retval = (dst != nullptr) ? static_cast<DWORD>(cnt_dst) : 0;
    if (!::CryptBinaryToStringW(src, static_cast<DWORD>(cnt_src),
            CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
            dst, &retval)) {
        throw std::system_error(::GetLastError(), std::system_category());
    }
    return retval;
#else /* defined(_WIN32) */
    throw "TODO"
#endif /* defined(_WIN32) */
}



/*
 * visus::dataverse::to_base64
 */
std::size_t visus::dataverse::to_base64(
        _Out_writes_to_opt_(cnt_dst, return) char *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_(cnt_src) const std::uint8_t *src,
        _In_ const std::size_t cnt_src) {
#if defined(_WIN32)
    auto retval = (dst != nullptr) ? static_cast<DWORD>(cnt_dst) : 0;
    if (!::CryptBinaryToStringA(src, static_cast<DWORD>(cnt_src),
            CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
            dst, &retval)) {
        throw std::system_error(::GetLastError(), std::system_category());
    }
    return retval;
#else /* defined(_WIN32) */
    throw "TODO"
#endif /* defined(_WIN32) */
}
