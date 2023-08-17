# CMakeLists.txt
# Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.

include(FetchContent)

set(FETCHCONTENT_QUIET ON)
mark_as_advanced(FORCE
    FETCHCONTENT_BASE_DIR
    FETCHCONTENT_FULLY_DISCONNECTED
    FETCHCONTENT_QUIET
    FETCHCONTENT_UPDATES_DISCONNECTED)

#find_package(openssl CONFIG REQUIRED)

# Windows Implementation Library
FetchContent_Declare(wil
    URL "https://github.com/microsoft/wil/archive/v1.0.230202.1.tar.gz"
)
option(FAST_BUILD "" OFF)
option(WIL_BUILD_PACKAGING "" OFF)
option(WIL_BUILD_TESTS "" OFF)
FetchContent_GetProperties(wil)
if (NOT wil_POPULATED)
    FetchContent_Populate(wil)
    add_subdirectory(${wil_SOURCE_DIR} ${wil_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()
mark_as_advanced(FORCE
    FETCHCONTENT_SOURCE_DIR_WIL
    FETCHCONTENT_UPDATES_DISCONNECTED_WIL
    FAST_BUILD
    WIL_BUILD_PACKAGING
    WIL_BUILD_TESTS)


# Open SSL (On Windows, we use SChannel)
if (NOT WIN32)
    find_package(OpenSSL REQUIRED)
endif ()


# cURL
FetchContent_Declare(curl
    URL "https://github.com/curl/curl/archive/refs/tags/curl-8_2_1.zip"
)
option(BUILD_CURL_EXE "" OFF)
option(CURL_ENABLE_SSL "" ON)
option(CURL_ZLIB "" OFF)
IF (WIN32)
    option(CURL_USE_SCHANNEL "" ON)
    option(CURL_WINDOWS_SSPI "" ON)
    option(ENABLE_UNICODE "" ON)
ELSE ()
    option(CURL_USE_OPENSSL "" ON)
ENDIF ()
FetchContent_MakeAvailable(curl)
mark_as_advanced(FORCE
    FETCHCONTENT_SOURCE_DIR_CURL
    FETCHCONTENT_UPDATES_DISCONNECTED_CURL
    BUILD_CURL_EXE
    BUILD_SHARED_LIBS
    CMAKE_LBER_LIB
    CMAKE_LDAP_LIB
    CURL_BROTLI
    CURL_CA_FALLBACK
    CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
    CURL_ENABLE_SSL
    CURL_LTO
    CURL_STATIC_CRT
    CURL_TARGET_WINDOWS_VERSION
    CURL_USE_BEARSSL
    CURL_USE_MBEDTLS
    CURL_USE_NSS
    CURL_USE_OPENSSL
    CURL_USE_SCHANNEL
    CURL_USE_WOLFSSL
    CURL_WERROR
    CURL_WINDOWS_SSPI
    CURL_ZLIB
    CURL_ZSTD
    ENABLE_ARES
    ENABLE_CURLDEBUG
    ENABLE_DEBUG
    ENABLE_THREADED_RESOLVER
    ENABLE_UNICODE
    ENABLE_UNIX_SOCKETS
    ENABLE_WEBSOCKETS
    LIBCURL_OUTPUT_NAME
    PICKY_COMPILER
    USE_LIBIDN2
    USE_MSH3
    USE_NGHTTP2
    USE_NGTCP2
    USE_QUICHE
    USE_WIN32_IDN
    USE_WIN32_LDAP)


# JSON library
FetchContent_Declare(nlohmann_json
    URL "https://github.com/nlohmann/json/archive/v3.11.2.tar.gz"
)
FetchContent_MakeAvailable(nlohmann_json)
mark_as_advanced(FORCE
    FETCHCONTENT_SOURCE_DIR_NLOHMANN_JSON
    FETCHCONTENT_UPDATES_DISCONNECTED_NLOHMANN_JSON
    JSON_BuildTests
    JSON_CI
    JSON_Diagnostics
    JSON_DisableEnumSerialization
    JSON_GlobalUDLs
    JSON_ImplicitConversions
    JSON_Install
    JSON_LegacyDiscardedValueComparison
    JSON_MultipleHeaders
    JSON_SystemInclude)
