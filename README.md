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
        }, [](const int error, const char_type *msg, const char_type *cat, void *context) {
            std::cerr << msg << std::endl << std::endl;
        });
```
