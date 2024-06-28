# CMakeLists.txt
# Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.

cmake_minimum_required(VERSION 3.15...3.29)

project(dataverse++ LANGUAGES CXX)

include(CMakeDependentOption)
include(GNUInstallDirs)

# User-configurable options.
option(DATAVERSE_BuildCli "Build CLI tool." ON)
option(DATAVERSE_DownloadThirdParty "Download dependencies." ON)
cmake_dependent_option(DATAVERSE_BuildTests "Build unit tests." ON WIN32 OFF)
cmake_dependent_option(DATAVERSE_Unicode "Use Unicode string." ON WIN32 OFF)


# Dependencies
if (DATAVERSE_DownloadThirdParty)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party")
else ()
    find_package(CURL REQUIRED)
    find_package(nlohmann_json REQUIRED)
    if (WIN32)
        find_package(wil REQUIRED)
    endif ()
endif ()

# The library
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dataverse")

# The command line tool
if (DATAVERSE_BuildCli)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dataversecli")
endif ()

# Unit tests
if (DATAVERSE_BuildTests)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif ()
