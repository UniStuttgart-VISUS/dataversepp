# dataversepp
This library provides a C++ API for uploading files to a [Dataverse](https://github.com/IQSS/dataverse) instance.

## Building
[CMake](https://cmake.org/) is required for building. It should obtain all dependencies automatically when configuring.

## Usage
```c++
dataverse_connection dataverse;
dataverse.base_path(L"https://darus.uni-stuttgart.de/api")
    .api_key(L"YOUR API KEY")
    .upload(L"doi:10.18419/darus-3044",
        L"T:\\data\\rtx4090.csv",
        L"Measurement results for GeForce RTX4090",
        L"/raw/",
        nullptr, 0, false,
        [](const blob& result, void *context) {
            std::string output(result.as<char>(), r.size());
            std::cout << convert<char>(convert<wchar_t>(output, CP_UTF8), CP_OEMCP) << std::endl;
            std::cout << std::endl;
        },
        [](const int error, const char_type *msg, const char_type *cat, void *context) {
            std::cerr << msg << std::endl << std::endl;
        });
```

When using narrow strings, you must tell the API about their encoding:
```c++
dataverse.base_path(const_narrow_string("https://darus.uni-stuttgart.de/api/", CP_OEMCP))
    .api_key(const_narrow_string("YOUR API KEY", CP_OEMCP))
    .upload(const_narrow_string("doi:10.18419/darus-3044", CP_OEMCP),
        const_narrow_string("T:\\data\\rtx4090.csv", CP_OEMCP),
        const_narrow_string("Measurement results for GeForce RTX4090", CP_OEMCP),
        const_narrow_string("/raw/", CP_OEMCP),
        std::vector<const_narrow_string> { narrow_string("#bota", CP_OEMCP), narrow_string("#boschofthemall", CP_OEMCP) },
        false,
        [](const blob& result, void *context) {
            std::string output(result.as<char>(), r.size());
            std::cout << convert<char>(convert<wchar_t>(output, CP_UTF8), CP_OEMCP) << std::endl;
            std::cout << std::endl;
        },
        [](const int error, const char_type *msg, const char_type *cat, void *context) {
            std::cerr << msg << std::endl << std::endl;
        });
```

Provided you have [nlohmann/json](https://github.com/nlohmann/json) installed, you can also make requests like for the creation of a new data set:
```c++
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

dataverse.base_path(L"https://darus.uni-stuttgart.de/api/")
    .api_key(L"YOUR API KEY")
    .post(L"dataverses/visus/datasets"),
        data_set,
        [](const blob& r, void *) {
            std::cout << convert<char>(convert<wchar_t>(std::string(r.as<char>(), r.size()), CP_UTF8), CP_OEMCP) << std::endl;
            std::cout << std::endl;
        },
        [](const int, const char *, const char *, const narrow_string::code_page_type, void *) {},
        nullptr);
```
Make sure that all string content is UTF-8. Just assuming that string literals are UTF-8 is not sufficient.
