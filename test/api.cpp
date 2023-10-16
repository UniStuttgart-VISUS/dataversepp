// <copyright file="api.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "CppUnitTest.h"

#include <array>
#include <atomic>
#include <chrono>

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
                log_response("GET /dataverses/visus", response);
                const auto json = nlohmann::json::parse(response);
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                Assert::AreEqual(visus::dataverse::to_utf8(L"visus"), json["data"]["alias"].get<std::string>(), L"Dataverse alias", LINE_INFO());
                visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
            }, [](const int c, const char *m, const char *t, const visus::dataverse::narrow_string::code_page_type p, void *e) {
                log_response("GET /dataverses/visus", m);
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
                log_response("GET /dataverses/visus", response);
                const auto json = nlohmann::json::parse(response);
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                Assert::AreEqual(visus::dataverse::to_utf8(L"visus"), json["data"]["alias"].get<std::string>(), L"Dataverse alias", LINE_INFO());
                visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
            }, [](const int c, const char *m, const char *t, const visus::dataverse::narrow_string::code_page_type p, void *e) {
                log_response("GET /dataverses/visus", m);
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
            log_response("GET /dataverses/visus", json_text);
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
                    log_response("POST /dataverses/visus/datasets", response);
                    const auto json = nlohmann::json::parse(response);
                    Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                    visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                    Assert::IsTrue(true, L"Response callback invoked", LINE_INFO());
                } , [](const int c, const char *m, const char *t, const visus::dataverse::narrow_string::code_page_type p, void *e) {
                    log_response("POST /dataverses/visus/datasets", m);
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
            log_response("POST /dataverses/visus/datasets", rpost.dump());
            Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), rpost["status"].get<std::string>(), L"Response status", LINE_INFO());

            const auto persistent_id = rpost["data"]["persistentId"].get<std::string>();

            auto fget = this->_connection.data_set(visus::dataverse::make_narrow_string(persistent_id, CP_UTF8));
            fget.wait();
            const auto rget = fget.get();
            log_response("GET new data set", rget.dump());
            Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), rget["status"].get<std::string>(), L"Response status", LINE_INFO());
            Assert::AreEqual(persistent_id, rget["data"]["latestVersion"]["datasetPersistentId"].get<std::string>(), L"Retrieved the right data set", LINE_INFO());
        }

        TEST_METHOD(upload_file) {
            auto data_set = this->create_test_data_set(L"Upload Test (Callback)");

            struct {
                visus::dataverse::dataverse_connection *connection;
                visus::dataverse::event_type evt_done;
                std::pair<std::wstring, nlohmann::json> upload;
            } context = {
                &this->_connection,
                visus::dataverse::create_event(),
                create_test_file()
            };

            this->_connection.post(L"/dataverses/visus/datasets",
                data_set,
                [](const visus::dataverse::blob& r, void *c) {
                    auto cc = static_cast<decltype(context) *>(c);

                    const auto response = std::string(r.as<char>(), r.size());
                    log_response("POST /dataverses/visus/datasets", response);
                    const auto json = nlohmann::json::parse(response);
                    Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());

                    const auto persistent_id = visus::dataverse::convert<wchar_t>(json["data"]["persistentId"].get<std::string>(), CP_UTF8);
                    auto description = nlohmann::json::object({
                        { "description", visus::dataverse::to_utf8(L"The test driver.")},
                        { "restrict", true },
                    });

                    cc->connection->upload(persistent_id,
                        cc->upload.first,
                        description,
                        [](const nlohmann::json& r, void *c) {
                            auto cc = static_cast<decltype(context) *>(c);
                            auto response = r.dump();
                            log_response("POST file", response);
                            visus::dataverse::set_event(cc->evt_done);
                            Assert::IsTrue(true, L"Upload succeeded", LINE_INFO());
                        },
                        [](const int, const char *m, const char *, const visus::dataverse::narrow_string::code_page_type, void *c) {
                            auto cc = static_cast<decltype(context) *>(c);
                            log_response("POST file", m);
                            visus::dataverse::set_event(cc->evt_done);
                            Assert::Fail(L"Error callback invoked for upload", LINE_INFO());
                        },
                        c);
                    
                }, [](const int, const char *m, const char *, const visus::dataverse::narrow_string::code_page_type, void *c) {
                    auto cc = static_cast<decltype(context) *>(c);
                    log_response("POST /dataverses/visus/datasets", m);
                    visus::dataverse::set_event(cc->evt_done);
                    Assert::Fail(L"Error callback invoked for data set creation", LINE_INFO());
                }, &context);

            Assert::IsTrue(visus::dataverse::wait_event(context.evt_done, 60 * 1000), L"Operation completed in reasonable time", LINE_INFO());
            visus::dataverse::destroy_event(context.evt_done);
        }

        TEST_METHOD(upload_file_future) {
            const auto data_set = this->create_test_data_set(L"Upload Test (std::future)");
            const auto file = create_test_file();
            std::uint64_t data_set_id;
            std::uint64_t file_id;
            std::wstring persistent_id;

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
                auto future = this->_connection.upload(persistent_id, file.first, file.second);
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
                    log_response("POST /dataverses/visus_directupload/datasets", response);
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
                            log_response("POST direct file", response);
                            visus::dataverse::set_event(cc->evt_done);
                            Assert::IsTrue(true, L"Upload succeeded", LINE_INFO());
                        },
                        [](const int, const char *m, const char *, const visus::dataverse::narrow_string::code_page_type, void *c) {
                            auto cc = static_cast<decltype(context) *>(c);
                            log_response("POST direct file", m);
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
                log_response("POST /dataverses/visus_directupload/datasets", json.dump());
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
                log_response("POST direct file", json_text);
                const auto json = nlohmann::json::parse(json_text);
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
            }
        }

        TEST_METHOD(download_persistent_id_binary) {
            visus::dataverse::dataverse_connection dv;
            auto future = dv.base_path(L"https://darus.uni-stuttgart.de/api")
                .download(L"doi:10.18419/darus-3044/48");
            future.wait();
            auto content = future.get();
        }

        // Note: the following is really, really slow, becasue the file is huge.
