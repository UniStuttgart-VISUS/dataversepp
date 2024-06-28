// <copyright file="dataverse_connection.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>


/*
 * visus::dataverse::dataverse_connection::invoke_async
 */
template<class TResult, class TOperation, class... TArgs>
std::future<TResult> visus::dataverse::dataverse_connection::invoke_async(
        TOperation&& operation, dataverse_connection& that,
        TArgs&&... arguments) {
    auto promise = new std::promise<TResult>();
    auto retval = promise->get_future();

    try {
        (that.*operation)(std::forward<TArgs>(arguments)...,
                [](const TResult& r, void *f) {
            // This was a success, so fulfil the promise.
            auto promise = static_cast<std::promise<TResult> *>(f);
            promise->set_value(r);
            delete promise;

        }, [](const int e, const char *m, const char *c,
                const narrow_string::code_page_type p, void *f) {
            // Rethrow to fulfil the promise. Unfortunately, we cannot use a
            // system_error here, because it would be too expensive to
            // reconstruct the error category from its name.
            auto promise = static_cast<std::promise<TResult> *>(f);
            try {
                throw std::runtime_error(m);
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
            delete promise;
        }, promise);

    } catch (...) {
        // The operation failed synchronously, but we need to fulfil our
        // promise as we have created a future from it ...
        promise->set_exception(std::current_exception());
        delete promise;
    }

    return retval;
}


/*
 * visus::dataverse::dataverse_connection::invoke_async
 */
template<class TOperation, class... TArgs>
std::future<void> visus::dataverse::dataverse_connection::invoke_async(
        TOperation&& operation, dataverse_connection& that,
        TArgs&&... arguments) {
    auto promise = new std::promise<void>();
    auto retval = promise->get_future();

    try {
        (that.*operation)(std::forward<TArgs>(arguments)...,
                [](const blob& r, void *f) {
            auto promise = static_cast<std::promise<void> *>(f);
            promise->set_value();
            delete promise;

        }, [](const int e, const char *m, const char *c,
                const narrow_string::code_page_type p, void *f) {
            auto promise = static_cast<std::promise<void> *>(f);
            try {
                throw std::runtime_error(m);
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
            delete promise;
        }, promise);

    } catch (...) {
        promise->set_exception(std::current_exception());
        delete promise;
    }

    return retval;
}


#if defined(DATAVERSE_WITH_JSON)
/*
 * visus::dataverse::dataverse_connection::translate_api_reponse
 */
template<class TJson>
void visus::dataverse::dataverse_connection::translate_api_reponse(
        _In_ const blob& response, _In_ void *client_data) {
    _Analysis_assume_(client_data != nullptr);
    const auto callback = get_on_api_response(client_data);
    const auto on_api_response = reinterpret_cast<on_api_response_type>(callback);
    _Analysis_assume_(on_api_response != nullptr);
    auto context = get_api_response_client_data(client_data);

    try {
        const auto r = std::string(response.as<char>(), response.size());
        const TJson json = TJson::parse(r);

        // TODO: this should not be necessary according to https://json.nlohmann.me/api/basic_json/get/
        std::string status;
        nlohmann::adl_serializer<std::string>::from_json(json, status);

        if (status == "ERROR") {
            report_api_error(context, status.c_str());
        } else {
            on_api_response(json, context);
        }

        //if (json["status"].get<std::string>() == "ERROR") {
        //    // TODO: this (content of 'message') seems to be wrong ...
        //    auto message = json["status"].get<std::string>();
        //    report_api_error(context, message.c_str());
        //} else {
        //    on_api_response(json, context);
        //}
    } catch (...) {
        // This should never happen unless an invalid end point was invoked.
        get_on_error(client_data)(0, "The translation of an API response "
            "failed with an unexpected exception.", "Unexpected Exception",
            dataversepp_code_page, context);
    }
}
#endif /* defined(DATAVERSE_WITH_JSON) */
