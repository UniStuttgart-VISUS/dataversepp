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
        _In_ std::size_t cnt_src,
        _In_ const narrow_string::code_page_type code_page) {
    if (src == nullptr) {
        throw std::invalid_argument("The string to convert cannot be null.");
    }

    if (cnt_src == 0) {
        cnt_src = std::char_traits<TChar>::length(src) + 1;
    }

    if (dst != nullptr) {
        ::memcpy(dst, src, (std::min)(cnt_dst, cnt_src));
    }

    return cnt_src;
}
