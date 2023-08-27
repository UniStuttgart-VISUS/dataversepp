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

        TEST_METHOD(get_dataverse_ansi) {
            auto evt_done = visus::dataverse::create_event();

            this->_connection.get(visus::dataverse::make_narrow_string("/dataverses/visus", CP_ACP),
                    [](const visus::dataverse::blob &r, void *e) {
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

        TEST_METHOD(get_dataverse_unicode) {
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
            const auto title = visus::dataverse::to_utf8(L"Energy consumption of scientific visualisation and data visualisation algorithms") + this->_test_suffix;
            auto data_set = nlohmann::json({ });

            data_set["datasetVersion"]["license"]["name"] = "CC BY 4.0";
            data_set["datasetVersion"]["license"]["uri"] = "http://creativecommons.org/licenses/by/4.0/";
            data_set["datasetVersion"]["metadataBlocks"]["citation"] = visus::dataverse::json::make_citation_metadata(
                visus::dataverse::json::make_meta_field(L"title", L"primitive", false, title),

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

        TEST_METHOD(post_get_data_set) {
            auto data_set = this->create_test_data_set(L"Post data set (std::future)");

            auto fpost = this->_connection.post(L"/dataverses/visus/datasets", data_set);
            fpost.wait();

            const auto rpost = fpost.get();
            Logger::WriteMessage(rpost.dump().c_str());
            Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), rpost["status"].get<std::string>(), L"Response status", LINE_INFO());

            const auto persistent_id = rpost["data"]["persistentId"].get<std::string>();

            auto fget = this->_connection.data_set(visus::dataverse::make_narrow_string(persistent_id, CP_UTF8));
            fget.wait();
            const auto rget = fget.get();
            const auto rdump = rget.dump();
            Logger::WriteMessage(rdump.c_str());
            Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), rget["status"].get<std::string>(), L"Response status", LINE_INFO());
            Assert::AreEqual(persistent_id, rget["data"]["latestVersion"]["datasetPersistentId"].get<std::string>(), L"Retrieved the right data set", LINE_INFO());
        }

        TEST_METHOD(upload_file) {
            auto data_set = this->create_test_data_set(L"Upload Test (Callback)");

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
            const auto data_set = this->create_test_data_set(L"Upload Test (std::future)");
            const auto description = nlohmann::json::object({
                { "description", visus::dataverse::to_utf8(L"The test driver.")},
                { "restrict", true },
                { "categories", nlohmann::json::array({ "test", "future", "azure-devops" }) }
            });
            std::uint64_t data_set_id;
            std::uint64_t file_id;
            std::wstring path;
            std::wstring persistent_id;

            {
                std::array<wchar_t, MAX_PATH> p;
                Assert::IsTrue(::GetModuleFileNameW(NULL, p.data(), static_cast<DWORD>(p.size())), L"GetModuleFileName", LINE_INFO());
                path = p.data();
            }

            {
                auto future = this->_connection.post(L"/dataverses/visus/datasets", data_set);
                future.wait();
                const auto json = future.get();
                const auto dump = json.dump();
                Logger::WriteMessage(dump.c_str());
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                data_set_id = json["data"]["id"].get<std::uint64_t>();
                persistent_id = visus::dataverse::convert<wchar_t>(json["data"]["persistentId"].get<std::string>(), CP_UTF8);
            }

            {
                auto future = this->_connection.upload(persistent_id, path, description);
                future.wait();
                const auto json = future.get();
                const auto dump = json.dump();
                Logger::WriteMessage(dump.c_str());
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
            }

            {
                auto future = this->_connection.files(data_set_id, visus::dataverse::dataverse_connection::draught_version);
                future.wait();
                const auto json = future.get();
                const auto dump = json.dump();
                Logger::WriteMessage(dump.c_str());
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                Assert::IsTrue(json["data"].type() == nlohmann::json::value_t::array, L"Array", LINE_INFO());
                Assert::AreEqual(std::size_t(1), json["data"].size(), L"Array of one", LINE_INFO());
                file_id = json["data"][0]["dataFile"]["id"].get<std::uint64_t>();
            }

            {
                auto future = this->_connection.download(file_id);
                future.wait();
                const auto content = future.get();
                const auto size = content.size();
            }
        }

        TEST_METHOD(direct_upload) {
            const auto data_set = this->create_test_data_set(L"Direct Upload Test (Callback)");

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

            this->_connection.post(L"/dataverses/visus_directupload/datasets", data_set,
                [](const visus::dataverse::blob& r, void *c) {
                    auto cc = static_cast<decltype(context) *>(c);

                    const auto response = std::string(r.as<char>(), r.size());
                    Logger::WriteMessage(response.c_str());
                    const auto json = nlohmann::json::parse(response);
                    Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());

                    const auto persistent_id = visus::dataverse::convert<wchar_t>(json["data"]["persistentId"].get<std::string>(), CP_UTF8);

                    cc->connection->direct_upload(persistent_id,
                        cc->upload,
                        std::wstring(L"application/octet-stream"),
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
            const auto data_set = this->create_test_data_set(L"Direct Upload Test (std::future)");
            std::wstring persistent_id;
            std::wstring path;

            {
                std::array<wchar_t, MAX_PATH> p;
                Assert::IsTrue(::GetModuleFileNameW(NULL, p.data(), static_cast<DWORD>(p.size())), L"GetModuleFileName", LINE_INFO());
                path = p.data();
            }

            {
                auto future = this->_connection.post(L"/dataverses/visus_directupload/datasets", data_set);
                future.wait();
                const auto json = future.get();
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());

                persistent_id = visus::dataverse::convert<wchar_t>(json["data"]["persistentId"].get<std::string>(), CP_UTF8);
            }

            {
                auto future = this->_connection.direct_upload(persistent_id,
                    path, std::wstring(L"application/octet-stream"),
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
            const auto specific_title = visus::dataverse::to_utf8(title) + this->_test_suffix;
            auto data_set = nlohmann::json({ });

            data_set["datasetVersion"]["license"]["name"] = "CC BY 4.0";
            data_set["datasetVersion"]["license"]["uri"] = "http://creativecommons.org/licenses/by/4.0/";
            data_set["datasetVersion"]["metadataBlocks"]["citation"] = visus::dataverse::json::make_citation_metadata(
                visus::dataverse::json::make_meta_field(L"title", L"primitive", false, specific_title),

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
