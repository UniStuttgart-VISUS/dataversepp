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
dataverse.base_path(const_narrow_string(L"https://darus.uni-stuttgart.de/api", CP_OEMCP))
    .api_key(const_narrow_string("YOUR API KEY", CP_OEMCP))
    .upload(const_narrow_string(L"doi:10.18419/darus-3044", CP_OEMCP),
        const_narrow_string("T:\\data\\rtx4090.csv", CP_OEMCP),
        const_narrow_string(L"Measurement results for GeForce RTX4090", CP_OEMCP),
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

