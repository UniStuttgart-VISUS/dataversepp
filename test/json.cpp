// <copyright file="blob.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "CppUnitTest.h"

#include <nlohmann/json.hpp>

#include "dataverse/json.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace test {

    TEST_CLASS(json) {

    public:

        TEST_METHOD(data_set) {
            const auto licence_name = "CC BY 4.0";
            const auto licence_uri = "http://creativecommons.org/licenses/by/4.0/";
            const auto citation_display_name = L"Citation Metadata";
            const auto title = visus::dataverse::to_utf8(L"Energy consumption of scientific visualisation and data visualisation algorithms");
            const auto first_author = L"Müller, Christoph";
            const auto contact_name = L"Querulant";
            const auto contact_email = L"querulant@visus.uni-stuttgart.de";

            auto data_set = nlohmann::json({ });

            data_set["datasetVersion"]["license"]["name"] = licence_name;
            data_set["datasetVersion"]["license"]["uri"] = licence_uri;
            data_set["datasetVersion"]["metadataBlocks"]["citation"] = visus::dataverse::json::make_citation_metadata(
                visus::dataverse::json::make_meta_field(
                    L"title",
                    L"primitive",
                    false,
                    title),

                visus::dataverse::json::make_meta_field(L"author", L"compound", true,
                    visus::dataverse::json::make_author(first_author),
                    visus::dataverse::json::make_author(L"Heinemann, Moritz"),
                    visus::dataverse::json::make_author(L"Weiskopf, Daniel"),
                    visus::dataverse::json::make_author(L"Ertl, Thomas")),

                visus::dataverse::json::make_meta_field(L"datasetContact", L"compound", true,
                    visus::dataverse::json::make_contact(contact_name, contact_email)),

                visus::dataverse::json::make_meta_field(L"dsDescription", L"compound", true,
                    visus::dataverse::json::make_data_desc(L"This data set comprises a series of measurements of GPU power consumption.")),

                visus::dataverse::json::make_meta_field(L"subject",
                    L"controlledVocabulary",
                    true,
                    visus::dataverse::to_utf8(L"Computer and Information Science"))
            );

            Assert::AreEqual(visus::dataverse::to_utf8(licence_name, CP_OEMCP), data_set["datasetVersion"]["license"]["name"].get<std::string>(), L"Licence name", LINE_INFO());
            Assert::AreEqual(visus::dataverse::to_utf8(licence_uri, CP_OEMCP), data_set["datasetVersion"]["license"]["uri"].get<std::string>(), L"Licence URI", LINE_INFO());

            Assert::AreEqual(visus::dataverse::to_utf8(citation_display_name), data_set["datasetVersion"]["metadataBlocks"]["citation"]["displayName"].get<std::string>(), L"Citation display name", LINE_INFO());
            Assert::AreEqual(std::size_t(5), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"].size(), L"Citation size", LINE_INFO());

            Assert::AreEqual(title, data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][0]["value"].get<std::string>(), L"Title value", LINE_INFO());
            Assert::AreEqual(std::string("title"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][0]["typeName"].get<std::string>(), L"Title typeName", LINE_INFO());
            Assert::AreEqual(std::string("primitive"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][0]["typeClass"].get<std::string>(), L"Title typeClass", LINE_INFO());
            Assert::IsFalse(data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][0]["multiple"].get<bool>(), L"Title multiple", LINE_INFO());

            Assert::AreEqual(std::string("author"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][1]["typeName"].get<std::string>(), L"Author typeName", LINE_INFO());
            Assert::AreEqual(std::string("compound"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][1]["typeClass"].get<std::string>(), L"Author typeClass", LINE_INFO());
            Assert::IsTrue(data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][1]["multiple"].get<bool>(), L"Author multiple", LINE_INFO());
            Assert::AreEqual(std::size_t(4), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][1]["value"].size(), L"Author size", LINE_INFO());

            Assert::AreEqual(visus::dataverse::to_utf8(first_author), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][1]["value"][0]["authorName"]["value"].get<std::string>(), L"First author's name", LINE_INFO());
            Assert::AreEqual(std::string("authorName"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][1]["value"][0]["authorName"]["typeName"].get<std::string>(), L"First author's typeName", LINE_INFO());
            Assert::AreEqual(std::string("primitive"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][1]["value"][0]["authorName"]["typeClass"].get<std::string>(), L"First author's typeClass", LINE_INFO());
            Assert::IsFalse(data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][1]["value"][0]["authorName"]["multiple"].get<bool>(), L"First author's multiple", LINE_INFO());

            Assert::AreEqual(std::string("datasetContact"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["typeName"].get<std::string>(), L"Contact typeName", LINE_INFO());
            Assert::AreEqual(std::string("compound"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["typeClass"].get<std::string>(), L"Contact typeClass", LINE_INFO());
            Assert::IsTrue(data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["multiple"].get<bool>(), L"Contact multiple", LINE_INFO());
            Assert::AreEqual(std::size_t(1), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["value"].size(), L"Contact size", LINE_INFO());

            Assert::AreEqual(visus::dataverse::to_utf8(contact_name), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["value"][0]["datasetContactName"]["value"].get<std::string>(), L"Contact name", LINE_INFO());
            Assert::AreEqual(std::string("datasetContactName"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["value"][0]["datasetContactName"]["typeName"].get<std::string>(), L"Contact name typeName", LINE_INFO());
            Assert::AreEqual(std::string("primitive"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["value"][0]["datasetContactName"]["typeClass"].get<std::string>(), L"Contact name typeClass", LINE_INFO());
            Assert::IsFalse(data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["value"][0]["datasetContactName"]["multiple"].get<bool>(), L"Contact name multiple", LINE_INFO());

            Assert::AreEqual(visus::dataverse::to_utf8(contact_email), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["value"][0]["datasetContactEmail"]["value"].get<std::string>(), L"Contact email", LINE_INFO());
            Assert::AreEqual(std::string("datasetContactEmail"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["value"][0]["datasetContactEmail"]["typeName"].get<std::string>(), L"Contact email typeName", LINE_INFO());
            Assert::AreEqual(std::string("primitive"), data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["value"][0]["datasetContactEmail"]["typeClass"].get<std::string>(), L"Contact email typeClass", LINE_INFO());
            Assert::IsFalse(data_set["datasetVersion"]["metadataBlocks"]["citation"]["fields"][2]["value"][0]["datasetContactEmail"]["multiple"].get<bool>(), L"Contact email multiple", LINE_INFO());

        }

    };

} /* namespace test */
