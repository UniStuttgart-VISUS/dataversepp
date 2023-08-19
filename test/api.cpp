// <copyright file="api.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "CppUnitTest.h"

#include "dataverse/dataverse_connection.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace test {

    TEST_CLASS(api) {

    public:

        api(void) {
            this->_connection.api_key(visus::dataverse::make_narrow_string(std::getenv("ApiKey"), CP_OEMCP));
            this->_connection.base_path(visus::dataverse::make_narrow_string(std::getenv("ApiEndPoint"), CP_OEMCP));
        }

        TEST_METHOD(get_dataverse) {
            auto evt_done = visus::dataverse::create_event();

            this->_connection.get(L"/dataverse",
                [](const visus::dataverse::blob& r, void *e) {
                const auto response = std::string(r.as<char>(), r.size());
                //auto rr = convert<char>(convert<wchar_t>(response, CP_UTF8),
                //    CP_OEMCP);
                //std::cout << rr << std::endl;
                //std::cout << std::endl;
                visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                Assert::IsTrue(true, L"Response callback invoked", LINE_INFO());
            }, [](const int c, const char *m, const char *t, const visus::dataverse::narrow_string::code_page_type p, void *e) {
                visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                Assert::IsTrue(false, L"Error callback invoked", LINE_INFO());
            }, &evt_done);

            visus::dataverse::wait_event(evt_done);
        }

    private:

        visus::dataverse::dataverse_connection _connection;

    };

} /* namespace test */
