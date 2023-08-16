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
    using namespace visus::dataverse;
#if (defined(DEBUG) || defined(_DEBUG))
    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //::_CrtSetBreakAlloc(190);
#endif /* (defined(DEBUG) || defined(_DEBUG)) */

    try {
        winsock_scope w;

        auto e = create_event();

        dataverse_connection c;
        c.connect(DVSL("www.visus.uni-stuttgart.de"), 443, true);
        c.get("/", [](_In_ dataverse_connection *, _In_ const http_response&, _In_opt_ void *e) {
            set_event(*reinterpret_cast<event_type *>(e));
        }, nullptr, nullptr, &e);

        wait_event(e);

        return 0;
    } catch (std::exception& ex) {
        std::cout << ex.what() << std::endl << std::endl;
        return -1;
    }
}
