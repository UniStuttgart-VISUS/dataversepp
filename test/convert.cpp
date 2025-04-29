// <copyright file="blob.cpp" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "CppUnitTest.h"

#include "dataverse/convert.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace test {

    TEST_CLASS(convert) {

    public:

        TEST_METHOD(oem2wchar) {
            typedef char src_type;
            typedef wchar_t dst_type;

            {
                const auto cp = CP_OEMCP;
                const std::basic_string<dst_type> expected = L"input";
                const auto input = "input";
                const auto actual = visus::dataverse::convert<dst_type>(input, -1, cp);
                Assert::AreEqual(expected, actual, L"standard input", LINE_INFO());
            }

            {
                const auto cp = CP_OEMCP;
                const std::basic_string<dst_type> expected = L"input";
                const auto input = visus::dataverse::make_narrow_string("input", cp);
                const auto actual = visus::dataverse::convert<dst_type>(input);
                Assert::AreEqual(expected, actual, L"standard narrow input", LINE_INFO());
            }

            {
                const auto cp = CP_OEMCP;
                const std::basic_string<dst_type> expected = L"";
                const auto input = visus::dataverse::make_narrow_string("", cp);
                const auto actual = visus::dataverse::convert<dst_type>(input);
                Assert::AreEqual(expected, actual, L"empty conversion", LINE_INFO());
            }

            {
                const auto cp = CP_OEMCP;
                const std::basic_string<dst_type> expected = L"";
                const auto input = "";
                const auto actual = visus::dataverse::convert<dst_type>(input, -1, cp);
                Assert::AreEqual(expected, actual, L"empty narrow conversion", LINE_INFO());
            }

            {
                const auto cp = CP_OEMCP;
                const auto input = static_cast<src_type *>(nullptr);
                Assert::ExpectException<std::invalid_argument>([&input, cp](void) {
                    visus::dataverse::convert<dst_type>(input, 0, cp), L"nullptr conversion", LINE_INFO();
                });
            }

            {
                const auto cp = CP_OEMCP;
                const auto input = visus::dataverse::make_narrow_string(static_cast<src_type *>(nullptr), cp);
                Assert::ExpectException<std::invalid_argument>([&input](void) {
                    visus::dataverse::convert<dst_type>(input), L"nullptr narrow conversion", LINE_INFO();
                });
            }
        }

        TEST_METHOD(wchar2oem) {
            const auto cp = CP_OEMCP;
            typedef wchar_t src_type;
            typedef char dst_type;

            {
                const std::basic_string<dst_type> expected = "input";
                const auto input = L"input";
                Assert::AreEqual(expected, visus::dataverse::convert<dst_type>(input, -1, cp), L"standard input", LINE_INFO());
            }

            {
                const std::basic_string<dst_type> expected = "";
                const auto input = L"";
                Assert::AreEqual(expected, visus::dataverse::convert<dst_type>(input, -1, cp), L"empty conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::ExpectException<std::invalid_argument>([&input, cp](void) {
                    visus::dataverse::convert<dst_type>(input, -1, cp), L"nullptr conversion", LINE_INFO();
                });
            }
        }

        TEST_METHOD(wchar2utf8) {
            const auto cp = CP_UTF8;
            typedef wchar_t src_type;
            typedef char dst_type;

            {
                const std::basic_string<dst_type> expected = "input";
                const auto input = L"input";
                const auto actual = visus::dataverse::convert<dst_type>(input, -1, cp);
                Assert::AreEqual(expected, actual, L"standard input", LINE_INFO());
            }

            {
                const std::basic_string<dst_type> expected = "";
                const auto input = L"";
                const auto actual = visus::dataverse::convert<dst_type>(input, -1, cp);
                Assert::AreEqual(expected, actual, L"empty conversion", LINE_INFO());
            }

            {
                // This is incredibly hard to test without messing the constants ...
                // Reference from https://onlineutf8tools.com/convert-utf8-to-bytes.
                const std::vector<std::uint8_t> expected { 0xd0, 0xbf, 0xd1, 0x80, 0xd0, 0xb8, 0xd0, 0xb2, 0xd0, 0xb5, 0xd1, 0x82, 0 };
                const auto input = L"привет";
                const auto actual = visus::dataverse::convert<dst_type>(input, -1, cp);
                Assert::AreEqual(0, ::memcmp(expected.data(), actual.c_str(), expected.size()), L"non-ASCII conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::ExpectException<std::invalid_argument>([&input, cp](void) {
                    visus::dataverse::convert<dst_type>(input, -1, cp), L"nullptr conversion", LINE_INFO();
                });
            }
        }

        TEST_METHOD(make_narrow_string) {
            {
                auto input = "Müller";
                const auto actual = visus::dataverse::make_narrow_string(input, CP_OEMCP);
                Assert::AreEqual(input, actual.value(), L"value", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual.code_page(), L"code_page", LINE_INFO());
            }

            {
                std::string input = "Müller";
                const auto actual = visus::dataverse::make_narrow_string(input, CP_OEMCP);
                Assert::AreEqual(input.c_str(), actual.value(), L"value", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual.code_page(), L"code_page", LINE_INFO());
            }

            Assert::ExpectException<std::logic_error>([](void) {
                const auto actual = visus::dataverse::make_narrow_string(std::string("Müller"), CP_OEMCP);
            }, L"No temporary", LINE_INFO());
        }

        TEST_METHOD(make_narrow_strings) {
            {
                std::vector<char *> input = { "Müller", "Dörr", "Lämmle", "Schneegaß" };
                const auto actual = visus::dataverse::make_narrow_strings(input.begin(), input.end(), CP_OEMCP);
                Assert::AreEqual(input[0], actual[0].value(), L"value[0]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[0].code_page(), L"code_page[0]", LINE_INFO());
                Assert::AreEqual(input[1], actual[1].value(), L"value[1]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[1].code_page(), L"code_page[1]", LINE_INFO());
                Assert::AreEqual(input[2], actual[2].value(), L"value[2]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[2].code_page(), L"code_page[2]", LINE_INFO());
            }

            {
                std::vector<const char *> input = { "Müller", "Dörr", "Lämmle", "Schneegaß" };
                const auto actual = visus::dataverse::make_narrow_strings(input.begin(), input.end(), CP_OEMCP);
                Assert::AreEqual(input[0], actual[0].value(), L"value[0]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[0].code_page(), L"code_page[0]", LINE_INFO());
                Assert::AreEqual(input[1], actual[1].value(), L"value[1]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[1].code_page(), L"code_page[1]", LINE_INFO());
                Assert::AreEqual(input[2], actual[2].value(), L"value[2]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[2].code_page(), L"code_page[2]", LINE_INFO());
            }

            {
                std::vector<std::string> input = { "Müller", "Dörr", "Schneegaß" };
                const auto actual = visus::dataverse::make_narrow_strings(input.begin(), input.end(), CP_OEMCP);
                Assert::AreEqual(input[0].c_str(), actual[0].value(), L"value[0]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[0].code_page(), L"code_page[0]", LINE_INFO());
                Assert::AreEqual(input[1].c_str(), actual[1].value(), L"value[1]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[1].code_page(), L"code_page[1]", LINE_INFO());
                Assert::AreEqual(input[2].c_str(), actual[2].value(), L"value[2]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[2].code_page(), L"code_page[2]", LINE_INFO());
            }

            {
                std::vector<std::string> input = { "Müller", "Dörr", "Schneegaß" };
                std::vector<visus::dataverse::const_narrow_string> actual;
                actual.reserve(input.size());
                visus::dataverse::make_narrow_strings(std::back_inserter(actual), input.begin(), input.end(), CP_OEMCP);
                Assert::AreEqual(input[0].c_str(), actual[0].value(), L"value[0]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[0].code_page(), L"code_page[0]", LINE_INFO());
                Assert::AreEqual(input[1].c_str(), actual[1].value(), L"value[1]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[1].code_page(), L"code_page[1]", LINE_INFO());
                Assert::AreEqual(input[2].c_str(), actual[2].value(), L"value[2]", LINE_INFO());
                Assert::AreEqual(unsigned int(CP_OEMCP), actual[2].code_page(), L"code_page[2]", LINE_INFO());
            }
        }

        TEST_METHOD(narrow_to_ascii) {
            typedef char src_type;
            typedef char dst_type;

            {
                const std::basic_string<dst_type> expected = "input";
                const auto input = "input";
                const auto actual = visus::dataverse::to_ascii(input, -1);
                Assert::AreEqual(expected, actual, L"standard input", LINE_INFO());
            }

            {
                const auto cp = CP_OEMCP;
                const std::basic_string<dst_type> expected = "input";
                const auto input = visus::dataverse::make_narrow_string("input", cp);
                const auto actual = visus::dataverse::to_ascii(input);
                Assert::AreEqual(expected, actual, L"standard narrow input", LINE_INFO());
            }

            {
                const auto cp = CP_OEMCP;
                const std::basic_string<dst_type> expected = "";
                const auto input = visus::dataverse::make_narrow_string("", cp);
                const auto actual = visus::dataverse::to_ascii(input);
                Assert::AreEqual(expected, actual, L"empty conversion", LINE_INFO());
            }

            {
                const auto cp = CP_OEMCP;
                const std::basic_string<dst_type> expected = "";
                const auto input = "";
                const auto actual = visus::dataverse::to_ascii(input, -1);
                Assert::AreEqual(expected, actual, L"empty narrow conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::ExpectException<std::invalid_argument>([&input](void) {
                    visus::dataverse::to_ascii(input, CP_OEMCP), L"nullptr conversion", LINE_INFO();
                });
            }

            {
                const auto cp = CP_OEMCP;
                const auto input = visus::dataverse::make_narrow_string(static_cast<src_type *>(nullptr), cp);
                Assert::ExpectException<std::invalid_argument>([&input](void) {
                    visus::dataverse::to_ascii(input), L"nullptr narrow conversion", LINE_INFO();
                });
            }

        }

    };

} /* namespace test */
