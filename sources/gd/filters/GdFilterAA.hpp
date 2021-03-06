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

#include "GdFilterAA.h"
#include <cstring>

inline void GdFilterAA::clear()
{
    std::memset(&sec_, 0, NS * sizeof(Section));
}

inline float GdFilterAA::getSampleRate() const
{
    return sampleRate_;
}

inline float GdFilterAA::processOne(float input)
{
    float output = input;

    for (unsigned nthSection = 0; nthSection < NS; ++nthSection) {
        Section &sec = sec_[nthSection];
        const float *sc = coeffs_ + 5 * nthSection;

        float b0 = sc[0], b1 = sc[1], b2 = sc[2];
        float a1 = sc[3], a2 = sc[4];

        float in = output;
        float out = sec.s1 + b0 * in;
        sec.s1 = sec.s2 + b1 * in - a1 * out;
        sec.s2 = b2 * in - a2 * out;

        output = out;
    }

    float k = coeffs_[NS * 5];
    return k * output;
}
