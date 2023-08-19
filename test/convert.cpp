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
                Assert::AreEqual(expected, visus::dataverse::convert<dst_type>(input, 0, cp), L"standard input", LINE_INFO());
            }

            {
                const auto cp = CP_OEMCP;
                const std::basic_string<dst_type> expected = L"input";
                const auto input = visus::dataverse::make_narrow_string("input", cp);
                Assert::AreEqual(expected, visus::dataverse::convert<dst_type>(input), L"standard narrow input", LINE_INFO());
            }

            {
                const auto cp = CP_OEMCP;
                const std::basic_string<dst_type> expected = L"";
                const auto input = visus::dataverse::make_narrow_string("", cp);
                Assert::AreEqual(expected, visus::dataverse::convert<dst_type>(input), L"empty conversion", LINE_INFO());
            }

            {
                const auto cp = CP_OEMCP;
                const std::basic_string<dst_type> expected = L"";
                const auto input = "";
                Assert::AreEqual(expected, visus::dataverse::convert<dst_type>(input, 0, cp), L"empty narrow conversion", LINE_INFO());
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
                Assert::AreEqual(expected, visus::dataverse::convert<dst_type>(input, 0, cp), L"standard input", LINE_INFO());
            }

            {
                const std::basic_string<dst_type> expected = "";
                const auto input = L"";
                Assert::AreEqual(expected, visus::dataverse::convert<dst_type>(input, 0, cp), L"empty conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::ExpectException<std::invalid_argument>([&input, cp](void) {
                    visus::dataverse::convert<dst_type>(input, 0, cp), L"nullptr conversion", LINE_INFO();
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
                const auto actual = visus::dataverse::convert<dst_type>(input, 0, cp);
                Assert::AreEqual(expected, actual, L"standard input", LINE_INFO());
            }

            {
                const std::basic_string<dst_type> expected = "";
                const auto input = L"";
                const auto actual = visus::dataverse::convert<dst_type>(input, 0, cp);
                Assert::AreEqual(expected, actual, L"empty conversion", LINE_INFO());
            }

            {
                // This is incredibly hard to test without messing the constants ...
                // Reference from https://onlineutf8tools.com/convert-utf8-to-bytes.
                const std::vector<std::uint8_t> expected { 0xd0, 0xbf, 0xd1, 0x80, 0xd0, 0xb8, 0xd0, 0xb2, 0xd0, 0xb5, 0xd1, 0x82, 0 };
                const auto input = L"привет";
                const auto actual = visus::dataverse::convert<dst_type>(input, 0, cp);
                Assert::AreEqual(0, ::memcmp(expected.data(), actual.c_str(), expected.size()), L"non-ASCII conversion", LINE_INFO());
            }

            {
                const auto input = static_cast<src_type *>(nullptr);
                Assert::ExpectException<std::invalid_argument>([&input, cp](void) {
                    visus::dataverse::convert<dst_type>(input, 0, cp), L"nullptr conversion", LINE_INFO();
                });
            }
        }

    };

} /* namespace test */
