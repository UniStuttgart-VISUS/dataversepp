# CMakeLists.txt
# Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.

project(dataversecli)


# Collect source files.
file(GLOB_RECURSE HeaderFiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h" "*.inl")
file(GLOB_RECURSE SourceFiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp")
file(GLOB_RECURSE ResourceFiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.rc")


# Define the output.
add_executable(${PROJECT_NAME} ${HeaderFiles} ${SourceFiles})


# Add resource scripts.
target_sources(${PROJECT_NAME} PRIVATE ${ResourceFiles})


# Configure the compiler.
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)


# Configure the linker.
target_link_libraries(${PROJECT_NAME} dataverse nlohmann_json)


# Installation
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
