/* Delay Architect
 * Copyright (C) 2021  Jean Pierre Cimalando
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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
