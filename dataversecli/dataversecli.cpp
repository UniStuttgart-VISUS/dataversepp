// <copyright file="dataversecli.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/dataverse_connection.h"
#include "dataverse/winsock_scope.h"

#include <iostream>

#include <map>
#include <vector>
#include <nlohmann/json.hpp>


int main() {
#if (defined(DEBUG) || defined(_DEBUG))
    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //::_CrtSetBreakAlloc(190);
#endif /* (defined(DEBUG) || defined(_DEBUG)) */

    try {
        visus::dataverse::winsock_scope w;

        visus::dataverse::dataverse_connection c;
        c.connect(DVSL("darus.uni-stuttgart.de"), 443, true);


        return 0;
    } catch (std::exception& ex) {
        std::cout << ex.what() << std::endl << std::endl;
        return -1;
    }
}