#if false
        TEST_METHOD(download_persistent_id_table) {
            visus::dataverse::dataverse_connection dv;
            auto future = dv.base_path(L"https://darus.uni-stuttgart.de/api")
                .download(L"doi:10.18419/darus-3044/1");
            future.wait();
            auto content = future.get();
        }
#endif

        TEST_METHOD(get_persistent_id_binary) {
            visus::dataverse::dataverse_connection dv;
            auto future = dv.base_path(L"https://darus.uni-stuttgart.de/api")
                .get(L"/access/datafile/:persistentId?persistentId=doi:10.18419/darus-3044/48");
            future.wait();
            auto content = future.get();
        }

        TEST_METHOD(erase_data_set) {
            const auto data_set = this->create_test_data_set(L"Erase Data Set Test");
            std::uint64_t data_set_id;
            std::wstring data_set_persistent_id;

            {
                auto future = this->_connection.post(L"/dataverses/visus/datasets", data_set);
                future.wait();
                const auto json = future.get();
                const auto dump = json.dump();
                Logger::WriteMessage(dump.c_str());
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                data_set_id = json["data"]["id"].get<std::uint64_t>();
            }

            {
                auto evt_done = visus::dataverse::create_event();

                std::wstring resource(L"/datasets/");
                resource += std::to_wstring(data_set_id);
                resource += L"/versions/:draft";

                this->_connection.erase(resource.c_str(), [](const visus::dataverse::blob& r, void *e) {
                    const auto json = nlohmann::json::parse(r);
                    auto response = json.dump();
                    log_response("DELETE data set", response);
                    Assert::IsTrue(true, L"Erase succeeded", LINE_INFO());
                    visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                }, [](const int, const char *m, const char *, const visus::dataverse::narrow_string::code_page_type, void *e) {
                    log_response("DELETE data set", m);
                    Assert::Fail(L"Error callback invoked for erase", LINE_INFO());
                    visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                }, &evt_done);

                Assert::IsTrue(visus::dataverse::wait_event(evt_done, 60 * 1000), L"Operation completed in reasonable time", LINE_INFO());
                visus::dataverse::destroy_event(evt_done);
            }

            {
                std::wstring resource(L"/datasets/");
                resource += std::to_wstring(data_set_id);

                auto future = this->_connection.get(resource);
                Assert::ExpectException<std::runtime_error>([&future](void) {
                    future.get();
                }, L"Get deleted data set", LINE_INFO());
            }
        }

        TEST_METHOD(erase_data_set_future) {
            const auto data_set = this->create_test_data_set(L"Erase Data Set Test (std::future)");
            std::uint64_t data_set_id;
            std::wstring data_set_persistent_id;

            {
                auto future = this->_connection.post(L"/dataverses/visus/datasets", data_set);
                future.wait();
                const auto json = future.get();
                const auto dump = json.dump();
                Logger::WriteMessage(dump.c_str());
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                data_set_id = json["data"]["id"].get<std::uint64_t>();
            }

            {
                std::wstring resource(L"/datasets/");
                resource += std::to_wstring(data_set_id);
                resource += L"/versions/:draft";

                auto future = this->_connection.erase(resource);
                future.get();   // This just must not throw for the test to succeed.
            }

            {
                std::wstring resource(L"/datasets/");
                resource += std::to_wstring(data_set_id);

                auto future = this->_connection.get(resource);
                Assert::ExpectException<std::runtime_error>([&future](void) {
                    future.get();
                }, L"Get deleted data set", LINE_INFO());
            }
        }

