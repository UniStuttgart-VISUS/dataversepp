// <copyright file="dataversecli.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <cinttypes>
#include <iostream>
#include <vector>

#include <nlohmann/json.hpp>

#include "dataverse/dataverse_connection.h"



int main() {
    using namespace visus::dataverse;
#if (defined(DEBUG) || defined(_DEBUG))
    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //::_CrtSetBreakAlloc(190);
#endif /* (defined(DEBUG) || defined(_DEBUG)) */

    try {
        dataverse_connection c;

        //auto h = ::CreateFile(DVSL("T:\\Programmcode\\dataversepp\\dataverse\\src\\convert.cpp"), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL);
        //ULARGE_INTEGER size;
        //size.LowPart = ::GetFileSize(h, &size.HighPart);

        auto data_set = nlohmann::json({ });
        data_set["datasetVersion"]["license"]["name"] = "CC BY 4.0";
        data_set["datasetVersion"]["license"]["uri"] = "https://creativecommons.org/licenses/by/4.0/legalcode.de";

        auto citation_metadata = nlohmann::json::array();
        citation_metadata.push_back(json::make_meta_field(
            L"title",
            L"primitive",
            false,
            to_utf8(L"Energy consumption of scientific visualisation and data visualisation algorithms")));
        citation_metadata.push_back(
            json::make_meta_field(L"author", L"compound", true,
                json::make_author(L"Müller, Christoph"),
                json::make_author(L"Heinemann, Moritz"),
                json::make_author(L"Weiskopf, Daniel"),
                json::make_author(L"Ertl, Thomas"))
        );
        citation_metadata.push_back(
            json::make_meta_field(L"datasetContact", L"compound", true,
                json::make_contact(L"Querulant", L"querulant@visus.uni-stuttgart.de"))
        );
        citation_metadata.push_back(
            json::make_meta_field(L"dsDescription", L"compound", true,
                json::make_data_desc(L"This data set comprises a series of measurements of GPU power consumption.")
            )
        );
        citation_metadata.push_back(
            json::make_meta_field(L"subject", L"controlledVocabulary", true, to_utf8(L"Computer and Information Science"))
        );

        data_set["datasetVersion"]["metadataBlocks"]["citation"]["displayName"] = to_utf8(L"Citation Metadata");
        data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"] = citation_metadata;

        std::cout << convert<char>(convert<wchar_t>(data_set.dump(), CP_UTF8), CP_OEMCP) << std::endl;

        c.base_path(L"https://demodarus.izus.uni-stuttgart.de/api/")
            .api_key(L"")
            //.get(L"/dataverses/visus", [](const blob &r, void *) {
            //    std::cout << convert<char>(convert<wchar_t>(std::string(r.as<char>(), r.size()), CP_UTF8), CP_OEMCP) << std::endl;
            //    std::cout << std::endl;
            //}, [](const int, const char *, const char *, const narrow_string::code_page_type, void *) { })
            //.post(DVSL("/datasets/:persistentId/add?persistentId=doi:10.15770/darus-1644"), c.make_form().add_file(DVSL("file"), DVSL("T:\\Programmcode\\dataversepp\\dataverse\\src\\convert.cpp")), [](const blob &r, void *) {
            //    std::cout << convert<char>(convert<wchar_t>(std::string(r.as<char>(), r.size()), CP_UTF8), CP_OEMCP) << std::endl;
            //    std::cout << std::endl;
            //}, [](const int, const char_type *, const char_type *, void *) {});
            //.post(DVSL("/datasets/:persistentId/add?persistentId=doi:10.15770/darus-1644"), c.make_form().add_field(L"file", L"convert.cpp").add_file(L"file", size.QuadPart, form_data::win32_read, form_data::win32_seek, form_data::win32_close, (void *)h), [](const blob &r, void *) {
            //    std::cout << convert<char>(convert<wchar_t>(std::string(r.as<char>(), r.size()), CP_UTF8), CP_OEMCP) << std::endl;
            //    std::cout << std::endl;
            //}, [](const int, const char_type *, const char_type *, void *) {});
            //.upload(DVSL("doi:10.15770/darus-1644"), DVSL("T:\\Programmcode\\dataversepp\\dataverse\\src\\convert.cpp"), [](const blob &r, void *) {
            //    std::cout << convert<char>(convert<wchar_t>(std::string(r.as<char>(), r.size()), CP_UTF8), CP_OEMCP) << std::endl;
            //    std::cout << std::endl;
            //}, [](const int, const char_type *, const char_type *, void *) {});
            //.upload(std::wstring(L"doi:10.15770/darus-1644"),
            //    std::wstring(L"T:\\Programmcode\\dataversepp\\dataverse\\src\\blob.cpp"),
            //    std::wstring(L"Eine mächtige C++-Datei"),
            //    std::wstring(L"/source/"),
            //    std::vector<std::wstring> { L"#bota", L"#boschofthemall" },
            //    true,
            //    [](const blob &r, void *) {
            //        std::cout << convert<char>(convert<wchar_t>(std::string(r.as<char>(), r.size()), CP_UTF8), CP_OEMCP) << std::endl;
            //        std::cout << std::endl;
            //    },
            //    [](const int, const char *, const char *, const narrow_string::code_page_type, void *) {},
            //    nullptr)
            //.upload(const_narrow_string("doi:10.15770/darus-1644", CP_OEMCP),
            //    const_narrow_string("T:\\Programmcode\\dataversepp\\dataverse\\src\\blob.cpp", CP_OEMCP),
            //    const_narrow_string("Eine mächtige C++-Datei", CP_OEMCP),
            //    const_narrow_string("/source/", CP_OEMCP),
            //    std::vector<const_narrow_string> { narrow_string("#bota", CP_OEMCP), narrow_string("#boschofthemall", CP_OEMCP) },
            //    true,
            //    [](const blob &r, void *) {
            //        std::cout << convert<char>(convert<wchar_t>(std::string(r.as<char>(), r.size()), CP_UTF8), CP_OEMCP) << std::endl;
            //        std::cout << std::endl;
            //    },
            //    [](const int, const char *, const char *, const narrow_string::code_page_type, void *) {},
            //    nullptr)
            .post(const_narrow_string("dataverses/visus/datasets", CP_OEMCP),
                data_set,
                [](const blob &r, void *) {
                    std::cout << convert<char>(convert<wchar_t>(std::string(r.as<char>(), r.size()), CP_UTF8), CP_OEMCP) << std::endl;
                    std::cout << std::endl;
                },
                [](const int, const char *, const char *, const narrow_string::code_page_type, void *) {},
                nullptr);


        //wait_event(e);

        return 0;
    } catch (std::exception& ex) {
        std::cout << ex.what() << std::endl << std::endl;
        return -1;
    }
}
