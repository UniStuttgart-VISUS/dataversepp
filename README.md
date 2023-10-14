# dataverse++
[![Build Status - Azure Pipelines][build-button]][build-link]

[build-button]: https://img.shields.io/github/checks-status/UniStuttgart-VISUS/dataversepp/master?label=Azure%20Pipelines&logo=Azure%20Pipelines
[build-link]: https://devops.visus.uni-stuttgart.de/tfs/VIS(US)/Dataverse%20CI/_build/latest?definitionId=44&branchName=master

This library provides a C++ API for uploading files to a [Dataverse](https://github.com/IQSS/dataverse) instance. Its main purpose is enabling direct archival of research data from your simulation code or benchmarking experiments.

**Note that this library is currently in development.** We eventually plan to support Windows and Linux, but it is currently only running on Windows.

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
dataverse.base_path(const_narrow_string("https://darus.uni-stuttgart.de/api/", CP_ACP))
    .api_key(make_narrow_string("YOUR API KEY", CP_ACP))
    .upload(make_narrow_string("doi:10.18419/darus-3044", CP_ACP),
        make_narrow_string("T:\\data\\rtx4090.csv", CP_ACP),
        make_narrow_string("Measurement results for GeForce RTX4090", CP_ACP),
        make_narrow_string("/raw/", CP_ACP),
        std::vector<const_narrow_string> { narrow_string("#bota", CP_ACP), narrow_string("#boschofthemall", CP_ACP) },
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

> :warning: **The above code is only for illustration purposes.** You should never hard-code the code page like this in production code unless you are a MegaMol developer. Rather than hard-coding the encoding of your strings, you should track it from its origin. For instance, console input on Windows is `CP_OEMCP`. On Linux, you need to determine that using system calls or via libicu. The encoding of literal strings is compiler-dependent, but usually `CP_ACP`` on Windows. When using file input, you must ask the user or apply heuristics like detecing UTF-8 BOMs. While you might get away with hard coding for URLs and your API key, because these are most likely 7-bit ASCII anyway, **going the hard coding way has the potential to turn your metadata into rubbish.** The library will convert all metadata to UTF-8 based on your input in order to create the JSON descriptor objects. Specifying the wrong code page may corrupt the output. Specifying UTF-8 as the code page while the data are not actually UTF-8 may crash your application.

Provided you have [nlohmann/json](https://github.com/nlohmann/json) installed, you can also make requests like for the creation of a new data set:
```c++
auto data_set = nlohmann::json({ });

data_set["datasetVersion"]["license"]["name"] = "CC BY 4.0";
data_set["datasetVersion"]["license"]["uri"] = "http://creativecommons.org/licenses/by/4.0/";
data_set["datasetVersion"]["metadataBlocks"]["citation"] = visus::dataverse::json::make_citation_metadata(
    visus::dataverse::json::make_meta_field(
        L"title",
        L"primitive",
        false,
        visus::dataverse::to_utf8(L"Energy consumption of scientific visualisation and data visualisation algorithms")),

    visus::dataverse::json::make_meta_field(L"author", L"compound", true,
        visus::dataverse::json::make_author(L"Müller, Christoph"),
        visus::dataverse::json::make_author(L"Heinemann, Moritz"),
        visus::dataverse::json::make_author(L"Weiskopf, Daniel"),
        visus::dataverse::json::make_author(L"Ertl, Thomas")),

    visus::dataverse::json::make_meta_field(L"datasetContact", L"compound", true,
        visus::dataverse::json::make_contact(L"Querulant", L"querulant@visus.uni-stuttgart.de")),

    visus::dataverse::json::make_meta_field(L"dsDescription", L"compound", true,
        visus::dataverse::json::make_data_desc(L"This data set comprises a series of measurements of GPU power consumption.")),

    visus::dataverse::json::make_meta_field(L"subject",
        L"controlledVocabulary",
        true,
        visus::dataverse::to_utf8(L"Computer and Information Science"))
);
```
Make sure that all string content is UTF-8. Just assuming that string literals are UTF-8 is not sufficient. The construction of the meta data will fail if any non-UTF-8 code unit is discovered. It is best using the `to_utf8` functions provided by the library.
