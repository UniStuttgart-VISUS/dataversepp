// <copyright file="dataversecli.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/dataverse_connection.h"


int main() {

    visus::dataverse::dataverse_connection c;
    sockaddr_in a;
    ::ZeroMemory(&a, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_port = ::htons(80);
    a.sin_addr.S_un.S_addr = INADDR_LOOPBACK;
    //c.connect(reinterpret_cast<sockaddr *>(&a), sizeof(a));

    c.__hack();

    return 0;
}
