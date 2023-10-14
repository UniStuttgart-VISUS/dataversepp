// <copyright file="unique_variable.h" company="Visualisierungsinstitut der Universität Stuttgart">
// Copyright © 2021 - 2023 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
// </copyright>
// <author>Christoph Müller</author>


#define _DATAVERSE_CONCAT0(l, r) l##r
#define _DATAVERSE_CONCAT(l, r) _DATAVERSE_CONCAT0(l, r)

// Andrei Alexandrescu's unique variable generator.
#if defined(__COUNTER__)
#define _DATAVERSE_UNIQUE_VARIABLE(p) _DATAVERSE_CONCAT(p, __COUNTER__)
#else /* defined(__COUNTER__) */
#define _DATAVERSE_UNIQUE_VARIABLE(p) _DATAVERSE_CONCAT(p, __LINE__)
#endif /* defined(__COUNTER__) */
