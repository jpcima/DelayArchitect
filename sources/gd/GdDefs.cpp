// SPDX-License-Identifier: AGPL-3.0-or-later
#include "GdDefs.h"
#include <algorithm>

int GdFindNearestDivisor(float div)
{
    const int *begin = GdGridDivisors;
    const int *end = begin + GdNumGridDivisors;
    const int *pos = std::lower_bound(begin, end, div);
    if (pos == begin)
        return *pos;
    if (pos == end)
        return end[-1];
    int div1 = *pos;
    int div2 = pos[-1];
    return (div1 - div > div - div2) ? div2 : div1;
}
