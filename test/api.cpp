// <copyright file="api.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "CppUnitTest.h"

#include <array>

#include <nlohmann/json.hpp>

#include "dataverse/dataverse_connection.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace test {

    TEST_CLASS(api) {

    public:

        api(void) {
            this->_connection.api_key(visus::dataverse::make_narrow_string(std::getenv("ApiKey"), CP_OEMCP));
            this->_connection.base_path(visus::dataverse::make_narrow_string(std::getenv("ApiEndPoint"), CP_OEMCP));

            {
                auto test_id = std::getenv("TestID");
                this->_test_suffix = visus::dataverse::to_utf8((test_id != nullptr)
                    ? std::wstring(L" - ") + visus::dataverse::convert<wchar_t>(test_id, 0, CP_OEMCP)
                    : std::wstring());
            }
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
                Logger::WriteMessage(m);
                visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                Assert::Fail(L"Error callback invoked", LINE_INFO());
            }, &evt_done);

            Assert::IsTrue(visus::dataverse::wait_event(evt_done, 60 * 1000), L"Operation completed in reasonable time", LINE_INFO());
            visus::dataverse::destroy_event(evt_done);
        }

        TEST_METHOD(get_dataverse_future) {
            auto future = this->_connection.get(L"/dataverses/visus");
            future.wait();
            const auto response = future.get();
            const auto json_text = std::string(response.as<char>(), response.size());
            Logger::WriteMessage(json_text.c_str());
            const auto json = nlohmann::json::parse(json_text);
            Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
            Assert::AreEqual(visus::dataverse::to_utf8(L"visus"), json["data"]["alias"].get<std::string>(), L"Dataverse alias", LINE_INFO());

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
                    visus::dataverse::to_utf8(L"Energy consumption of scientific visualisation and data visualisation algorithms") + this->_test_suffix),

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
                } , [](const int c, const char *m, const char *t, const visus::dataverse::narrow_string::code_page_type p, void *e) {
                    Logger::WriteMessage(m);
                    visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                    Assert::Fail(L"Error callback invoked", LINE_INFO());
                }, &evt_done);

            Assert::IsTrue(visus::dataverse::wait_event(evt_done, 60 * 1000), L"Operation completed in reasonable time", LINE_INFO());
            visus::dataverse::destroy_event(evt_done);
        }

        TEST_METHOD(upload_file) {
            auto data_set = this->create_test_data_set(L"Upload Test");

            struct {
                visus::dataverse::dataverse_connection *connection;
                visus::dataverse::event_type evt_done;
                std::wstring upload;
            } context;
            context.connection = &this->_connection;
            context.evt_done = visus::dataverse::create_event();

            {
                std::array<wchar_t, MAX_PATH> path;
                Assert::IsTrue(::GetModuleFileNameW(NULL, path.data(), static_cast<DWORD>(path.size())), L"GetModuleFileName", LINE_INFO());
                context.upload = path.data();
            }

            this->_connection.post(L"/dataverses/visus/datasets",
                data_set,
                [](const visus::dataverse::blob& r, void *c) {
                    auto cc = static_cast<decltype(context) *>(c);

                    const auto response = std::string(r.as<char>(), r.size());
                    Logger::WriteMessage(response.c_str());
                    const auto json = nlohmann::json::parse(response);
                    Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());

                    const auto persistent_id = visus::dataverse::convert<wchar_t>(json["data"]["persistentId"].get<std::string>(), CP_UTF8);
                    auto description = nlohmann::json::object({
                        { "description", visus::dataverse::to_utf8(L"The test driver.")},
                        { "restrict", true },
                    });

                    cc->connection->upload(persistent_id,
                        cc->upload,
                        description,
                        [](const nlohmann::json& r, void *c) {
                            auto cc = static_cast<decltype(context) *>(c);
                            auto response = r.dump();
                            Logger::WriteMessage(response.c_str());

                            visus::dataverse::set_event(cc->evt_done);
                            Assert::IsTrue(true, L"Upload succeeded", LINE_INFO());
                        },
                        [](const int, const char *m, const char *, const visus::dataverse::narrow_string::code_page_type, void *c) {
                            auto cc = static_cast<decltype(context) *>(c);
                            Logger::WriteMessage(m);
                            visus::dataverse::set_event(cc->evt_done);
                            Assert::Fail(L"Error callback invoked for upload", LINE_INFO());
                        },
                        c);
                    
                }, [](const int, const char *m, const char *, const visus::dataverse::narrow_string::code_page_type, void *c) {
                    auto cc = static_cast<decltype(context) *>(c);
                    Logger::WriteMessage(m);
                    visus::dataverse::set_event(cc->evt_done);
                    Assert::Fail(L"Error callback invoked for data set creation", LINE_INFO());
                }, &context);

            Assert::IsTrue(visus::dataverse::wait_event(context.evt_done, 60 * 1000), L"Operation completed in reasonable time", LINE_INFO());
            visus::dataverse::destroy_event(context.evt_done);
        }

        TEST_METHOD(upload_file_future) {
            std::wstring path;
            std::wstring persistent_id;

            {
                std::array<wchar_t, MAX_PATH> p;
                Assert::IsTrue(::GetModuleFileNameW(NULL, p.data(), static_cast<DWORD>(p.size())), L"GetModuleFileName", LINE_INFO());
                path = p.data();
            }

            {
                auto future = this->_connection.post(L"/dataverses/visus/datasets",
                    this->create_test_data_set(L"Upload Test (std::future)"));
                future.wait();
                const auto response = future.get();
                const auto json_text = std::string(response.as<char>(), response.size());
                const auto json = nlohmann::json::parse(json_text);
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());

                persistent_id = visus::dataverse::convert<wchar_t>(json["data"]["persistentId"].get<std::string>(), CP_UTF8);
            }

            {
                auto description = nlohmann::json::object({
                    { "description", visus::dataverse::to_utf8(L"The test driver.")},
                    { "restrict", true },
                    { "categories", nlohmann::json::array({ "test", "future", "azure-devops" }) }
                });

                auto future = this->_connection.upload(persistent_id, path, description);
                future.wait();
                const auto json = future.get();
                
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
            }
        }

        TEST_METHOD(direct_upload) {
            auto data_set = this->create_test_data_set(L"Direct Upload Test");

            struct {
                visus::dataverse::dataverse_connection *connection;
                visus::dataverse::event_type evt_done;
                std::wstring upload;
            } context;
            context.connection = &this->_connection;
            context.evt_done = visus::dataverse::create_event();

            {
                std::array<wchar_t, MAX_PATH> path;
                Assert::IsTrue(::GetModuleFileNameW(NULL, path.data(), static_cast<DWORD>(path.size())), L"GetModuleFileName", LINE_INFO());
                context.upload = path.data();
            }

            this->_connection.post(L"/dataverses/visus_directupload/datasets",
                data_set,
                [](const visus::dataverse::blob& r, void *c) {
                    auto cc = static_cast<decltype(context) *>(c);

                    const auto response = std::string(r.as<char>(), r.size());
                    Logger::WriteMessage(response.c_str());
                    const auto json = nlohmann::json::parse(response);
                    Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());

                    const auto persistent_id = visus::dataverse::convert<wchar_t>(json["data"]["persistentId"].get<std::string>(), CP_UTF8);

                    cc->connection->direct_upload(persistent_id,
                        cc->upload,
                        std::wstring(L"application/octet"),
                        std::wstring(L"The test driver."),
                        std::wstring(),
                        std::vector<std::wstring> { L"test", L"azure-devops" },
                        true,
                        [](const visus::dataverse::blob &r, void *c) {
                            auto cc = static_cast<decltype(context) *>(c);

                            const auto response = std::string(r.as<char>(), r.size());
                            Logger::WriteMessage(response.c_str());

                            visus::dataverse::set_event(cc->evt_done);
                            Assert::IsTrue(true, L"Upload succeeded", LINE_INFO());
                        },
                        [](const int, const char *m, const char *, const visus::dataverse::narrow_string::code_page_type, void *c) {
                            auto cc = static_cast<decltype(context) *>(c);
                            Logger::WriteMessage(m);
                            visus::dataverse::set_event(cc->evt_done);
                            Assert::Fail(L"Error callback invoked for upload", LINE_INFO());
                        },
                        c);
                    
                }, [](const int, const char *m, const char *, const visus::dataverse::narrow_string::code_page_type, void *c) {
                    auto cc = static_cast<decltype(context) *>(c);
                    Logger::WriteMessage(m);
                    visus::dataverse::set_event(cc->evt_done);
                    Assert::Fail(L"Error callback invoked for data set creation", LINE_INFO());
                }, &context);

            Assert::IsTrue(visus::dataverse::wait_event(context.evt_done, 60 * 1000), L"Operation completed in reasonable time", LINE_INFO());
            visus::dataverse::destroy_event(context.evt_done);
        }

        TEST_METHOD(direct_upload_future) {
            std::wstring persistent_id;
            std::wstring path;

            {
                std::array<wchar_t, MAX_PATH> p;
                Assert::IsTrue(::GetModuleFileNameW(NULL, p.data(), static_cast<DWORD>(p.size())), L"GetModuleFileName", LINE_INFO());
                path = p.data();
            }


            {
                auto future = this->_connection.post(L"/dataverses/visus_directupload/datasets",
                    this->create_test_data_set(L"Direct Upload Test (std::future)"));
                future.wait();
                const auto response = future.get();
                const auto json_text = std::string(response.as<char>(), response.size());
                const auto json = nlohmann::json::parse(json_text);
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());

                persistent_id = visus::dataverse::convert<wchar_t>(json["data"]["persistentId"].get<std::string>(), CP_UTF8);
            }

            {
                auto future = this->_connection.direct_upload(persistent_id,
                    path, std::wstring(L"application/octet"),
                    std::wstring(L"A test file"),
                    std::wstring(),
                    std::vector<std::wstring> { L"test", L"future", L"azure-devops" },
                    true);

                future.wait();
                const auto response = future.get();
                const auto json_text = std::string(response.as<char>(), response.size());
                const auto json = nlohmann::json::parse(json_text);
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
            }
        }

    private:

        inline nlohmann::json create_test_data_set(const std::wstring& title) {
            auto data_set = nlohmann::json({ });

            data_set["datasetVersion"]["license"]["name"] = "CC BY 4.0";
            data_set["datasetVersion"]["license"]["uri"] = "http://creativecommons.org/licenses/by/4.0/";
            data_set["datasetVersion"]["metadataBlocks"]["citation"] = visus::dataverse::json::make_citation_metadata(
                visus::dataverse::json::make_meta_field(
                    L"title",
                    L"primitive",
                    false,
                    visus::dataverse::to_utf8(title) + this->_test_suffix),

                visus::dataverse::json::make_meta_field(L"author", L"compound", true,
                    visus::dataverse::json::make_author(L"Müller, Christoph")),

                visus::dataverse::json::make_meta_field(L"datasetContact", L"compound", true,
                    visus::dataverse::json::make_contact(L"Azure Pipelines", L"noreply@visus.uni-stuttgart.de")),

                visus::dataverse::json::make_meta_field(L"dsDescription", L"compound", true,
                    visus::dataverse::json::make_data_desc(L"The is a test data set created from Azure Pipelines.")),

                visus::dataverse::json::make_meta_field(L"subject",
                    L"controlledVocabulary",
                    true,
                    visus::dataverse::to_utf8(L"Computer and Information Science"))
            );

            return data_set;
        }

        visus::dataverse::dataverse_connection _connection;
        std::string _test_suffix;

    };

} /* namespace test */
