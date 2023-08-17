// <copyright file="dataversecli.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include <WinSock2.h>
#include <Windows.h>

#include "dataverse/convert.h"
#include "dataverse/dataverse_connection.h"

#include <iostream>

#include <map>
#include <vector>
#include <nlohmann/json.hpp>


int main() {
    using namespace visus::dataverse;
#if (defined(DEBUG) || defined(_DEBUG))
    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //::_CrtSetBreakAlloc(190);
#endif /* (defined(DEBUG) || defined(_DEBUG)) */

    try {
        dataverse_connection c;
        c.connect(L"darus.uni-stuttgart.de", 443)
            .base_path(L"https://darus.uni-stuttgart.de/api")
            .api_key(L"");
        c.get(DVSL("/dataverses/tr161"), [](const blob &r, void *) {
            std::cout << convert<char>(convert<wchar_t>(std::string(r.as<char>(), r.size()), CP_UTF8), CP_OEMCP) << std::endl;
        }, [](const int, const char_type *, const char_type *, void *) {
        });

        

        //wait_event(e);

        return 0;
    } catch (std::exception& ex) {
        std::cout << ex.what() << std::endl << std::endl;
        return -1;
    }
}