#if false
        // TODO: This does not work. Why?!
        TEST_METHOD(erase_file_manually) {
            const auto data_set = this->create_test_data_set(L"Erase File Test");
            const auto file = create_test_file();
            std::uint64_t data_set_id;
            std::wstring data_set_persistent_id;
            std::uint64_t file_id;

            {
                auto future = this->_connection.post(L"/dataverses/visus/datasets", data_set);
                future.wait();
                const auto json = future.get();
                const auto dump = json.dump();
                Logger::WriteMessage(dump.c_str());
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                data_set_id = json["data"]["id"].get<std::uint64_t>();
                data_set_persistent_id = visus::dataverse::convert<wchar_t>(json["data"]["persistentId"].get<std::string>(), CP_UTF8);
            }

            {
                auto future = this->_connection.upload(data_set_persistent_id, file.first, file.second);
                future.wait();
                const auto json = future.get();
                const auto dump = json.dump();
                Logger::WriteMessage(dump.c_str());
                Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                const auto data = json["data"];
                Assert::IsTrue(data.type() == nlohmann::json::value_t::object, L"Payload data is object", LINE_INFO());
                const auto files = data["files"];
                Assert::IsTrue(files.type() == nlohmann::json::value_t::array, L"Files in data are array", LINE_INFO());
                Assert::AreEqual(std::size_t(1), files.size(), L"Array of one", LINE_INFO());
                file_id = files[0]["dataFile"]["id"].get<std::uint64_t>();
            }

            {
                auto evt_done = visus::dataverse::create_event();

                std::wstring resource(L"/files/");
                resource += std::to_wstring(file_id);

                this->_connection.erase(resource.c_str(), [](const visus::dataverse::blob& r, void *e) {
                    const auto json = nlohmann::json::parse(r);
                    auto response = json.dump();
                    log_response("DELETE file", response);
                    Assert::IsTrue(true, L"Erase succeeded", LINE_INFO());
                    visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                }, [](const int, const char *m, const char *, const visus::dataverse::narrow_string::code_page_type, void *e) {
                    log_response("DELETE file", m);
                    Assert::Fail(L"Error callback invoked for erase", LINE_INFO());
                    visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                }, &evt_done);

                Assert::IsTrue(visus::dataverse::wait_event(evt_done, 60 * 1000), L"Operation completed in reasonable time", LINE_INFO());
                visus::dataverse::destroy_event(evt_done);
            }
        }
