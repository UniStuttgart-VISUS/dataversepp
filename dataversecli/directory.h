// <copyright file="directory.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "convert.h"


/// <summary>
/// Gets all files in the given directory.
/// </summary>
/// <param name="path"></param>
/// <param name="recurse"></param>
/// <returns></returns>
std::vector<std::wstring> get_files(_In_ const std::wstring& path,
    _In_ const bool recurse);

/// <summary>
/// Gets all files in the given directory.
/// </summary>
/// <param name="path"></param>
/// <param name="recurse"></param>
/// <returns></returns>
std::vector<std::string> get_files(
    _In_ const visus::dataverse::const_narrow_string& path,
    _In_ const bool recurse);

/// <summary>
/// Answer whether the given path designates a directory.
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
bool is_directory(_In_ const std::wstring& path);

/// <summary>
/// Answer whether the given path designates a directory.
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
bool is_directory(_In_ const visus::dataverse::const_narrow_string& path);
