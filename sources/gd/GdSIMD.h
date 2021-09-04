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

#pragma once
#include <simde/simde-features.h>
#include <simde/x86/sse2.h>

HEDLEY_ALWAYS_INLINE simde__m128 ternaryPS(simde__m128 c, simde__m128 a, simde__m128 b)
{
    return simde_mm_or_ps(simde_mm_and_ps(c, a), simde_mm_andnot_ps(c, b));
}

HEDLEY_ALWAYS_INLINE simde__m128i ternaryEPI32(simde__m128i c, simde__m128i a, simde__m128i b)
{
    return simde_mm_castps_si128(
        ternaryPS(simde_mm_castsi128_ps(c), simde_mm_castsi128_ps(b), simde_mm_castsi128_ps(a)));
}

HEDLEY_ALWAYS_INLINE simde__m128 lerpPS(const float *table, simde__m128i i0, simde__m128 mu)
{
    alignas(simde__m128i) int j0[4];
    simde_mm_store_si128((simde__m128i *)j0, i0);

    // reference: Interpolated table lookups using SSE2 [2/2]
    // https://rawstudio.org/blog/?p=482
    simde__m128 p0p1 = simde_mm_castsi128_ps(simde_mm_loadl_epi64((simde__m128i *)&table[j0[0]]));
    simde__m128 p2p3 = simde_mm_castsi128_ps(simde_mm_loadl_epi64((simde__m128i *)&table[j0[2]]));
    p0p1 = simde_mm_loadh_pi(p0p1, (simde__m64*)&table[j0[1]]);
    p2p3 = simde_mm_loadh_pi(p2p3, (simde__m64*)&table[j0[3]]);
    simde__m128 y0 = simde_mm_shuffle_ps(p0p1, p2p3, SIMDE_MM_SHUFFLE(2, 0, 2, 0));
    simde__m128 y1 = simde_mm_shuffle_ps(p0p1, p2p3, SIMDE_MM_SHUFFLE(3, 1, 3, 1));

    simde__m128 dy = simde_mm_sub_ps(y1, y0);
    return simde_mm_add_ps(y0, simde_mm_mul_ps(mu, dy));
}
