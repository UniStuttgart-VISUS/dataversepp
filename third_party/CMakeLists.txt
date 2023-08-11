# CMakeLists.txt
# Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.

include(FetchContent)

set(FETCHCONTENT_QUIET ON)
mark_as_advanced(FORCE
    FETCHCONTENT_BASE_DIR
    FETCHCONTENT_FULLY_DISCONNECTED
    FETCHCONTENT_QUIET
    FETCHCONTENT_UPDATES_DISCONNECTED)


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
