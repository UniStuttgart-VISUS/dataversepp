# CMakeLists.txt
# Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.

cmake_minimum_required(VERSION 3.15)

include(CMakeDependentOption)


project(dataverse++)

# User-configurable options.
cmake_dependent_option(DATAVERSE_BuildTests "Build unit tests." ON WIN32 OFF)
cmake_dependent_option(DATAVERSE_Unicode "Use Unicode string." ON WIN32 OFF)


# Dependencies
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party")

# The library
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dataverse")

# The command line tool
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dataversecli")

# Unit tests
if (DATAVERSE_BuildTests)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif ()
