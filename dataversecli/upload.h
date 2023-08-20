// <copyright file="upload.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <chrono>

#include <nlohmann/json.hpp>

#include "dataverse/dataverse_connection.h"

#include "convert.h"


/// <summary>
/// Synchronously uploads the specified file.
/// </summary>
/// <param name="dataverse"></param>
/// <param name="doi"></param>
/// <param name="file"></param>
/// <param name="description"></param>
/// <param name="path"></param>
/// <param name="tags"></param>
/// <param name="restricted"></param>
void upload(_In_ visus::dataverse::dataverse_connection& dataverse,
    _In_ const std::basic_string<TCHAR>& doi,
    _In_ const std::basic_string<TCHAR>& file,
    _In_ const std::basic_string<TCHAR>& description,
    _In_ const std::basic_string<TCHAR>& path,
    _In_ const std::vector< std::basic_string<TCHAR>>& tags,
    _In_ const bool restricted);

/// <summary>
/// Synchronously uploads the files in the specified directory.
/// </summary>
/// <param name="dataverse"></param>
/// <param name="doi"></param>
/// <param name="directory"></param>
/// <param name="recurse"></param>
/// <param name="wait"></param>
void upload(_In_ visus::dataverse::dataverse_connection& dataverse,
    _In_ const std::basic_string<TCHAR>& doi,
    _In_ const std::basic_string<TCHAR>& directory,
    _In_ const bool recurse,
    _In_ const std::chrono::milliseconds wait);
