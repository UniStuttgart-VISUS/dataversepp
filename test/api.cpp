// <copyright file="api.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "CppUnitTest.h"

#include <nlohmann/json.hpp>

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

            this->_connection.get(L"/dataverses/visus",
                [](const visus::dataverse::blob& r, void *e) {
                const auto response = std::string(r.as<char>(), r.size());
                Logger::WriteMessage(response.c_str());
                const auto json = nlohmann::json::parse(response);
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                Assert::AreEqual(visus::dataverse::to_utf8(L"visus"), json["data"]["alias"].get<std::string>(), L"Dataverse alias", LINE_INFO());
                visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
            }, [](const int c, const char *m, const char *t, const visus::dataverse::narrow_string::code_page_type p, void *e) {
                visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                Assert::Fail(L"Error callback invoked", LINE_INFO());
            }, &evt_done);

            visus::dataverse::wait_event(evt_done);
        }

        TEST_METHOD(post_data_set) {
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
                    visus::dataverse::json::make_contact(L"Azure Pipelines", L"noreply@visus.uni-stuttgart.de")),

                visus::dataverse::json::make_meta_field(L"dsDescription", L"compound", true,
                    visus::dataverse::json::make_data_desc(L"This data set comprises a series of measurements of GPU power consumption.")),

                visus::dataverse::json::make_meta_field(L"subject",
                    L"controlledVocabulary",
                    true,
                    visus::dataverse::to_utf8(L"Computer and Information Science"))
            );

            auto evt_done = visus::dataverse::create_event();

            this->_connection.post(L"/dataverses/visus/datasets",
                data_set,
                [](const visus::dataverse::blob& r, void *e) {
                const auto response = std::string(r.as<char>(), r.size());
                Logger::WriteMessage(response.c_str());
                const auto json = nlohmann::json::parse(response);
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                Assert::IsTrue(true, L"Response callback invoked", LINE_INFO());
            }, [](const int c, const char *m, const char *t, const visus::dataverse::narrow_string::code_page_type p, void *e) {
                visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                Assert::Fail(L"Error callback invoked", LINE_INFO());
            }, &evt_done);

            visus::dataverse::wait_event(evt_done);
        }

    private:

        visus::dataverse::dataverse_connection _connection;

    };

} /* namespace test */
