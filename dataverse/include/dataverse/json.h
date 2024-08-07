﻿// <copyright file="json.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#pragma once

// Enable JSON helper if the JSON library has been included before this header.
#if defined(NLOHMANN_JSON_NAMESPACE_BEGIN)
#define DATAVERSE_WITH_JSON (1)
#endif /* defined(NLOHMANN_JSON_NAMESPACE_BEGIN) */


#include "dataverse/convert.h"


#if defined(DATAVERSE_WITH_JSON)
namespace nlohmann {

    /// <summary>
    /// Allow for automatic conversion of <see cref="std::wstring" /> to JSON.
    /// </summary>
    template<> struct adl_serializer<std::wstring> {

        static inline void to_json(_Inout_ nlohmann::json &dst,
                _In_ const std::wstring &src) {
            dst = visus::dataverse::to_utf8(src);
        }

        static inline void from_json(_In_ const nlohmann::json &src,
                _Inout_ std::wstring &dst) {
            dst = visus::dataverse::from_utf8<wchar_t>(src.get<std::string>());
        }
    };

} /* namespace nlohmann */
#endif /* defined(DATAVERSE_WITH_JSON) */


namespace visus {
namespace dataverse {
namespace json {


#if defined(DATAVERSE_WITH_JSON)
    /// <summary>
    /// Creates a Dataverse metadata field with one value.
    /// </summary>
    /// <typeparam name="TValue"></typeparam>
    /// <param name="name"></param>
    /// <param name="clazz"></param>
    /// <param name="multiple"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    template<class TValue>
    inline nlohmann::json make_meta_field(_In_z_ const wchar_t *name,
            _In_z_ const wchar_t *clazz,
            _In_ const bool multiple,
            _In_ TValue&& value) {
        if (multiple) {
            // Note: GCC does not implicitly cast json::array and he value to
            // a compatible JSON type, so we cannot use the ternary here.
            return nlohmann::json({
                { "value", nlohmann::json::array({ value }) },
                { "typeClass", to_utf8(clazz) },
                { "typeName", to_utf8(name) },
                { "multiple", multiple }
            });

        } else {
            return nlohmann::json({
                { "value", value },
                { "typeClass", to_utf8(clazz) },
                { "typeName", to_utf8(name) },
                { "multiple", multiple }
            });
        }
    }

    /// <summary>
    /// Creates a Dataverse metadata field with multiple values.
    /// </summary>
    /// <typeparam name="TValues"></typeparam>
    /// <param name="type"></param>
    /// <param name="clazz"></param>
    /// <param name="values"></param>
    /// <returns></returns>
    template<class... TValues>
    inline nlohmann::json make_meta_field(_In_z_ const wchar_t *name,
            _In_z_ const wchar_t *clazz,
            _In_ const bool multiple,
            _In_ TValues&&... values) {
        return nlohmann::json({
            { "value", nlohmann::json::array({ values... }) },
            { "typeClass", to_utf8(clazz) },
            { "typeName", to_utf8(name) },
            { "multiple", multiple || (sizeof...(values) != 1) }
        });
    }

    /// <summary>
    /// Makes a compound metadata field for an author.
    /// </summary>
    /// <param name="name"></param>
    /// <param name="affiliation"></param>
    /// <param name="orcid"></param>
    /// <returns></returns>
    inline nlohmann::json make_author(_In_z_ const wchar_t *name,
            _In_opt_z_ const wchar_t *affiliation = nullptr,
            _In_opt_z_ const wchar_t *orcid = nullptr) {

        auto retval = nlohmann::json::object();
        retval["authorName"] = make_meta_field(L"authorName", L"primitive",
            false, to_utf8(name));

        if (affiliation != nullptr) {
            retval["authorAffiliation"] = make_meta_field(L"authorAffiliation",
                L"primitive", false, to_utf8(affiliation));
        }

        if (orcid != nullptr) {
            retval["authorIdentifierScheme"] = make_meta_field(
                L"authorIdentifierScheme", L"controlledVocabulary", L"ORCID");
            retval["authorIdentifier"] = make_meta_field(L"authorIdentifier",
                L"primitive", false, to_utf8(orcid));
        }

        return retval;
    }

    /// <summary>
    /// Creates a citation metadata object.
    /// </summary>
    /// <typeparam name="TFields"></typeparam>
    /// <param name="fields"></param>
    /// <returns></returns>
    template<class... TFields>
    inline nlohmann::json make_citation_metadata(_In_ TFields&&... fields) {
        return nlohmann::json::object({
            { "displayName", "Citation Metadata" },
            { "fields", nlohmann::json::array({ fields... }) }
        });
    }

    /// <summary>
    /// Makes a data set contact compound object.
    /// </summary>
    /// <param name="name"></param>
    /// <param name="email"></param>
    /// <returns></returns>
    inline nlohmann::json make_contact(_In_z_ const wchar_t *name,
            _In_z_ const wchar_t *email) {
        auto retval = nlohmann::json::object();
        retval["datasetContactEmail"] = make_meta_field(L"datasetContactEmail",
            L"primitive", false, to_utf8(email));
        retval["datasetContactName"] = make_meta_field(L"datasetContactName",
            L"primitive", false, to_utf8(name));
        return retval;
    }

    /// <summary>
    /// Makes a data set description compound object.
    /// </summary>
    /// <param name="desc"></param>
    /// <returns></returns>
    inline nlohmann::json make_data_desc(_In_z_ const wchar_t *desc) {
        auto retval = nlohmann::json::object();
        retval["dsDescriptionValue"] = make_meta_field(L"dsDescriptionValue",
            L"primitive", false, to_utf8(desc));
        return retval;
    }
#endif /* defined(DATAVERSE_WITH_JSON) */

} /* namespace json */
} /* namespace dataverse */
} /* namespace visus */
