// <copyright file="convert.inl" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>


/*
 * visus::dataverse::convert
 */
template<class TChar>
std::size_t visus::dataverse::convert(
        _Out_writes_to_opt_(cnt_dst, return) TChar *dst,
        _In_ const std::size_t cnt_dst,
        _In_reads_or_z_(cnt_src) const TChar *src,
        _In_ int cnt_src,
        _In_ const narrow_string::code_page_type code_page) {
    if (src == nullptr) {
        throw std::invalid_argument("The string to convert cannot be null.");
    }

    auto retval = cnt_src;
    auto len = cnt_src;

    if (len == 0) {
        retval = std::char_traits<TChar>::length(src);
        len = retval + 1;
    }

    if (len > cnt_dst) {
        len = cnt_dst;
    }

    if (dst != nullptr) {
        ::memcpy(dst, src, len * sizeof(TChar));
    }

    return retval;
}
