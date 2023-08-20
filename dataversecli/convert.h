// <copyright file="convert.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>

#include "dataverse/convert.h"

#if defined(_WIN32)
#include <Windows.h>
#include <tchar.h>

#if (defined(UNICODE) || defined(_UNICODE))
#define _TT(s) _T(s)
#define _TTS(s) _T(s)
#define _MAKE_NARROW_VECTOR(v)
#define _TTV(v) v
#else /* (defined(UNICODE) || defined(_UNICODE)) */
#define _TT(s) visus::dataverse::make_narrow_string(s, CP_OEMCP)
#define _TTS(s) _TT(s.c_str())
#define _MAKE_NARROW_VECTOR(v) const auto narrow_##v = \
    visus::dataverse::make_narrow_strings(v.begin(), v.end(), CP_OEMCP)
#define _TTV(v) narrow_##v
#endif /* (defined(UNICODE) || defined(_UNICODE)) */

#else /* defined(_WIN32) */
#define TCHAR char
#define _T(s) (s)
#define _TT(s) visus::dataverse::make_narrow_string(s, nullptr)
#define _TTS(s) _TT(s.c_str())
#define _MAKE_NARROW_VECTOR(v) const auto narrow_##v = \
    visus::dataverse::make_narrow_strings(v.begin(), v.end(), nullptr)
#define _TTV(v) narrow_##v
#endif /* defined(_WIN32) */
