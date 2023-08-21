// <copyright file="dataverse_connection.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>


/*
 * visus::dataverse::dataverse_connection::invoke_async
 */
template<class TOperation, class... TArgs>
std::future<visus::dataverse::blob>
visus::dataverse::dataverse_connection::invoke_async(TOperation&& operation,
        dataverse_connection& that, TArgs&&... arguments) {
    auto promise = new std::promise<blob>();
    auto retval = promise->get_future();

    try {
        (that.*operation)(std::forward<TArgs>(arguments)...,
                [](const blob& r, void *f) {
            // This was a success, so fulfil the promise.
            auto promise = static_cast<std::promise<blob> *>(f);
            promise->set_value(r);
            delete promise;

        }, [](const int e, const char *m, const char *c,
                const narrow_string::code_page_type p, void *f) {
            // Rethrow to fulfil the promise. Unfortunately, we cannot use a
            // system_error here, because it would be too expensive to
            // reconstruct the error category from its name.
            auto promise = static_cast<std::promise<blob> *>(f);
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
