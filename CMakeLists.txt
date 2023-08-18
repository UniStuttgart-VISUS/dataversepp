# CMakeLists.txt
# Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.

cmake_minimum_required(VERSION 3.15)

include(CMakeDependentOption)


project(dataverse++)

# User-configurable options.
#option(PWROWG_BuildDemo "Build demo programme" OFF)
#cmake_dependent_option(PWROWG_BuildDriver "Build RAPL MSR driver" OFF WIN32 OFF)
#option(PWROWG_BuildDumpSensors "Build dump_sensors utility" ON)
#option(PWROWG_BuildRtxConfig "Build R&S RTA/RTB configuration utility" OFF)
#cmake_dependent_option(PWROWG_BuildStablePower "Build setstablepowerstate tool" OFF WIN32 OFF)
cmake_dependent_option(DATAVERSE_BuildTests "Build unit tests." ON WIN32 OFF)
#cmake_dependent_option(PWROWG_BuildWeb "Build browser for benchmarking web-based visualisations" OFF WIN32 OFF)
#option(PWROWG_NoPackageRestore "Disable automatic restore of Nuget packages as pre-build step" OFF)
#mark_as_advanced(PWROWG_NoPackageRestore)


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