#endif

        TEST_METHOD(create_stress_test) {
            const auto timestamp = std::chrono::steady_clock::now();
            const auto contact = nlohmann::json::object({
                { "contactEmail", visus::dataverse::to_utf8(L"querulant@visus.uni-stuttgart.de") }
            });
            const auto dataverse_desc = nlohmann::json::object({
                { "name", std::string("Create Data Set Stress Test") + this->_test_suffix },
                { "alias", std::string("visus_") + std::to_string(timestamp.time_since_epoch().count()) },
                { "dataverseContacts", nlohmann::json::array({ contact }) },
                { "affiliation", visus::dataverse::to_utf8(L"Universität Stuttgart") },
                { "description", "Container for data sets that are created by requests that are in flight in parallel." },
                { "dataverseType", "RESEARCH_PROJECTS" }
            });

            nlohmann::json dataverse;
            {
                auto future = this->_connection.post(L"/dataverses/visus", dataverse_desc);
                dataverse = future.get();
                log_response("POST /dataverses/visus", dataverse.dump());
            }

            {
                const std::size_t expected = 16;

                struct context_type {
                    visus::dataverse::event_type event;
                    std::atomic<std::size_t> counter;
                    std::string parent;
                    std::string request;
                } context = {
                    visus::dataverse::create_event(),
                    0,
                    dataverse["data"]["alias"].get<std::string>()
                };

                std::wstring request(L"/dataverses/");
                request += visus::dataverse::convert<wchar_t>(visus::dataverse::make_const_narrow_string(context.parent, CP_UTF8));
                request += L"/datasets";

                context.request = std::string("POST ") + visus::dataverse::convert<char>(request, CP_OEMCP);

                for (std::size_t i = 0; i < expected; ++i) {
                    const auto desc = this->create_test_data_set(std::to_wstring(i) + L" callback");
                    this->_connection.post(request.c_str(), desc,
                        [](const visus::dataverse::blob &r, void *c) {
                            const auto response = std::string(r.as<char>(), r.size());
                            auto context = static_cast<context_type *>(c);
                            log_response(context->request.c_str(), response);
                            const auto json = nlohmann::json::parse(response);
                            Assert::AreEqual(visus::dataverse::to_utf8(L"OK"), json["status"].get<std::string>(), L"Response status", LINE_INFO());
                            ++context->counter;
                            visus::dataverse::set_event(context->event);
                        }, [](const int c, const char *m, const char *t, const visus::dataverse::narrow_string::code_page_type p, void *cc) {
                            auto context = static_cast<context_type *>(cc);
                            log_response(context->request.c_str(), m);
                            ++context->counter;
                            visus::dataverse::set_event(context->event);
                            Assert::Fail(L"Error callback invoked", LINE_INFO());
                        },
                        &context);
                }

                for (std::size_t i = 0; i < expected; ++i) {
                    visus::dataverse::wait_event(context.event);
                }

                Assert::AreEqual(expected, context.counter.load(), L"All parallel requests finished", LINE_INFO());
            }

            //{
            //    const auto dataverse_alias = dataverse["data"]["alias"].get<std::string>();
            //    std::wstring request(L"/dataverses/");
            //    request += visus::dataverse::convert<wchar_t>(visus::dataverse::make_const_narrow_string(dataverse_alias, CP_UTF8));
            //    request += L"/datasets";

            //    std::array<std::future<nlohmann::json>, 16> futures;
            //    for (std::size_t i = 0; i < futures.size(); ++i) {
            //        const auto desc = this->create_test_data_set(std::to_wstring(i) + L" future");
            //        futures[i] = std::move(this->_connection.post(request.c_str(), desc));
            //    }

            //    for (auto& f : futures) {
            //        try {
            //            f.get();
            //        } catch (std::exception& ex) {
            //            auto msg = visus::dataverse::convert<wchar_t>(visus::dataverse::make_narrow_string(ex.what(), CP_OEMCP));
            //            Assert::Fail(msg.c_str(), LINE_INFO());
            //        }
            //    }
            //}
        }

        TEST_METHOD(error_callback) {
            auto evt_done = visus::dataverse::create_event();

            this->_connection.get(L"__bla_bla_bla__",
                [](const visus::dataverse::blob &r, void *e) {
                    visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                    Assert::Fail(L"Data callback invoked on invalid URL", LINE_INFO());
                }, [](const int c, const char *m, const char *t, const visus::dataverse::narrow_string::code_page_type p, void *e) {
                    Assert::IsTrue(true, L"Error callback invoked", LINE_INFO());
                    visus::dataverse::set_event(*static_cast<visus::dataverse::event_type *>(e));
                }, &evt_done);

            Assert::IsTrue(visus::dataverse::wait_event(evt_done, 60 * 1000), L"Operation completed in reasonable time", LINE_INFO());
            visus::dataverse::destroy_event(evt_done);
        }

        TEST_METHOD(error_future) {
            auto future = this->_connection.get(L"__bla_bla_bla__");
            Assert::ExpectException<std::runtime_error>([&future](void) {
                future.get();
            }, L"Exception thrown in future", LINE_INFO());
        }

    private:

        static inline void log_response(_In_z_ const char *request, _In_ const std::string& response) {
            const auto m = std::string(request) + ": " + response + "\r\n";
            Logger::WriteMessage(m.c_str());
        }

        static inline std::pair<std::wstring, nlohmann::json> create_test_file(void) {
            auto description = nlohmann::json::object({
                { "description", visus::dataverse::to_utf8(L"The test driver.")},
                { "restrict", true },
                { "categories", nlohmann::json::array({ "test", "future", "azure-devops" }) }
                });

            std::array<wchar_t, MAX_PATH> path;
            Assert::IsTrue(::GetModuleFileNameW(NULL, path.data(), static_cast<DWORD>(path.size())), L"GetModuleFileName", LINE_INFO());

            return std::make_pair(std::wstring(path.data()), description);
        }

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
